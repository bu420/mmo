#ifndef MMO_ARR_CHAR_H
#define MMO_ARR_CHAR_H

#include <mmo/arr/template.h>

#define MMO_LITERAL_TO_SPAN(l)                                                 \
    (mmo_char_span_t) { l, strlen(l) }

#define MMO_ARR_TO_SPAN(a)                                                     \
    (mmo_char_span_t) { a.elems, a.num_elems }

MMO_ARR_DECL(char, mmo_char)

#endif
