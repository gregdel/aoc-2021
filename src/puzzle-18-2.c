#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_LENGTH 1024
#define MAX_NUMBERS 100

struct number {
	int value;
	int depth;
};

struct expression {
	struct number numbers[MAX_LENGTH];
	int count;
};

void print_expression(struct expression *e) {
	for (int i = 0; i < e->count; i++) {
		/* printf("%02d(%d) ", */
		/*        e->numbers[i].value, */
		/*        e->numbers[i].depth); */
		printf("%d ", e->numbers[i].value);
	}
	printf("\n");
}

void add(struct expression *a, struct expression *b) {
	memcpy(
		&a->numbers[a->count],
		&b->numbers[0],
		b->count * sizeof(struct number));

	if (a->count > 0) {
		for (int i = 0; i < a->count + b->count; i++) {
			a->numbers[i].depth++;
		}
	}

	a->count += b->count;
}

bool explode(struct expression *e) {
	bool exploded = false;
	for (int i = 0; i < e->count; i++) {
		if (e->numbers[i].depth <= 4) continue;
		if (i + 1 >= e->count) continue;

		/* printf("explode (%d,%d)[%d] in %d\n", */
		/*        e->numbers[i].value, */
		/*        e->numbers[i+1].value, */
		/*        e->numbers[i].depth, i); */
		/* print_expression(e); */

		if (i > 0)
			e->numbers[i-1].value += e->numbers[i].value;
		if (i + 2 < e->count)
			e->numbers[i+2].value += e->numbers[i+1].value;

		e->numbers[i].value = 0;
		e->numbers[i].depth--;

		if (i + 2 < e->count) {
			memmove(&e->numbers[i+1], &e->numbers[i+2],
				(e->count - (i + 2)) * sizeof(struct number));
		}
		e->count--;
		i++;
		exploded = true;
		return true;
	}

	return exploded;
}

bool split(struct expression *e) {
	for (int i = 0; i < e->count; i++) {
		if (e->numbers[i].value < 10) continue;

		int l = e->numbers[i].value / 2;
		int r = (e->numbers[i].value+1)/2;
		int c = e->count - 1 - i;
		if (c > 0) {
			memmove(&e->numbers[i+2], &e->numbers[i+1],
				c * sizeof(struct number));
		}

		e->numbers[i].value = l;
		e->numbers[i].depth++;
		e->numbers[i+1].value = r;
		e->numbers[i+1].depth = e->numbers[i].depth;

		e->count++;
		return true;
	}

	return false;
}

int magnitude(struct expression *e) {
	struct expression ne;
	ne.count = e->count;
	memcpy(ne.numbers,
	       &e->numbers,
	       e->count * sizeof(struct number));

	int i = 0;
	for (;;) {
		if (ne.numbers[i].depth != ne.numbers[i+1].depth) {
			i++;
			continue;
		}

		ne.numbers[i].value = 3 * ne.numbers[i].value
				+ 2 * ne.numbers[i+1].value;
		ne.numbers[i].depth--;
		if (ne.numbers[i].depth == 0) {
			return ne.numbers[i].value;
		}

		int c = e->count - 1 - i;
		if (c > 0) {
			memmove(&ne.numbers[i+1],
				&ne.numbers[i+2],
				c * sizeof(struct number));
		}
		ne.count--;
		i = 0;
	}
}

void reduce(struct expression *e) {
	for (;;) {
		if (explode(e)) continue;
		if (split(e))   continue;
		break;
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

	struct expression expressions[MAX_NUMBERS];
	int count = 0;

	char *line = NULL;
	size_t n;
	while(getline(&line, &n, file) != -1) {
		struct expression *ne = &expressions[count++];
		ne->count = 0;
		int depth = 0;
		for (int i = 0; i < strlen(line) - 1; i++) {
			char c = line[i];
			switch (c) {
			case '[':  depth++;  break;
			case ']':  depth--;  break;
			case ',':  continue; break;
			case '\n': continue; break;
			default:
				   ne->numbers[ne->count++] = (struct number){
					   .value = c - '0',
					   .depth = depth,
				   };
			}
		}
	}
	free(line);
	fclose(file);

	int max = 0;
	for (int i = 0; i < count; i++) {
		for (int j = 0; j < count; j++) {
			if (i == j) continue;
			struct expression e;
			memcpy(&e, &expressions[i], sizeof(struct expression));

			add(&e, &expressions[j]);
			reduce(&e);
			int m = magnitude(&e);
			if (m > max) {
				max = m;
			}
		}
	}

	printf("%d\n", max);
	return 0;
}
