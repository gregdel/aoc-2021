#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_CAVES 16
#define MAX_PATH 100000

struct cave {
	uint8_t id;
	char name[6];
	bool big;
	uint8_t visited;
	struct cave *links[MAX_CAVES];
	uint8_t link_count;
};

struct cave *find_cave(struct cave caves[MAX_CAVES],
		       uint8_t *cave_count, char *name) {

	struct cave *c = NULL;
	for (int i = 0; i < *cave_count; i++) {
		if (strcmp(caves[i].name, name) == 0) {
			c = &caves[i];
			break;
		}
	}

	if (c != NULL)
		return c;

	c = &caves[*cave_count];
	c->id = *cave_count;
	*cave_count = *cave_count+1;
	c->visited = 0;
	c->link_count = 0;
	c->big = false;
	c->visited = 0;
	for (int i = 0; i < MAX_CAVES; i++) c->links[i] = NULL;
	strncpy(c->name, name, MAX_CAVES);
	if (c->name[0] >= 'A' && c->name[0] < 'Z')  c->big   = true;

	return c;
}

int explore_cave(struct cave *from, struct cave *c, struct cave *lsc) {
	bool start = strcmp(c->name, "start") == 0 ? true : false;
	if (from != NULL && start) return 0;
	if (strcmp(c->name, "end") == 0) return 1;

	int ret = 0;
	if (!c->big) {
		if (lsc == NULL && c->visited == 1) {
			if (!start) lsc = c;
		} else {
			if (lsc == c) {
				if (c->visited == 2) return 0;
			} else {
				if (c->visited == 1) return 0;
			}
		}
	}

	c->visited++;
	for (int i = 0; i < c->link_count; i++) {
		ret += explore_cave(c, c->links[i], lsc);
	}
	c->visited--;

	if (lsc == c && c->visited < 2) lsc = NULL;
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
	char delim[] = "-";
	char *token;

	struct cave *c, *c1 ;
	struct cave caves[MAX_CAVES];
	uint8_t cave_count = 0;

	while(getline(&line, &n, file) != -1) {
		token = strtok(line, delim);
		int j = 0;
		while (token != NULL) {
			if (j == 1) token[strlen(token)-1] = '\0';
			c = find_cave(caves, &cave_count, token);
			if (c == NULL) break;
			if (j == 0) c1 = c;
			token = strtok(NULL, delim);
			j++;
		}

		c1->links[c1->link_count++] = c;
		c->links[c->link_count++] = c1;
	}
	free(line);
	fclose(file);

	struct cave *start = find_cave(caves, &cave_count, "start");

	int ret = explore_cave(NULL, start, NULL);
	printf("%d\n", ret);
	return 0;
}
