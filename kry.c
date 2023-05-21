/*
 * project: Affine cipher
 * author: xmikul69
 * course: KRY @ FIT VUT
 * 2023
*/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/param.h>


#define MORE_FREQUENT(ch, a, b) a[ch - 'A'] > b[ch - 'A']
#define FREQUENCY_OF(ch, a) a[ch - 'A']
#define FREQUENCY_OF_REL(ch, a) (float)a[ch - 'A'] / len


void encrypt (char *plain, char *enc, unsigned int a, unsigned int b);
void decrypt (char *enc, char *plain, unsigned int a, unsigned int b);
void freq_analysis (char *enc, char *plain);

void sanitize (char *str);
unsigned mod_inv (unsigned ra, unsigned rb);
void compute_freqs (int freq[], char *input);
unsigned int heuristic (int freq[]);



// adapted from
// https://github.com/michel-leonard/C-MathSnip/blob/main/mod_inv.c
unsigned mod_inv (unsigned ra, unsigned rb) {
    unsigned rc, sa = 1, sb = 0, sc, i = 0;
    if (rb > 1) do {
            rc = ra % rb;
            sc = sa - (ra / rb) * sb;
            sa = sb, sb = sc;
            ra = rb, rb = rc;
        } while (++i, rc);
    sa *= (i *= ra == 1) != 0;
    sa += (i & 1) * sb;
    return sa;
}


void encrypt (char *plain, char *enc, unsigned int a, unsigned int b) {
    const size_t len = strlen(plain);

    #pragma omp simd
    for (size_t i = 0; i < len; i++) {
        if (isupper(plain[i]))
            enc[i] = ((a * (plain[i] - 'A') + b) % 26) + 'A';
        else
            enc[i] = plain[i];
    }
}


void decrypt (char *enc, char *plain, unsigned int a, unsigned int b) {
    size_t len = strlen(enc);

    unsigned int a_invar = mod_inv(a, 26);

    #pragma omp simd
    for (size_t i = 0; i < len; i++) {
        if (isupper(enc[i]))
            plain[i] = (((enc[i] - 'A' + 26 - b) * a_invar) % 26) + 'A';
        else
            plain[i] = enc[i];
    }
}


void compute_freqs (int freq[], char *input) {
    #pragma omp simd
    for (size_t i = 0; i < 26; i++) {
        freq[i] = 0;
    }

    size_t len = strlen(input);

    #pragma omp simd
    for (size_t i = 0; i < len; i++) {
        if (isupper(input[i]))
            freq[input[i] - 'A']++;
    }
}

void sanitize (char *str) {
    size_t len = strlen(str);

    #pragma omp simd
    for (size_t i = 0; i < len; i++) {
        if (isalpha((str)[i]))
            str[i] = toupper(str[i]);
    }
}


unsigned int heuristic (int freq[]) {
    unsigned int value = 0;

    // from
    // https://algoritmy.net/article/40/Cetnost-znaku-CJ
    float frequencies[] = {8.4548, 1.5582, 2.5557, 3.6219, 10.6751, 0.2732, 0.2729, 1.2712, 7.6227, 2.1194, 3.7367, 3.8424, 3.2267, 6.6167, 8.6977, 3.4127, 0.0013, 4.9136, 5.3212, 5.7668, 3.9421, 4.6616, 0.0088, 0.0755, 2.9814, 3.1939};

    #pragma omp simd
    for (size_t i = 0; i < 26; i++) {
        value = MIN((unsigned) (value + freq[i] * frequencies[i]), UINT_MAX);
    }

    return value;
}


void freq_analysis (char *enc, char *plain) {

    const unsigned int possible_a_values[] = {1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25};
    const unsigned int possible_a_values_count = 12;

    unsigned int best_a = possible_a_values[0];
    unsigned int best_b = 0;

    int freqs[26];
    int best_heuristic = 0;
    int current_heuristic = 0;

    for (size_t i = 0; i < possible_a_values_count; i++) {
        unsigned int curent_a = possible_a_values[i];

        for (unsigned int curent_b = 0; curent_b < 26; curent_b++) {
            decrypt(enc, plain, curent_a, curent_b);
            compute_freqs(freqs, plain);

            current_heuristic = heuristic(freqs);

            if (current_heuristic > best_heuristic) {
                best_a = curent_a;
                best_b = curent_b;
                best_heuristic = current_heuristic;
            }
        }
    }

    printf("a=%d,b=%d\n", best_a, best_b);

    decrypt(enc, plain, best_a, best_b);
}


int main (int argc, char * argv[]) {

    // getopt structure based on:
    // https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
    int eflag = 0;
    int dflag = 0;
    int cflag = 0;
    unsigned int a = 0;
    unsigned int b = 0;
    int a_specified = 0;
    int b_specified = 0;
    FILE *infile = NULL;
    FILE *outfile = NULL;

    int c;

    while ((c = getopt (argc, argv, "edca:b:o:f:")) != -1) {
        switch (c) {
            case 'e':
                eflag = 1;
                break;
            case 'd':
                dflag = 1;
                break;
            case 'c':
                cflag = 1;
                break;
            case 'a':
                a = atoi(optarg);
                a_specified = 1;
                break;
            case 'b':
                b = atoi(optarg);
                b_specified = 1;
                break;
            case 'o':
                outfile = fopen(optarg, "w");
                break;
            case 'f':
                infile = fopen(optarg, "r");
                break;
            case '?':
                if (optopt == 'c')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
                return 1;
            default:
                abort();
        }
    }


    if (eflag + dflag + cflag != 1) {
        fprintf(stderr, "Exactly one of -e -d -c must be specified!\n");
        return 1;
    }



    if (cflag) {
        if (!infile || !outfile) {
            fprintf(stderr, "Options -o and -f must be specified!\n");
            return 2;
        }

        char *enc = NULL;
        int input_len;

        if (infile) {
            fseek(infile, 0, SEEK_END);
            input_len = ftell(infile);
            fseek(infile, 0, SEEK_SET);
            enc = malloc(input_len);
            if (enc) {
                fread(enc, 1, input_len, infile);
            }
            fclose(infile);
        }

        if (!enc) {
            fprintf(stderr, "some err\n");
            return 4;
        }

        char *result = (char *) malloc(sizeof(char) * strlen(enc));

        sanitize(enc);


        freq_analysis(enc, result);


        fputs(result, outfile);

        free(result);
        fclose(outfile);
    }
    else {

        if (!a_specified || !b_specified) {
            fprintf(stderr, "Options -a and -b must be specified!\n");
            return 3;
        }

        char *input = argv[optind];
        size_t input_len = strlen(input);

        sanitize(input);

        char *result = (char *) malloc(sizeof(char) * input_len);


        if (eflag) {
            encrypt(input, result, a, b);
        }
        if (dflag) {
            decrypt(input, result, a, b);
        }

        printf("%s\n", result);
        free(result);
    }

    return 0;
}
