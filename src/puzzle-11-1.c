#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INPUT 100

int handle(uint8_t grid[MAX_INPUT][MAX_INPUT],
	   bool exploded[MAX_INPUT][MAX_INPUT],
	   int i, int j, int max_i, int max_j) {
	if (i < 0) return 0;
	if (j < 0) return 0;
	if (i > max_i-1) return 0;
	if (j > max_j-1) return 0;

	if ((grid[i][j] == 0) && (exploded[i][j])) return 0;
	grid[i][j]++;
	if (grid[i][j] <= 9) return 0;

	int ret = 1;
	grid[i][j] = 0;
	exploded[i][j] = true;

	ret += handle(grid, exploded, i-1, j-1, max_i, max_j);
	ret += handle(grid, exploded, i,   j-1, max_i, max_j);
	ret += handle(grid, exploded, i+1, j-1, max_i, max_j);
	ret += handle(grid, exploded, i-1, j  , max_i, max_j);
	ret += handle(grid, exploded, i+1, j  , max_i, max_j);
	ret += handle(grid, exploded, i-1, j+1, max_i, max_j);
	ret += handle(grid, exploded, i,   j+1, max_i, max_j);
	ret += handle(grid, exploded, i+1, j+1, max_i, max_j);

	return ret;
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

	char *line = NULL;
	size_t n;

	uint8_t grid[MAX_INPUT][MAX_INPUT] = {0};
	int grid_length = 0, grid_height = 0;
	while(getline(&line, &n, file) != -1) {
		for (int i = 0; i < MAX_INPUT; i++) {
			char c = line[i];
			if (c == '\n') {
				grid_length = i;
				break;
			}

			grid[grid_height][i] = c - '0';
		}
		grid_height++;
	}
	free(line);
	fclose(file);

	int ret = 0;
	for (int step = 0; step < 100; step++) {
		bool exploded[MAX_INPUT][MAX_INPUT] = {0};
		for (int i = 0; i < grid_height; i++) {
			for (int j = 0; j < grid_length; j++) {
				ret += handle(grid, exploded,
				      i, j, grid_height, grid_length);
			}

		}
	}

	printf("%d\n", ret);
	return 0;
}
