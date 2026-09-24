// vec.h — the menu. Only the API lives here: the struct and the function
// prototypes. No implementation. main.c and vec.c both #include this so
// they agree on what a Vec is and what the functions look like.
//
// The header guard (next two lines + #endif at the bottom) makes this file
// safe to include multiple times. #include copy-pastes text, and if this
// got pasted twice the struct would be defined twice → compile error.
// First include defines VEC_H, every include after that sees it defined
// and skips the whole file.
#ifndef VEC_H
#define VEC_H

// A Vec is three things in one crate:
typedef struct {
    int* items; // items: pointer to the actual numbers, on the HEAP.
                //        starts NULL (points nowhere) until first push
                //        mallets a chunk for it.
    int  used;  // used: how many slots are filled right now
                //       (passengers on the bus).
    int  room;  // room:  total seats the heap chunk can hold, filled or not.
                //        grows 4 → 8 → 16 by doubling.
} Vec;

// Prototypes = "these functions exist somewhere (vec.c), here's their
// signature so callers can use them." Main.c sees these and trusts them;
// the linker joins main.o to the real bodies in vec.o at the end.
// The * means: pass the crate's ADDRESS, so the function can modify the
// real Vec instead of a copy. Callers write vec_init(&v).
void vec_init(Vec* vec);   // empty the crate: items=NULL, used=0, room=0
void vec_push(Vec* vec,
              int value);  // append value, growing the chunk if it's full
int vec_get(Vec* vec,
            int index);    // return items[index] (bounds-checked)
                           // (not implemented yet — day 2)
void vec_set(Vec* vec, int index, int value); // not implemented yet
int vec_pop(Vec* vec);     // remove + return last item (not implemented yet)
void vec_free(Vec* vec);   // give the heap chunk back to the OS

#endif