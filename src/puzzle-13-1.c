#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_GRID 2300
#define MAX_FOLDS 100

void fold_x(unsigned int x, uint8_t grid[MAX_GRID][MAX_GRID],
	    unsigned int *max_x, unsigned int max_y) {

	for (int y = 0; y <= max_y; y++) {
		for (int i = x + 1; i <= *max_x; i++) {
			int dist = i - x;
			grid[x-dist][y] |= grid[i][y];
		}
	}

	*max_x = x - 1;
}

void fold_y(unsigned int y, uint8_t grid[MAX_GRID][MAX_GRID],
	    unsigned int max_x, unsigned int *max_y) {

	for (int i = y + 1; i <= *max_y; i++) {
		for (int x = 0; x <= max_x; x++) {
			int dist = i - y;
			grid[x][y-dist] |= grid[x][i];
		}
	}

	*max_y = y - 1;
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

	uint8_t grid[MAX_GRID][MAX_GRID] = {0};
	unsigned int max_x = 0, max_y = 0;
	unsigned int folds[MAX_FOLDS][2];
	unsigned int max_folds = 0;

	char *line = NULL;
	size_t n;
	char delim[] = ",";
	char *token;

	int i = 0;
	bool parsing_coordinates = true;
	while(getline(&line, &n, file) != -1) {
		if (line[0] == '\n') parsing_coordinates = false;
		if (parsing_coordinates) {
			int x = 0, y = 0;
			token = strtok(line, delim);

			int j = 0;
			while (token != NULL) {
				if (j == 1) token[strlen(token)-1] = '\0';
				/* printf("%d - %s\n", j, token); */
				if (j == 0) x = atoi(token);
				if (j == 1) y = atoi(token);
				j++;
				token = strtok(NULL, delim);
			}

			if (x > max_x) max_x = x;
			if (y > max_y) max_y = y;
			grid[x][y] = 1;
		} else {
			if (strlen(line) < 2) continue;
			char direction;
			int coordinate;
			int r = sscanf(line, "fold along %c=%d\n",
			       &direction, &coordinate);
			if (r != 2) continue;
			folds[max_folds][0] = direction == 'x' ? 1 : 0;
			folds[max_folds][1] = coordinate;
			max_folds++;
		}

		i++;
	}
	free(line);
	fclose(file);

	for (int i = 0; i < max_folds; i++) {
		if (folds[i][0] == 1) {
			fold_x(folds[i][1], grid, &max_x, max_y);
		} else {
			fold_y(folds[i][1], grid, max_x, &max_y);
		}
		break;
	}

	int ret = 0;
	for (int y =0; y <= max_y; y++) {
		for (int x = 0; x <= max_x; x++) {
			ret += grid[x][y];;
		}
	}

	printf("%d\n", ret);
	return 0;
}
