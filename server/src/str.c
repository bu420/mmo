#include <mmo/str.h>

#include <stdio.h>
#include <math.h>
#include <mmo/arr/char.h>
#include <mmo/arr/char_arr.h>
#include <mmo/list/char_span.h>

void mmo_string_split(mmo_char_span_t text, char delimiter, mmo_char_span_list_t *out) {
    assert(out);

    mmo_char_span_list_new(out);

    const char *start       = text.elems;
    const char *end_of_text = &text.elems[text.num_elems];

    while (true) {
        /* Iterate past all delimiters. */
        while (start < end_of_text && *start == delimiter) {
            start += 1;
        }

        const char *end = start;

        /* Iterate until end of word. */
        while (end < end_of_text && *end != delimiter) {
            end += 1;
        }

        /* If token is empty, stop splitting. */
        if (start == end) {
            return;
        }

        /* Create new token and add it to list. */

        mmo_char_span_t token;
        token.elems     = start;
        token.num_elems = (size_t)(end - start);

        mmo_char_span_list_append(out, &token);
    }
}

void mmo_string_justify_and_hyphenate(mmo_char_span_t in, int width, mmo_char_arr_arr_t *lines) {
    assert(lines);
    assert(width > 0);

    mmo_char_arr_arr_new(lines);

    /* Split up text into words. */

    mmo_char_span_list_t words;
    mmo_string_split(in, ' ', &words);

    mmo_char_span_list_node_t *word = words.head;

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
            mmo_char_arr_new(&new_word);
            mmo_char_arr_append_arr(&new_word, (mmo_char_arr_t *)&word->data);

            /* Append. */
            mmo_char_arr_arr_append(&words_on_line, &new_word);

            word = word->next;
        }

        /* If 0 or 1 words fit on line, hyphenate. */

        int num_spaces = width - num_visible_chars;

        if (word && words_on_line.num_elems < 2 && num_spaces >= 3) {
            /* Make copy of base, add hyphen and add it to array of words that fit on line. */

            int hyph_base_len = width - pos;

            mmo_char_arr_t base;
            mmo_char_arr_new(&base);
            mmo_char_arr_append_arr(&base, (mmo_char_arr_t *)&word->data);

            base.elems[hyph_base_len - 1] = '-';
            base.num_elems                = (size_t)hyph_base_len;
            base.max_elems                = (size_t)hyph_base_len;

            mmo_char_arr_arr_append(&words_on_line, &base);

            /* Copy current word view, offset it to point to remainder and insert new view into
               list of all words to be processed next iteration. */

            mmo_char_span_t remainder  = word->data;
            remainder.elems           += hyph_base_len + 1;
            remainder.num_elems       -= ((size_t)hyph_base_len + 1);

            mmo_char_span_list_insert_after_node(&words, &remainder, word);
        }

        /* Distribute spaces evenly between words. */

        int num_total_spaces = width - num_visible_chars;
        int num_used_spaces  = 0;

        mmo_char_arr_t line;
        mmo_char_arr_new(&line);

        mmo_char_arr_resize(&line, (size_t)width);

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

        mmo_char_arr_arr_append(lines, &line);
    }

    mmo_char_span_list_free(&words);
}
