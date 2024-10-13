#include <stdio.h>
#include <string.h>
#include <math.h>

int main() {
    int width = 25;
    char text[] = "Lorem ipsum odor amet, consectetuer adipiscing elit. Elit sed cras ac rutrum at ut purus velit convallis. Iaculis iaculis inceptos lacinia sapien risus cubilia fringilla viverra congue. Sociosqu torquent nulla sagittis parturient vulputate id turpis mollis dis. Non orci diam cubilia consectetur ornare nunc. Id ligula dignissim morbi lacinia pharetra efficitur donec quis.";

    char *word = strtok(text, " ");
    while (word) {
        /* Find words that fit on line. */

        char words[10][30];
        int num_words = 0;
        int num_non_space_chars = 0;
        int x = 0;

        while (word) {
            int len = strlen(word);

            if (x + len > width) {
                break;
            }

            strcpy(words[num_words++], word);
            num_non_space_chars += len;
            /* +1 for the space. */
            x += len + 1;

            word = strtok(NULL, " \r\n\t");
        }

        /* Format words by adding space inbetween. */

        int num_total_spaces = width - num_non_space_chars;
        int num_used_spaces = 0;

        for (int i = 0; i < num_words; ++i) {
            printf("%s", words[i]);

            if (i == num_words - 1) {
                break;
            }

            int num_spaces;

            if (i == num_words - 2) {
                num_spaces = num_total_spaces - num_used_spaces;
            }
            else {
                num_spaces = (int)round((double)num_total_spaces / (num_words - 1));
                num_used_spaces += num_spaces;
            }

            for (int j = 0; j < num_spaces; ++j) {
                putchar(' ');
            }
        }
        printf("\n");
    }
}