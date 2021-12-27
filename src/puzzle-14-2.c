#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_CHAIN 25
#define MAX_PAIRS 100
#define STEPS 40

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

	char chain[MAX_CHAIN];
	char pairs[26][26] = {0};
	uint64_t count[26][26] = {0};

	char *line = NULL;
	size_t n;
	int i = 0;
	while(getline(&line, &n, file) != -1) {
		if (i == 0) {
			strcpy(chain, line);
			chain[strlen(line)-1] = '\0';
		}

		if (i > 1) {
			char c1, c2, result;
			sscanf(line, "%c%c -> %c\n", &c1, &c2, &result);
			pairs[c1 - 'A'][c2 - 'A'] = result;
		}

		i++;
	}
	free(line);
	fclose(file);

	for (int i = 0; i < strlen(chain) - 1; i++) {
		char c1 = chain[i], c2 = chain[i+1];
		count[c1 - 'A'][c2 - 'A']++;
	}

	for (int i = 0; i < STEPS; i++) {
		uint64_t nc[26][26] = {0};
		for (int i = 0; i < 26; i++) {
			for (int j = 0; j < 26; j++) {
				if (count[i][j] == 0) continue;

				char r = pairs[i][j];
				nc[i][r - 'A'] += count[i][j];
				nc[r - 'A'][j] += count[i][j];
			}
		}
		memcpy(count, nc, sizeof(count));
	}

	uint64_t c_count[26] = {0};
	for (int i = 0; i < 26; i++) {
		for (int j = 0; j < 26; j++) {
			if (count[i][j] == 0) continue;
			c_count[i] += count[i][j];
			c_count[j] += count[i][j];
		}
	}

	uint64_t max = 0, min = ULONG_MAX;
	for (int i = 0; i < 26; i++) {
		if (c_count[i] == 0) continue;
		uint64_t c = (c_count[i]+1) / 2;
		if (c > max) max = c;
		if (c < min) min = c;
	}

	printf("%ld\n", max - min);
	return 0;
}
