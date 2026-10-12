// test.c — the test harness. One test per Vec function, proving the whole
// API behaves. Same idea as my old main.c, but now each function gets its
// own test instead of one long demo, and the checks assert instead of just
// printing, so a broken build fails with a non-zero exit code.
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>  // waitpid — so I can test the functions that exit()
#include <unistd.h>    // fork — run the abort paths in a child process

#include "vec.h"  // my own file → quotes

// Counts the failed checks so main can report a pass/fail summary.
static int failures = 0;

// CHECK — if `cond` is false, say where and bump the count. I don't stop on
// the first failure, so one run shows everything that's broken at once.
#define CHECK(cond)                                                         \
    do {                                                                    \
        if (!(cond)) {                                                      \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            failures++;                                                     \
        }                                                                   \
    } while (0)

// vec_init — a fresh crate must come back empty: no heap, nothing used,
// no seats.
static void test_init(void) {
    Vec v;  // fields are garbage until init touches them
    vec_init(&v);

    CHECK(v.items == NULL);  // no chunk asked for yet
    CHECK(v.used == 0);      // zero numbers in it
    CHECK(v.room == 0);      // and zero seats, since there's no chunk
}

// vec_push — appends happen in order, `used` tracks them, and pushing past
// the starting room of 4 forces the doubling (4 → 8 → 16).
static void test_push(void) {
    Vec v;
    vec_init(&v);

    vec_push(&v, 10);
    vec_push(&v, 20);
    vec_push(&v, 30);
    CHECK(v.used == 3);
    CHECK(v.room == 4);  // first push asked the heap for 4 seats
    CHECK(v.items[0] == 10);
    CHECK(v.items[1] == 20);
    CHECK(v.items[2] == 30);

    // Fill to 10: crossed 4 (→8) and then 8 (→16).
    for (int i = 3; i < 10; i++) {
        vec_push(&v, i);
    }
    CHECK(v.used == 10);
    CHECK(v.room == 16);     // 4 → 8 → 16, exactly two grows
    CHECK(v.items[9] == 9);  // the last one landed in the right slot

    vec_free(&v);
}

// vec_get — reads back exactly what was pushed, by index.
static void test_get(void) {
    Vec v;
    vec_init(&v);
    for (int i = 0; i < 5; i++) {
        vec_push(&v, i * 100);
    }

    CHECK(vec_get(&v, 0) == 0);
    CHECK(vec_get(&v, 2) == 200);
    CHECK(vec_get(&v, 4) == 400);  // last filled seat is used-1

    vec_free(&v);
}

// vec_set — overwrites one slot in place and leaves the size alone.
static void test_set(void) {
    Vec v;
    vec_init(&v);
    vec_push(&v, 1);
    vec_push(&v, 2);
    vec_push(&v, 3);

    vec_set(&v, 1, 999);
    CHECK(vec_get(&v, 1) == 999);  // the write stuck
    CHECK(vec_get(&v, 0) == 1);    // neighbours untouched
    CHECK(vec_get(&v, 2) == 3);
    CHECK(v.used == 3);  // set does not change how many are in it

    vec_free(&v);
}

// vec_pop — last in, first out. `used` drops by one, but `room` stays big
// so repeated push/pop doesn't thrash malloc.
static void test_pop(void) {
    Vec v;
    vec_init(&v);
    vec_push(&v, 1);
    vec_push(&v, 2);
    vec_push(&v, 3);

    CHECK(vec_pop(&v) == 3);
    CHECK(vec_pop(&v) == 2);
    CHECK(v.used == 1);  // two gone
    CHECK(v.room == 4);  // capacity stayed put — pop never shrinks
    CHECK(vec_pop(&v) == 1);
    CHECK(v.used == 0);  // empty crate again

    vec_free(&v);
}

// vec_free — hands the chunk back and leaves the crate looking empty, so a
// second free is harmless (free(NULL) is a no-op).
static void test_free(void) {
    Vec v;
    vec_init(&v);
    vec_push(&v, 42);

    vec_free(&v);
    CHECK(v.items == NULL);
    CHECK(v.used == 0);
    CHECK(v.room == 0);

    vec_free(&v);  // double free must not blow up
}

// The bounds-checked ops call exit() instead of quietly returning, so I
// can't test them in-process — a child process runs the bad call and the
// parent inspects the exit code.

typedef void (*test_fn)(void);

static void expect_exit_failure(const char* what, test_fn body) {
    fflush(NULL);  // don't let the parent's buffered output leak into the child

    pid_t pid = fork();
    if (pid == 0) {
        // Child: the op is supposed to abort, and it prints a message to
        // stderr first — bin that so a passing run stays quiet. Reaching
        // _exit(0) means it did NOT abort, which is the failure.
        (void)freopen("/dev/null", "w", stderr);
        body();
        _exit(0);
    }

    int status = 0;
    waitpid(pid, &status, 0);
    if (!(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)) {
        fprintf(stderr, "FAIL: %s should have exited with failure\n", what);
        failures++;
    }
}

// index == used is off the end (valid seats are 0 .. used-1). These use an
// empty vec on purpose: no heap is ever allocated, so the sanitizer build
// doesn't trip its leak check when the child exits.
static void get_out_of_range(void) {
    Vec v;
    vec_init(&v);
    vec_get(&v, 0);  // aborts
}

static void set_out_of_range(void) {
    Vec v;
    vec_init(&v);
    vec_set(&v, 0, 1);  // aborts
}

static void pop_empty(void) {
    Vec v;
    vec_init(&v);
    vec_pop(&v);  // aborts — an empty crate has no last item
}

int main(void) {
    test_init();
    test_push();
    test_get();
    test_set();
    test_pop();
    test_free();

    expect_exit_failure("vec_get out of range", get_out_of_range);
    expect_exit_failure("vec_set out of range", set_out_of_range);
    expect_exit_failure("vec_pop on empty", pop_empty);

    if (failures == 0) {
        printf("all tests passed\n");
        return 0;
    }
    printf("%d check(s) failed\n", failures);
    return 1;
}
