#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_GRID 100

struct explore_queue {
	unsigned int list[MAX_GRID * MAX_GRID][3];
	unsigned int count;
};

void add_to_queue(struct explore_queue *q, int x, int y, int distance) {
	int i;
	for (i = 0; i < q->count; i++) {
		if (distance > q->list[i][2]) break;
	}

	if (i != q->count) {
		memmove(q->list[i+1], q->list[i],
			(q->count - i) * sizeof(q->list[i]));
	}

	q->list[i][0] = x;
	q->list[i][1] = y;
	q->list[i][2] = distance;
	q->count++;
}

void pop_from_queue(struct explore_queue *q, int coord[2]) {
	if (q->count == 0) return;
	coord[0] = q->list[q->count-1][0];
	coord[1] = q->list[q->count-1][1];
	q->count--;
}

void explore(uint8_t x, uint8_t y, uint8_t max,
	     uint8_t grid[MAX_GRID][MAX_GRID],
	     bool explored[MAX_GRID][MAX_GRID],
	     unsigned int distance[MAX_GRID][MAX_GRID],
	     struct explore_queue *q) {

	unsigned int d;

	explored[x][y] = true;

	// UP
	if (y - 1 > 0 && !explored[x][y-1]) {
		d = distance[x][y] + grid[x][y-1];
		if (d < distance[x][y-1]) {
			distance[x][y-1] = d;
			add_to_queue(q, x, y-1, d);
		}
	}
	// DOWN
	if (y + 1 <= max && !explored[x][y+1]) {
		d = distance[x][y] + grid[x][y+1];
		if (d < distance[x][y+1]) {
			distance[x][y+1] = d;
			add_to_queue(q, x, y+1, d);
		}
	}
	// LEFT
	if (x - 1 > 0 && !explored[x-1][y]) {
		d = distance[x][y] + grid[x-1][y];
		if (d < distance[x-1][y]) {
			distance[x-1][y] = d;
			add_to_queue(q, x-1, y, d);
		}
	}
	// RIGHT
	if (x + 1 <= max && !explored[x+1][y]) {
		d = distance[x][y] + grid[x+1][y];
		if (d < distance[x+1][y]) {
			distance[x+1][y] = d;
			add_to_queue(q, x+1, y, d);
		}
	}
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

	char *line = NULL;
	size_t n;
	uint8_t max = 0, j = 0;
	while(getline(&line, &n, file) != -1) {
		if (max == 0) max = strlen(line) - 1;
		for (int i = 0; i < max; i++) {
			grid[j][i] = line[i] - '0';
		}
		j++;
	}
	free(line);
	fclose(file);

	bool explored[MAX_GRID][MAX_GRID];
	unsigned int distance[MAX_GRID][MAX_GRID];
	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++) {
			explored[i][j] = false;
			distance[i][j] = UINT_MAX;
		}
	}

	struct explore_queue queue;
	queue.count = 0;
	add_to_queue(&queue, 0, 0, 0);
	distance[0][0] = 0;

	for (;;) {
		if (queue.count == 0) break;
		int coords[2] = {0};
		pop_from_queue(&queue, coords);
		explore(coords[0], coords[1], max, grid,
			explored, distance, &queue);
	}

	printf("%d\n", distance[max-1][max-1]);
	return 0;
}
