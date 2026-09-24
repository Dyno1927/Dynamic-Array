# Dynamic Array (C)

My C learning project — a growing array (vector) written from scratch. It's
the project that finally got me into pointers and heap memory. Fixed-size
arrays (like the `vault[1024]` in my password manager) can't grow; this one
can. It starts with room for 4 and doubles whenever it fills up.

## What I learned here

- pointers and `->` vs `.`
- `malloc` / `realloc` / `free` — my first time owning heap memory
- automatic resizing by doubling the capacity (so pushing n things is
  amortized O(1))
- header + source split: `vec.h` is the menu (struct + prototypes), `vec.c`
  is the kitchen (the implementation), `main.c` is the test harness
- `#include "..."` vs `#include <...>`, header guards
- a Makefile that builds the `.o` files and links them
- why ASan exists: once it caught a heap-buffer-overflow I didn't even know
  I had, then caught a leak. Run the sanitizer build, learn the bug is real.

## Layout

```
Dynamic-Array/
├── include/vec.h   # the Vec struct + prototypes (the API)
├── src/vec.c       # the implementation
├── src/main.c      # test harness / demo
├── build/          # objects + binary (gitignored)
├── Makefile
└── README.md
```

## The Vec struct

```c
typedef struct {
    int* items; // the actual numbers, living on the heap
    int  used;  // how many slots are filled right now
    int  room;  // total seats the heap chunk can hold
} Vec;
```

`room` vs `used`: capacity vs contents. Pushing only fills `used`; `room`
only jumps when the bus is full.

## Build

```sh
make && ./build/vec
make clean
```

With sanitizers (should report zero errors / leaks):

```sh
make clean && make CFLAGS="-fsanitize=address,undefined -g -O0 -Wall -Wextra -Iinclude"
```

## Status

Day 1 done: `vec_init`, `vec_push`, `vec_free` — working, ASan-clean,
pushed to GitHub. Still to write: `vec_get` / `vec_set` / `vec_pop`
(prototypes already waiting in `vec.h`).