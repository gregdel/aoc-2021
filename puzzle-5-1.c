#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define GRID_SIZE 1000

void trace_segment(int grid[GRID_SIZE][GRID_SIZE],
		   int x1, int y1, int x2, int y2) {



	printf("(%d,%d) -> (%d,%d)\n", x1, y1, x2, y2);

	int start, steps;

	if (x1 - x2 == 0) {
		// Vertical line
		steps = (y1 < y2) ? y2 - y1 : y1 - y2;
		start = (y1 < y2) ? y1 : y2;
		for (int i = start; i <= start + steps; i++)
			grid[x1][i]++;

		/* printf("x=%d\n y from %d to %d+%d\n", x1, start, start, steps); */
	} else {
		int a = (y1 - y2) / (x1 - x2);
		int b = y1 - (a * x1);

		steps = (x1 - x2 < 0) ? x2 - x1 : x1 - x2;
		start = (x1 < x2) ? x1 : x2;

		if (y1 != y2)
			return;

		for (int i = start; i <= start + steps; i++) {
			int y = a * i + b;
			grid[i][y]++;
		}

		/* printf("y=%dx + %d\n", a, b); */
	}
}

void print_grid(int grid[GRID_SIZE][GRID_SIZE]) {
	for (int i = 0; i < GRID_SIZE; i++) {
		for (int j = 0; j < GRID_SIZE; j++) {
			printf("%d\t", grid[i][j]);
		}
		printf("\n");
	}
}

int count_intersections(int grid[GRID_SIZE][GRID_SIZE], int number) {
	int count = 0;
	for (int i = 0; i < GRID_SIZE; i++) {
		for (int j = 0; j < GRID_SIZE; j++) {
			if (grid[i][j] >= number)
				count++;
		}
	}
	return count;
}

int main() {
	FILE *file = fopen("inputs/input-5", "r");
	if (file == NULL) {
		printf("Failed to open the input file");
		return 1;
	}

	int grid[GRID_SIZE][GRID_SIZE] = {0};

	int x1, y1, x2, y2;
	while(fscanf(file, "%d,%d -> %d,%d\n", &x1, &y1, &x2, &y2) != EOF) {
		trace_segment(grid, x1, y1, x2, y2);
	}

	int number = 2;
	int count = count_intersections(grid, number);
	printf("Intersections > %d: %d\n", number, count);
	/* print_grid(grid); */

	return 0;
}
