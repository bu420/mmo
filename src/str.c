#include <mmo/str.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* Linked list of words. */
typedef struct mmo_node_s {
    struct mmo_node_s *next;
    mmo_str_t word;
} mmo_node_t;

static mmo_str_t mmo_next_word(const mmo_str_t text, mmo_str_t start) {
    /* Go to end of previous word. */
    start.ptr += start.len;
    size_t i = start.ptr - text.ptr;
    
    /* Iterate past all spaces. */
    while (i < text.len && *start.ptr == ' ') {
        start.ptr += 1;
        i += 1;
    }

    const char *end = start.ptr;

    /* Iterate until end of word. */
    while (*end != ' ') {
        if (i == text.len) {
            if (start.ptr == end) {
                return (mmo_str_t){ .ptr = NULL, .len = 0 };
            }
            
            break;
        }

        end += 1;
        i += 1;
    }

    start.len = end - start.ptr;

    return start;
}

static mmo_node_t *mmo_str_split(const mmo_str_t text) {
    mmo_node_t *head = NULL;
    mmo_node_t *tail = NULL;

    mmo_str_t word = mmo_next_word(text, (mmo_str_t){ .ptr = text.ptr, .len = 0 });

    while (word.ptr) {
        mmo_node_t *new_elem = malloc(sizeof(mmo_node_t));
        new_elem->next = NULL;

        /* Copy word. */
        new_elem->word.ptr = malloc(word.len);
        strncpy(new_elem->word.ptr, word.ptr, word.len);
        new_elem->word.len = word.len;

        /* Insert new element into list. */
        if (!head) {
            head = new_elem;
            tail = new_elem;
        }
        else {
            tail->next = new_elem;
            tail = tail->next;
        }

        word = mmo_next_word(text, word);
    }

    return head;
}

static void mmo_list_free(mmo_node_t *head) {
    while (head) {
        mmo_node_t *temp = head;
        head = head->next;

        free(temp->word.ptr);
        temp->word.ptr = NULL;

        free(temp);
        temp = NULL;
    }
}

static void mmo_insert_word(mmo_node_t **words, size_t *num_words, mmo_node_t **word, size_t *line_pos, size_t *num_chars) {
    words[*num_words] = *word;
    *num_words += 1;

    /* +1 for the required minimum of one following space. Used for calculation. */
    *line_pos += (*word)->word.len + 1;
    *num_chars += (*word)->word.len;

    *word = (*word)->next;
}

int mmo_justify_and_hyphenate(const mmo_str_t in, int width, mmo_str_t *out) {
    mmo_str_t result;
    result.len = 4 * text.len;
    result.ptr = malloc(result.len);

    size_t result_pos = 0;

    mmo_node_t *head = mmo_str_split(text);
    mmo_node_t *word = head;

    while (word) {
        /* Array of words that fit on line. */
        mmo_node_t *words[100];
        size_t num_words = 0;

        size_t line_pos = 0;
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
            new_elem->word.ptr = malloc(sizeof(hyph_remainder_len));
            strncpy(new_elem->word.ptr, word->word.ptr + hyph_base_len - 1, hyph_remainder_len);
            new_elem->word.len = hyph_remainder_len;

            new_elem->next = word->next;
            word->next = new_elem;

            /* Add hyphen to base. Add base to array of words that fit on line. */

            word->word.ptr[hyph_base_len - 1] = '-';
            word->word.len = hyph_base_len;

            mmo_insert_word(words, &num_words, &word, &line_pos, &num_chars);
        }

        /* Distribute spaces evenly between words. */

        int num_total_spaces = width - num_chars;
        int num_used_spaces = 0;

        for (size_t i = 0; i < num_words; i += 1) {
            strncpy(result.ptr + result_pos, words[i]->word.ptr, words[i]->word.len);
            result_pos += words[i]->word.len;

            if (i == num_words - 1) {
                break;
            }

            if (i == num_words - 2) {
                num_spaces = num_total_spaces - num_used_spaces;
            }
            else {
                num_spaces = (int)round((double)num_total_spaces / (num_words - 1));
                num_used_spaces += num_spaces;
            }

            for (int j = 0; j < num_spaces; j += 1) {
                result.ptr[result_pos] = ' '; 
                result_pos += 1;
            }
        }

        strncpy(result.ptr + result_pos, "\r\n", 2);
        result_pos += 2;
    }

    mmo_list_free(head);
    head = NULL;

    result.ptr[result_pos] = '\0'; 
    result_pos += 1;

    result.ptr = realloc(result.ptr, result_pos);
    result.len = result_pos;

    return result;
}
