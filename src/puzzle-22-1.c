#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define ABS(x)   ((x)<0?-(x):(x))

#define MIN_RANGE -50
#define MAX_RANGE 50
#define MAX_BOXES 5000

struct box {
	bool active;
	bool state;
	int x0, x1;
	int y0, y1;
	int z0, z1;
};

uint64_t size(struct box *b) {
	return ((uint64_t)(b->x1 - b->x0) + 1)
		* ((uint64_t)(b->y1 - b->y0) + 1)
		* ((uint64_t)(b->z1 - b->z0) + 1);
}

void print_box(struct box *b) {
	uint64_t sz = size(b);
	printf("[%s][%ld] x=%d..%d y=%d..%d z=%d..%d\n",
	       b->state ? "on " : "off",
	       sz,
	       b->x0, b->x1,
	       b->y0, b->y1,
	       b->z0, b->z1);
}

// Does out contain in
bool contains(struct box *out, struct box *in) {
	if (
	    ((in->x0 >= out->x0) && (in->x1 <= out->x1)) &&
	    ((in->y0 >= out->y0) && (in->y1 <= out->y1)) &&
	    ((in->z0 >= out->z0) && (in->z1 <= out->z1))
	   ) return true;

	return false;
}

bool overlap(struct box *b1, struct box *b2) {
	struct box b;
	b.x0 = MAX(b1->x0, b2->x0);
	b.x1 = MIN(b1->x1, b2->x1);
	b.y0 = MAX(b1->y0, b2->y0);
	b.y1 = MIN(b1->y1, b2->y1);
	b.z0 = MAX(b1->z0, b2->z0);
	b.z1 = MIN(b1->z1, b2->z1);
	return !((b.x0 > b.x1) || (b.y0 > b.y1) || (b.z0 > b.z1));
}

int merge(struct box *b1, struct box *b2, struct box boxes[7]) {
	int count = 0;
	struct box b;
	b.active = true;

	// outer cuboid on x max
	b.x0 = MIN(b1->x1, b2->x1) + 1;
	b.x1 = b.x0 + (ABS(b1->x1 - b2->x1) - 1);
	if (b.x1 - b.x0 >= 0) {
		b.y0 = (b1->x1 > b2->x1) ? b1->y0 :b2->y0;
		b.y1 = (b1->x1 > b2->x1) ? b1->y1 :b2->y1;
		b.z0 = (b1->x1 > b2->x1) ? b1->z0 :b2->z0;
		b.z1 = (b1->x1 > b2->x1) ? b1->z1 :b2->z1;
		b.state = (b1->x1 > b2->x1) ? b1->state : b2->state;
		memcpy(&boxes[count++], &b, sizeof(struct box));
	}

	// outer cuboid on x min
	b.x0 = MIN(b1->x0, b2->x0);
	b.x1 = b.x0 + (ABS(b1->x0 - b2->x0) - 1);
	if (b.x1 - b.x0 >= 0) {
		b.y0 = (b1->x0 < b2->x0) ? b1->y0 :b2->y0;
		b.y1 = (b1->x0 < b2->x0) ? b1->y1 :b2->y1;
		b.z0 = (b1->x0 < b2->x0) ? b1->z0 :b2->z0;
		b.z1 = (b1->x0 < b2->x0) ? b1->z1 :b2->z1;
		b.state = (b1->x0 < b2->x0) ? b1->state : b2->state;
		memcpy(&boxes[count++], &b, sizeof(struct box));
	}

	// outer cuboid on y max
	b.y1 = MAX(b1->y1, b2->y1);
	b.y0 = b.y1 - (ABS(b1->y1 - b2->y1) - 1);
	if (b.y1 - b.y0 >= 0) {
		b.x0 = MAX(b1->x0, b2->x0);
		b.x1 = MIN(b1->x1, b2->x1);
		b.z0 = (b1->y1 > b2->y1) ? b1->z0 :b2->z0;
		b.z1 = (b1->y1 > b2->y1) ? b1->z1 :b2->z1;
		b.state = (b1->y1 > b2->y1) ? b1->state :b2->state;
		memcpy(&boxes[count++], &b, sizeof(struct box));
	}

	// outer cuboid on y min
	b.y0 = MIN(b1->y0, b2->y0);
	b.y1 = b.y0 + (ABS(b1->y0 - b2->y0) - 1);
	if (b.y1 - b.y0 >= 0) {
		b.x0 = MAX(b1->x0, b2->x0);
		b.x1 = MIN(b1->x1, b2->x1);
		b.z0 = (b1->y0 < b2->y0) ? b1->z0 :b2->z0;
		b.z1 = (b1->y0 < b2->y0) ? b1->z1 :b2->z1;
		b.state = (b1->y0 < b2->y0) ? b1->state :b2->state;
		memcpy(&boxes[count++], &b, sizeof(struct box));
	}

	// outer cuboid on z max
	b.z1 = MAX(b1->z1, b2->z1);
	b.z0 = b.z1 - (ABS(b1->z1 - b2->z1) - 1);
	if (b.z1 - b.z0 >= 0) {
		b.x0 = MAX(b1->x0, b2->x0);
		b.x1 = MIN(b1->x1, b2->x1);
		b.y0 = MAX(b1->y0, b2->y0);
		b.y1 = MIN(b1->y1, b2->y1);
		b.state = (b1->z1 > b2->z1) ? b1->state :b2->state;
		memcpy(&boxes[count++], &b, sizeof(struct box));
	}

	// outer cuboid on z min
	b.z0 = MIN(b1->z0, b2->z0);
	b.z1 = b.z0 + (ABS(b1->z0 - b2->z0) - 1);
	if (b.z1 - b.z0 >= 0) {
		b.x0 = MAX(b1->x0, b2->x0);
		b.x1 = MIN(b1->x1, b2->x1);
		b.y0 = MAX(b1->y0, b2->y0);
		b.y1 = MIN(b1->y1, b2->y1);
		b.state = (b1->z0 < b2->z0) ? b1->state :b2->state;
		memcpy(&boxes[count++], &b, sizeof(struct box));
	}

	// inner cuboid
	b.x0 = MAX(b1->x0, b2->x0);
	b.x1 = MIN(b1->x1, b2->x1);
	b.y0 = MAX(b1->y0, b2->y0);
	b.y1 = MIN(b1->y1, b2->y1);
	b.z0 = MAX(b1->z0, b2->z0);
	b.z1 = MIN(b1->z1, b2->z1);
	b.state = b1->state && b2->state;
	memcpy(&boxes[count++], &b, sizeof(struct box));

	return count;
}

