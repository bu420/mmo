#ifndef MMO_META_H
#define MMO_META_H

#define mmo_concat2(A, B) A##B
#define mmo_cancat(A, B)  mmo_concat2(A, B)

/* Generate unique names for internal macro identifiers to avoid name collision
   if you call the macro more than once in the scope or function. */
#define mmo_unique(NAME) mmo_cancat(NAME, __LINE__)

/* Assign pointer of any type to pointer of any type without cast. */
#define mmo_generic_assign(A, B)                                               \
    do {                                                                       \
        void *mmo_unique(m) = B;                                               \
        memcpy(&A, &mmo_unique(m), sizeof(void *));                            \
    } while (0)

#endif
