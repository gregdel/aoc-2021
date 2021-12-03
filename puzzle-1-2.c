#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

int main() {
	FILE *file;
	char *line = NULL;
	size_t len = 0;
	size_t read;

	file = fopen("inputs/input-1", "r");
	if (file == NULL) {
		printf("Failed to open the input file");
		return 1;
	}

	// Previous and previous previous
	int previous_sum = 0, current_sum = 0;
	int n = 0, n_1 = 0, n_2 = 0;
	int current;
	int counter = 0;
	while ((read = getline(&line, &len, file)) != -1) {
		current = atoi(line);
		if (current == 0) {
			break;
		}

		if (n_2 == 0) {
			n_2 = current;
			continue;
		}

		if (n_1 == 0) {
			n_1 = current;
			continue;
		}

		n = current;
		current_sum = n + n_1 + n_2;

		if (previous_sum == 0) {
			previous_sum = current_sum;
			n_2 = n_1;
			n_1 = n;
			continue;
		}

		if (current_sum > previous_sum)
			counter++;

		previous_sum = current_sum;
		n_2 = n_1;
		n_1 = n;
	}

	printf("The number of increasing values is %d\n", counter);

	free(line);
	fclose(file);
	return 0;
}
