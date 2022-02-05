#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_INPUT 5
#define INSTRUCTION_SET_MAX 14
#define VAR_Z 0
#define VAR_A 1
#define VAR_B 2
#define VAR_C 3
#define VAR_W 4

struct prog {
    int a[INSTRUCTION_SET_MAX];
    int b[INSTRUCTION_SET_MAX];
    int c[INSTRUCTION_SET_MAX];
};

long int get_z(struct prog p, int step, int w, long int z) {
    int x = 0;
    long int y = 0;
    if ((z % 26 + p.b[step]) == w) {
        x = 0;
    }
    else {
        x = 1;
    }

    int tmp_z = z / p.a[step];

    y = tmp_z * ( 25*x + 1 );
    z = y + (w + p.c[step]) * x;
    return z;
}

void guess_w(struct prog p, int guesses[9], int z, int step) {
    int w;
    w = ( z % 26 ) + p.b[step];
    if (w > 0 && w < 10) {
        guesses[0] = w;
    } else {
        int idx = 0;
        // If we can forsee the future
        if (step < INSTRUCTION_SET_MAX-1) {
            for (int i = 0; i < 9; i++) {
                int new_w = 9-i;
                int new_z = get_z(p, step, new_w, z);
                int new_half_x = ((new_z%26)+p.b[step+1]);
                if ( new_half_x >= 1 && new_half_x <= 9) {
                    guesses[idx++] = new_w;
                }
            }
        }

        if (idx == 0) {
            for (int i = 0; i < 9; i++) {
                guesses[i] = 9-i;
            }
        }
    }

    return;
}

void print_input(int input[14]) {
    for (int i = 0; i < 14; i++) {
        printf("%d", input[i]);
    }
    printf("\n");
}

bool solve_step(struct prog p, int step, long int z, int input[14]) {
    if (step >= INSTRUCTION_SET_MAX) {
        if (z == 0) {
            print_input(input);
            return true;
        }
        return false;
    }
    int guesses[9] = {0};
    int w;

    guess_w(p, guesses, z, step);

    int starting_z = z;

    for (int i = 0; i < 9; i++) {
        z = starting_z;
        w = guesses[i];
        if (w == 0) break;

        int new_input[14];
        memcpy(new_input, input, sizeof(int[14]));
        new_input[step] = w;

        z = get_z(p, step, w, z);

        if (solve_step(p, step+1, z, new_input)) {
            return true;
        }
    }
    return false;
}

void run(struct prog p) {
    // We try to make x == 0
    // So in fact we try to make (z%26 + B) == w
    int input[14] = { 0 };
    solve_step(p, 0, 0, input);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Missing input file");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Failed to open the input file");
        return 1;
    }

    struct prog p;

    char *line = NULL;
    size_t n;
    int line_nb = 0;
    int block = -1;
    while(getline(&line, &n, file) != -1) {
        if (line[0] == 'i') {
            block++;
            line_nb = 0;
        } else {
            char op[4];
            char c;
            char value[6];
            sscanf(line, "%s %c %s\n", op, &c, value);

            if (line_nb == 4) {
                p.a[block] = atoi(value);
            }
            else if (line_nb == 5) {
                p.b[block] = atoi(value);
            }
            else if (line_nb == 15) {
                p.c[block] = atoi(value);
            }
        }
        line_nb++;
    }
    free(line);
    fclose(file);

    run(p);
    return 0;
}