void reduce(struct box boxes[MAX_BOXES], int *count) {
	if (*count == 1) return;
	int new_boxes_added = 1;
	while (new_boxes_added > 0) {
		new_boxes_added = 0;
		for (int i = 0; i < *count; i++) {
			for (int j = 0; j < *count; j++) {
				if (i == j) continue;
				if (!boxes[i].active) continue;
				if (!boxes[j].active) continue;

				// Boxes do not overlap
				if (!overlap(&boxes[i], &boxes[j])) continue;

				// Box j is contained in i
				if ((boxes[i].state == boxes[j].state) &&
				    contains(&boxes[i], &boxes[j])) {
					boxes[j].active = false;
					continue;
				}

				if ((boxes[i].state == boxes[j].state) &&
				    contains(&boxes[j], &boxes[i])) {
					boxes[i].active = false;
					continue;
				}

				// Get the new boxes
				struct box new_boxes[7];
				int new = merge(&boxes[i], &boxes[j], new_boxes);
				if (new == 0) continue;

				memcpy(&boxes[*count], new_boxes,
				       new * sizeof(struct box));
				*count = *count + new;
				new_boxes_added += new;

				boxes[i].active = false;
				boxes[j].active = false;
				continue;
			}
		}
	}

	struct box tmp[MAX_BOXES];
	int new_count = 0;
	for (int i = 0; i < *count; i++) {
		if (!boxes[i].active) continue;
		if (!boxes[i].state) continue;
		// print_box(&boxes[i]);
		memcpy(&tmp[new_count++], &boxes[i],
		       sizeof(struct box));
	}
	memcpy(boxes, tmp, new_count * sizeof(struct box));
	*count = new_count;

	return;
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

	struct box boxes[MAX_BOXES];
	int box_count = 0;

	char *line = NULL;
	size_t n;
	while(getline(&line, &n, file) != -1) {
		struct box b;
		b.active = true;
		char state[5];
		sscanf(line, "%s x=%d..%d,y=%d..%d,z=%d..%d\n",
		       state,
		       &b.x0, &b.x1,
		       &b.y0, &b.y1,
		       &b.z0, &b.z1);
		b.state = strcmp("on", state) == 0 ? true : false;

		if ((b.x0 < MIN_RANGE) || (b.x0 > MAX_RANGE)) continue;
		if ((b.y0 < MIN_RANGE) || (b.y0 > MAX_RANGE)) continue;
		if ((b.z0 < MIN_RANGE) || (b.z0 > MAX_RANGE)) continue;

		memcpy(&boxes[box_count++], &b, sizeof(struct box));
		reduce(boxes, &box_count);
	}
	free(line);
	fclose(file);

	uint64_t boxes_on = 0;
	for (int i = 0; i < box_count; i++) {
		boxes_on = boxes_on + size(&boxes[i]);
	}
	printf("%ld\n", boxes_on);
	return 0;
}
