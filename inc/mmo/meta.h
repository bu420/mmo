#ifndef MMO_META_H
#define MMO_META_H

#define mmo_concat2(A, B) A##B
#define mmo_concat(A, B)  mmo_concat2(A, B)

/* Generate unique name for internal macro identifiers. */
#define mmo_unique(NAME) mmo_concat(NAME, __LINE__)

#if !defined(NDEBUG) && defined(__GNUC__)
#define mmo_static_assert(COND, MSG)                                           \
    __attribute__((unused)) typedef char mmo_unique(                           \
        mmo_static_assert)[(COND) ? 1 : -1]
#else
#define mmo_static_assert(COND, MSG)
#endif

/* Assign pointer to pointer without type cast. */
#define mmo_generic_assign(A, B)                                               \
    do {                                                                       \
        void *mmo_unique(m) = B;                                               \
        memcpy(&A, &mmo_unique(m), sizeof(void *));                            \
    } while (0)

#endif
