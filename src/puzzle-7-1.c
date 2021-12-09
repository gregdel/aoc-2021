#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_INPUT 1024

inline int abs(int a) {
	return (a < 0) ? -1 * a : a;
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

	int numbers[MAX_INPUT] = {0};
	int numbers_count = 0;
	char *str_num = NULL;
	size_t n;
	while(getdelim(&str_num, &n, ',', file) > 0)
		numbers[numbers_count++] = atoi(str_num);
	free(str_num);
	fclose(file);

	int min = (1<<16), max = 0;
	for (int i = 0; i < numbers_count; i++) {
		int number = numbers[i];
		if (number < min)
			min = number;

		if (number > max)
			max = number;
	}

	int min_cost = (max * numbers_count);
	for (int position = min; position < max; position++) {
		int cost = 0;
		for (int i = 0; i < numbers_count; i++)
			cost += abs(numbers[i] - position);

		if (cost < min_cost)
			min_cost = cost;
	}

	printf("%d\n", min_cost);
	return 0;
}
