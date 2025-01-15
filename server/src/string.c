#include <mmo/string.h>

#include <stdio.h>
#include <math.h>
#include "mmo/char_arr_list.h"
#include "mmo/char_arr_view_list.h"

int mmo_string_split(mmo_char_arr_view_t text, char delimiter, mmo_char_arr_view_list_t *out) {
    assert(out);

    mmo_char_arr_view_list_new(out);

    char *start       = text.elems;
    char *end_of_text = &text.elems[text.num_elems];

    while (true) {
        /* Iterate past all delimiters. */
        while (start < end_of_text && *start == delimiter) {
            start += 1;
        }

        char *end = start;

        /* Iterate until end of word. */
        while (end < end_of_text && *end != delimiter) {
            end += 1;
        }

        /* If token is empty, stop splitting. */
        if (start == end) {
            return 0;
        }

        /* Create new token and add it to list. */

        mmo_char_arr_view_t token;
        token.elems     = start;
        token.num_elems = (size_t)(end - start);

        if (mmo_char_arr_view_list_append(out, token) == -1) {
            return -1;
        }
    }

    return 0;
}

static void mmo_insert_word(mmo_node_t **words, size_t *num_words, mmo_node_t **word,
                            size_t *line_pos, size_t *num_chars) {
    words[*num_words]  = *word;
    *num_words        += 1;

    /* +1 for the required minimum of one following space. Used for calculation. */
    *line_pos  += (*word)->word.len + 1;
    *num_chars += (*word)->word.len;

    *word = (*word)->next;
}

int mmo_string_justify_and_hyphenate(mmo_char_arr_view_t in, int width, mmo_char_arr_arr_t *lines) {
    assert(lines);
    assert(width > 0);

    mmo_char_arr_arr_new(lines);

    /* Split up text into words. */

    mmo_char_arr_view_list_t words;

    if (mmo_string_split(in, ' ', &words) == -1) {
        return -1;
    }

    mmo_char_arr_view_list_node_t *word = words.head;

    /* For each line. */
    while (word) {
        mmo_char_arr_list_t

        size_t i                   = 0;
        size_t num_non_space_chars = 0;

        /* If word fits on line, add it. Repeat. */
        while (word && i + word->data.num_elems <= width) {
            /* +1 for the required minimum of one following space. Used for calculation. */
            i                   += word->data.num_elems + 1;
            num_non_space_chars += word->data.num_elems;



            word = word->next;
        }
    }

    /* For each word. */
    for (mmo_char_arr_view_list_node_t *word = words.head; word; word = word->next) {
    }

    /* For each line. */
    while (word) {
        /* Array of words that fit on line. */
        mmo_node_t *words[100];
        size_t num_words = 0;

        size_t line_pos  = 0;
        /* Number of non-space characters on line. */
        size_t num_chars = 0;

        /* If word fits on line, add to array. Repeat. */
        while (word && line_pos + word->word.len <= width) {
            mmo_insert_word(words, &num_words, &word, &line_pos, &num_chars);
        }

        /* If 0 or 1 words fit on line, hyphenate. */

        size_t num_spaces = width - num_chars;

        if (word && num_words < 2 && num_spaces >= 3) {
            size_t hyph_base_len = width - line_pos;

            /* Insert node with remainder of hyphenated word into list. */

            size_t hyph_remainder_len = word->word.len - hyph_base_len + 1;

            mmo_node_t *new_elem = malloc(sizeof(mmo_node_t));
            new_elem->word.ptr   = malloc(sizeof(hyph_remainder_len));
            strncpy(new_elem->word.ptr, word->word.ptr + hyph_base_len - 1, hyph_remainder_len);
            new_elem->word.len = hyph_remainder_len;

            new_elem->next = word->next;
            word->next     = new_elem;

            /* Add hyphen to base. Add base to array of words that fit on line. */

            word->word.ptr[hyph_base_len - 1] = '-';
            word->word.len                    = hyph_base_len;

            mmo_insert_word(words, &num_words, &word, &line_pos, &num_chars);
        }

        /* Distribute spaces evenly between words. */

        int num_total_spaces = width - num_chars;
        int num_used_spaces  = 0;

        for (size_t i = 0; i < num_words; i += 1) {
            strncpy(result.ptr + result_pos, words[i]->word.ptr, words[i]->word.len);
            result_pos += words[i]->word.len;

            if (i == num_words - 1) {
                break;
            }

            if (i == num_words - 2) {
                num_spaces = num_total_spaces - num_used_spaces;
            } else {
                num_spaces       = (int)round((double)num_total_spaces / (num_words - 1));
                num_used_spaces += num_spaces;
            }

            for (int j = 0; j < num_spaces; j += 1) {
                result.ptr[result_pos]  = ' ';
                result_pos             += 1;
            }
        }

        strncpy(result.ptr + result_pos, "\r\n", 2);
        result_pos += 2;
    }

    mmo_list_free(head);
    head = NULL;

    result.ptr[result_pos]  = '\0';
    result_pos             += 1;

    result.ptr = realloc(result.ptr, result_pos);
    result.len = result_pos;

    return result;
}
