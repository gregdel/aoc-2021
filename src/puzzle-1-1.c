#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	FILE *file;
	char *line = NULL;
	size_t len = 0;
	size_t read;

	if (argc != 2) {
		printf("Missing input file");
		return 1;
	}

	file = fopen(argv[1], "r");
	if (file == NULL) {
		printf("Failed to open the input file");
		return 1;
	}

	int current;
	int previous;
	int counter = 0;
	while ((read = getline(&line, &len, file)) != -1) {
		current = atoi(line);
		if (current == 0) {
			printf("Invalid input: %s\n", line);
			break;
		}

		if (previous == 0) {
			previous = current;
			continue;
		}

		if (current > previous) {
			counter++;
		}
		previous = current;
	}

	printf("%d\n", counter);

	free(line);
	fclose(file);
	return 0;
}
