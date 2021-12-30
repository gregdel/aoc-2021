#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

struct area {
	int x_min, x_max;
	int y_min, y_max;
};

bool in_area(struct area *a, int x, int y) {
	if (x < a->x_min || x > a->x_max) return false;
	if (y < a->y_min || y > a->y_max) return false;
	return true;
}

bool after_area(struct area *a, int x, int y) {
	if (x > a->x_max) return true;
	if (y < a->y_min) return true;
	return false;
}

struct probe {
	int x, y;
	int vx, vy;
	int steps;
};

void next_step(struct probe *p) {
	p->x += p->vx;
	p->y += p->vy;
	p->vx += (p->vx > 0 ? -1 : p->vx < 0 ? 1 : 0);
	p->vy--;
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

	struct area target_area;

	if (fscanf(file, "target area: x=%d..%d, y=%d..%d",
		       &target_area.x_min, &target_area.x_max,
		       &target_area.y_min, &target_area.y_max) <= 0)
		return 1;
	fclose(file);

	int max_y = 0;
	for (int vx = 0; vx <= target_area.x_max; vx++) {
		for (int vy = target_area.y_min; vy < target_area.y_min * - 1; vy++) {
			struct probe p;
			p.x = 0; p.y = 0; p.vx = vx; p.vy = vy;
			int max_y_p = 0;
			bool working = false;

			int i = 0;
			for (;;) {
				next_step(&p);
				if (p.y > max_y_p) max_y_p = p.y;

				if (in_area(&target_area, p.x, p.y)) {
					working = true;
					break;
				}

				if (after_area(&target_area, p.x, p.y)) break;
				i++;
			}

			if (!working && i == 0) break;
			if (working && max_y_p > max_y) max_y = max_y_p;
		}
	}

	printf("%d\n", max_y);
	return 0;
}
