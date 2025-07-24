#ifndef AE_META_H
#define AE_META_H

#define ae_concat2(A, B) A##B
#define ae_concat(A, B)  ae_concat2(A, B)

/* Generate unique name for internal macro identifiers. */
#define ae_unique(NAME) ae_concat(NAME, __LINE__)

#if !defined(NDEBUG) && defined(__GNUC__)
#define ae_static_assert(COND, MSG)                                           \
    __attribute__((unused)) typedef char ae_unique(                           \
        ae_static_assert)[(COND) ? 1 : -1]
#else
#define ae_static_assert(COND, MSG)
#endif

/* Assign pointer to pointer without type cast. */
#define ae_generic_assign(A, B)                                               \
    do {                                                                      \
        void *ae_unique(m) = B;                                               \
        memcpy(&A, &ae_unique(m), sizeof(void *));                            \
    } while (0)

#endif
