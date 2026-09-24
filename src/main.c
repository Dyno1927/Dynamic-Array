// main.c — the test harness. Drive the Vec and prove it behaves.
#include <stdio.h>

#include "vec.h" // my own file → quotes

int main(void) {
    Vec numbers; // a real Vec crate on the stack — fields are garbage until init
    vec_init(&numbers); // & = pass the crate's ADDRESS so vec_init can
                        //    touch the real one (not a copy) and empty it

    // Push 10 numbers: more than the starting room of 4, which forces
    // the doubling to kick in (4 → 8 → 16). printed after every push.
    for (int i = 0; i < 10; i++) {
        vec_push(&numbers, i * 10);
        printf("used: %d | room: %d\n", numbers.used, numbers.room);
    }

    // Read the whole thing back and print each number.
    // I have the real crate here (`numbers`), not a pointer,
    // so I use plain `.` instead of `->`.
    for (int i = 0; i < numbers.used; i++) {
        printf("%d\n", numbers.items[i]);
    }

    vec_free(&numbers); // give the heap chunk back so nothing leaks

    return 0;
}