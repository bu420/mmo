#include <mmo/string.h>

#include <stdio.h>
#include <math.h>
#include "mmo/char_arr.h"
#include "mmo/char_arr_arr.h"
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
        mmo_char_arr_arr_t words_on_line;
        mmo_char_arr_arr_new(&words_on_line);

        int pos               = 0;
        /* Number of non-space characters. */
        int num_visible_chars = 0;

        /* If word fits on line, add it. Repeat. */
        while (word && (size_t)pos + word->data.num_elems <= (size_t)width) {
            /* +1 for the required minimum of one following space. Used for calculation. */
            pos               += (int)word->data.num_elems + 1;
            num_visible_chars += (int)word->data.num_elems;

            /* Copy word. */
            mmo_char_arr_t new_word;
            if (mmo_char_arr_new_from_view(&new_word, word->data) == -1) {
                return -1;
            }

            /* Append. */
            if (mmo_char_arr_arr_append(&words_on_line, new_word) == -1) {
                return -1;
            }

            word = word->next;
        }

        /* If 0 or 1 words fit on line, hyphenate. */

        int num_spaces = width - num_visible_chars;

        if (word && words_on_line.num_elems < 2 && num_spaces >= 3) {
            /* Make copy of base, add hyphen and add it to array of words that fit on line. */

            int hyph_base_len = width - pos;

            mmo_char_arr_t base;
            if (mmo_char_arr_new_from_view(&base, word->data) == -1) {
                return -1;
            }

            base.elems[hyph_base_len - 1] = '-';
            base.num_elems                = (size_t)hyph_base_len;
            base.max_elems                = (size_t)hyph_base_len;

            if (mmo_char_arr_arr_append(&words_on_line, base) == -1) {
                return -1;
            }

            /* Copy current word view, offset it to point to remainder and insert new view into
               list of all words to be processed next iteration. */

            mmo_char_arr_view_t remainder  = word->data;
            remainder.elems               += hyph_base_len + 1;
            remainder.num_elems           -= ((size_t)hyph_base_len + 1);

            if (mmo_char_arr_view_list_insert_after_node(&words, remainder, word) == -1) {
                return -1;
            }
        }

        /* Distribute spaces evenly between words. */

        int num_total_spaces = width - num_visible_chars;
        int num_used_spaces  = 0;

        mmo_char_arr_t line;
        mmo_char_arr_new(&line);

        if (mmo_char_arr_resize(&line, (size_t)width) == -1) {
            return -1;
        }

        pos = 0;

        for (size_t i = 0; i < words_on_line.num_elems; i += 1) {
            strncpy(line.elems + pos, words_on_line.elems[i].elems,
                    words_on_line.elems[i].num_elems);
            pos += (int)words_on_line.elems[i].num_elems;

            if (i == words_on_line.num_elems - 1) {
                break;
            }

            if (i == words_on_line.num_elems - 2) {
                num_spaces = num_total_spaces - num_used_spaces;
            } else {
                num_spaces =
                    (int)round((float)num_total_spaces / ((float)words_on_line.num_elems - 1));
                num_used_spaces += num_spaces;
            }

            for (int j = 0; j < num_spaces; j += 1) {
                line.elems[pos]  = ' ';
                pos             += 1;
            }
        }

        if (mmo_char_arr_arr_append(lines, line) == -1) {
            return -1;
        }
    }

    mmo_char_arr_view_list_free(&words);

    return 0;
}
