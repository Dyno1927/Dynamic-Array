// vec.c — the kitchen. Real implementation of the API promised in vec.h.
#include "vec.h"   // my own file → quotes. Pulls in the Vec struct + prototypes.

#include <stdio.h>
#include <stdlib.h> // malloc, realloc, free, NULL

// vec_init — take a brand-new Vec (whose fields are all garbage from the
// stack) and make it a safe, empty crate: no chunk yet, nothing in it.
void vec_init(Vec* vec) {
    vec->items = NULL; // points at nothing — we haven't asked for heap yet
    vec->used  = 0;    // zero numbers in it
    vec->room  = 0;    // and zero seats, since there's no chunk at all
}

// vec_push — append `value`. If the crate is full, grow the chunk first.
void vec_push(Vec* vec, int value) {
    // Full means: every seat taken (used == room). Need a bigger bus.
    if (vec->used == vec->room) {
        // New bus size: 4 seats to start (if we never had a chunk),
        // otherwise double whatever we had. Doubling is the trick that
        // makes pushes cheap on average — grows a lot, rarely.
        int new_room = vec->room == 0 ? 4 : vec->room * 2;

        // realloc takes the chunk I already own, gets a bigger one,
        // copies my data over, frees the old one. Note: I pass
        // vec->items (the chunk), NOT vec (the struct on the stack).
        vec->items = realloc(vec->items, new_room * sizeof(int));
        vec->room = new_room; // record the new bus size
    }

    // Now there's guaranteed room. items[used] is the next free slot
    // (same shape as my old vault[account_count++] trick), then bump used.
    vec->items[vec->used] = value;
    vec->used++;
}

// vec_free — hand the heap chunk back. The OS can reuse that memory.
void vec_free(Vec* vec) {
    free(vec->items);       // the actual give-back. free(NULL) is safe, too.

    // After free() the pointer is "dangling" — it still holds the old
    // address but that memory isn't ours anymore. Set it to NULL so it
    // points nowhere again; that way nobody can use it by accident and,
    // if vec_free were ever called twice, freeing NULL is harmless.
    vec->items = NULL;
    vec->used  = 0;  // a freed crate should look empty again
    vec->room  = 0;
}