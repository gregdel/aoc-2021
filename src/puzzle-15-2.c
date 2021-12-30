#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_GRID 500

struct node {
	int x;
	int y;
	int g;
	int h;
};

struct explore_queue {
	struct node *nodes[MAX_GRID * MAX_GRID];
	int count;
};

void add_to_queue(struct explore_queue *q, struct node *n) {
	int i, f;
	for (i = 0; i < q->count; i++) {
		f = q->nodes[i]->g + q->nodes[i]->h;
		if (n->g+n->h > f) break;
		if (n->g+n->h == f && n->h > q->nodes[i]->h) break;
	}

	if (i != q->count) {
		memmove(&q->nodes[i+1], &q->nodes[i],
			(q->count - i) * sizeof(struct node *));
	}

	q->nodes[i] = n;
	q->count++;
}

struct node* pop_from_queue(struct explore_queue *q) {
	if (q->count == 0) return NULL;
	return q->nodes[--q->count];
}

int manhattan_distance(int start_x, int start_y,
			      int end_x, int end_y) {
	return (end_x - start_x) + (end_y - start_y);
}

int explore(int x, int y, int max,
	     int grid[MAX_GRID][MAX_GRID],
	     bool explored[MAX_GRID][MAX_GRID],
	     struct node nodes[MAX_GRID][MAX_GRID],
	     struct explore_queue *q) {


	explored[x][y] = true;
	struct node *n = &nodes[x][y];
	if (x == max-1 && y == max-1) return n->g;

	int nx, ny;
	int g;
	struct node *nn;
	for (int i = 0; i < 4; i++) {
		nx = (i == 0 ? x+1 : (i == 1) ? x-1 : x);
		ny = (i == 2 ? y+1 : (i == 3) ? y-1 : y);
		if (nx < 0 || nx > max-1) continue;
		if (ny < 0 || ny > max-1) continue;

		nn = &nodes[nx][ny];

		g = n->g + grid[nx][ny];
		if (g >= nn->g) continue;
		nn->g = g;
		if (!explored[nx][ny]) add_to_queue(q, nn);
	}

	return 0;
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

	int grid[MAX_GRID][MAX_GRID] = {0};

	char *line = NULL;
	size_t sn;
	int max = 0, j = 0;
	while(getline(&line, &sn, file) != -1) {
		if (max == 0) max = strlen(line) - 1;
		for (int i = 0; i < max; i++) {
			grid[j][i] = line[i] - '0';
		}
		j++;
	}
	free(line);
	fclose(file);

	max = max*5;

	bool explored[MAX_GRID][MAX_GRID];
	struct node nodes[MAX_GRID][MAX_GRID];
	int h = max/5;
	for (int x = 0; x < max; x++) {
		for (int y = 0; y < max; y++) {
			explored[x][y] = false;
			nodes[x][y].x = x;
			nodes[x][y].y = y;
			nodes[x][y].g = INT_MAX;
			nodes[x][y].h = manhattan_distance(x, y, max-1, max-1);
			grid[x][y] = (grid[x%h][y%h] + x/h + y/h - 1) % 9 + 1;
		}
	}

	struct explore_queue queue;
	queue.count = 0;
	nodes[0][0].g = 0;
	add_to_queue(&queue, &nodes[0][0]);

	struct node *n;
	int g = 0;
	while (g == 0) {
		n = pop_from_queue(&queue);
		if (n == NULL) break;
		g = explore(n->x, n->y, max, grid,
			    explored, nodes, &queue);
	}

	printf("%d\n", g);
	return 0;
}
