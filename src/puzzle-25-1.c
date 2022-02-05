#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#define GRID_WIDTH 500
#define GRID_HEIGHT 500

#define IS_EMPTY(c) ((c)=='.'?1:0)

typedef struct map {
    char data[GRID_HEIGHT][GRID_WIDTH];
    int height;
    int width;
} map;

void print_map(map *m) {
    for (int row = 0; row < m->height; row++) {
        for (int col = 0; col < m->width; col++) {
            printf("%c", m->data[row][col]);
        }
        printf("\n");
    }
}

bool move_right(map *m) {
    bool moved = false;
    map tmp;
    memcpy(&tmp, m, sizeof(map));
    for (int row = 0; row < m->height; row++) {
        for (int col = 0; col < m->width; col++) {
            char c = m->data[row][col];
            if (c != '>') continue;

            int right_col = (col+1) % m->width;
            if (IS_EMPTY(m->data[row][right_col])) {
                moved = true;
                tmp.data[row][col] = '.';
                tmp.data[row][right_col] = '>';
            }
        }
    }

    memcpy(m, &tmp, sizeof(map));
    return moved;
}

bool move_down(map *m) {
    bool moved = false;
    map tmp;
    memcpy(&tmp, m, sizeof(map));
    for (int row = 0; row < m->height; row++) {
        for (int col = 0; col < m->width; col++) {
            char c = m->data[row][col];
            if (c != 'v') continue;

            int down_row = (row+1) % m->height;
            if (IS_EMPTY(m->data[down_row][col])) {
                moved = true;
                tmp.data[row][col] = '.';
                tmp.data[down_row][col] = 'v';
            }
        }
    }

    memcpy(m, &tmp, sizeof(map));
    return moved;
}

bool move(map *m) {
    bool r = move_right(m);
    bool d = move_down(m);
    return d || r;
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

    map m;

	char *line = NULL;
	size_t n;
	int height = 0;
    int width = 0;
	while(getline(&line, &n, file) != -1) {
        if (width == 0) {
            width = strlen(line) - 1;
            m.width = width;
        }

		memcpy(m.data[height++], line, width*sizeof(char));
	}
    m.height = height;
	free(line);
	fclose(file);

    int i = 1;
    while(move(&m)) { i++; }

    printf("%i\n", i);
    return 0;
}
