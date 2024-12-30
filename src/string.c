#include <mmo/string.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

MMO_ARR_DEF(char, mmo_string);

/* Get the next word in a string with words separated by spaces. */
static bool mmo_next_word(mmo_string_view_t text, mmo_string_view_t prev_word,
                          mmo_string_view_t *new_word) {
    /* Start at the end of the previous word. */
    const char *start = prev_word.elems + prev_words.num_elems;

    ptrdiff_t i = start - text.elems;

    /* Iterate past all spaces. */
    while (i < text.num_elems && *start == ' ') {
        start += 1;
        i     += 1;
    }

    const char *end = start;

    /* Iterate until end of word. */
    while (*end != ' ') {
        if (i == text.num_elems) {
            if (start == end) {
                return false;
            }

            break;
        }

        end += 1;
        i   += 1;
    }

    new_word->elems     = start;
    new_word->num_elems = end - start;

    return true;
}

/* Linked list of words. */
typedef struct mmo_node_s {
    struct mmo_node_s *next;
    mmo_string_t word;
} mmo_node_t;

/* Split string by spaces. */
static int mmo_string_split(mmo_string_view_t string, mmo_node_t **out) {
    mmo_node_t *head = NULL;
    mmo_node_t *tail = NULL;

    mmo_string_view_t word = {.elems = string.elems, .num_elems = 0};

    while (mmo_next_word(string, word, &word)) {
        mmo_node_t *new_node = malloc(sizeof(mmo_node_t));
        new_node->next       = NULL;

        /* Copy word. */

        if (mmo_string_new(&new_node->word, 0) == -1) {
            return -1;
        }

        for (size_t i = 0; i < word.num_elems; i += 1) {
            if (mmo_string_append(&new_node->word, word.elems[i]) == -1) {
                return -1;
            }
        }

        /* Insert new node into list. */

        if (!head) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail       = tail->next;
        }
    }

    *out = head;

    return 0;
}

static void mmo_list_free(mmo_node_t *head) {
    while (head) {
        mmo_node_t *temp = head;
        head             = head->next;

        mmo_string_free(&temp->word);

        free(temp);
        temp = NULL;
    }
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

int mmo_justify_and_hyphenate(mmo_string_view_t in, int width, mmo_string_t *out) {
    assert(out);

    if (width <= 0) {
        return -1;
    }

    if (mmo_string_new(out, 0) == -1) {
        return -1;
    }

    mmo_node_t *head;

    if (mmo_str_split(text, &head) == -1) {
        return -1;
    }

    mmo_node_t *word = head;

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
