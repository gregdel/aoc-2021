#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INPUT 128

enum direction { UP, DOWN, RIGHT, LEFT };

struct map {
	uint8_t data[MAX_INPUT][MAX_INPUT];
	uint8_t explored[MAX_INPUT][MAX_INPUT];
	uint8_t basin_sizes[MAX_INPUT*2];
	uint8_t max_length;
	uint8_t max_height;
	uint8_t basins;
};

void init_map(struct map *m) {
	memset(m->data, 0, MAX_INPUT*MAX_INPUT*sizeof(uint8_t));
	memset(m->explored, 0, MAX_INPUT*MAX_INPUT*sizeof(uint8_t));
	memset(m->basin_sizes, 0, MAX_INPUT*2*sizeof(uint8_t));
	m->max_length = 0;
	m->max_height = 0;
	m->basins = 0;
}

bool is_part_of_basin(struct map *m, uint8_t value, int i, int j) {
	uint8_t v = m->data[i][j];
	if (v == 9)
		return false;

	if (v < value)
		return false;

	if (m->explored[i][j] == 1)
		return false;

	return true;
}

int explore(struct map *m, int i, int j) {
	int ret = 1;
	m->explored[i][j] = 1;
	uint8_t value = m->data[i][j];

	if ((i > 0) && is_part_of_basin(m, value, i-1, j))
			ret += explore(m, i-1, j);
	if ((j > 0) && is_part_of_basin(m, value, i, j-1))
			ret += explore(m, i, j-1);
	if ((j+1 < m->max_length) && is_part_of_basin(m, value, i, j+1))
			ret += explore(m, i, j+1);
	if ((i+1 < m->max_height)  && is_part_of_basin(m, value, i+1, j))
			ret += explore(m, i+1, j);

	return ret;
}

int basin_size(struct map *m, int i, int j) {
	return explore(m, i, j);
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

	struct map map;
	init_map(&map);

	char *line = NULL;
	size_t n;
	while(getline(&line, &n, file) != -1) {
		for (int i = 0; i < MAX_INPUT; i++) {
			char c = line[i];
			if (c == '\n') {
				map.max_length = i;
				break;
			}

			map.data[map.max_height][i] = c - '0';
		}

		map.max_height++;
	}
	free(line);
	fclose(file);

	for (int i = 0; i < map.max_height; i++) {
		for (int j = 0; j < map.max_length; j++) {
			uint8_t up = 10, left = 10,
				down = 10, right = 10,
				value = map.data[i][j];

			if (i > 0) up=map.data[i-1][j];
			if (j > 0) left=map.data[i][j-1];
			if (j+1 < map.max_length) right=map.data[i][j+1];
			if (i+1 < map.max_height) down=map.data[i+1][j];

			if ((value < up) && (value < down)
			    && (value < right) && (value < left)) {
				map.basin_sizes[map.basins++] = basin_size(&map, i, j);
			}
		}
	}

	int m1 = 0, m2 = 0, m3 = 0, tmp1, tmp2;
	for (int i = 0; i < map.basins; i++) {
		uint8_t value = map.basin_sizes[i];
		if (value > m1) {
			tmp1 = m1; m1 = value;
			tmp2 = m2; m2 = tmp1;
			tmp1 = m3; m3 = tmp2;
			continue;
		}

		if (value > m2) {
			tmp1 = m2; m2 = value; m3 = tmp1;
			continue;
		}

		if (value > m3) m3 = value;
	}

	int ret = m1 * m2 *m3;

	printf("%d\n", ret);
	return 0;
}
