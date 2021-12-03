#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	FILE *file = fopen("inputs/input-2", "r");
	if (file == NULL) {
		printf("Failed to open the input file");
		return 1;
	}

	char direction[8];
	int value;
	int x = 0, y = 0;
	while(fscanf(file, "%s %d\n", direction, &value) != EOF) {
		if (strcmp(direction, "up") == 0) {
			y -= value;
		} else if (strcmp(direction, "down") == 0) {
			y += value;
		} else if (strcmp(direction, "forward") == 0) {
			x += value;
		}
	}

	printf("position (%d,%d)\n", x, y);
	printf("result: %d\n", x * y);

	fclose(file);
	return 0;
}
