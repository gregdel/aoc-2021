#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_LIFE_SPAN 9

int parse_input(uint64_t ages[MAX_LIFE_SPAN]) {
	FILE *file = fopen("inputs/input-6", "r");
	if (file == NULL) {
		printf("Failed to open the input file");
		return 0;
	}

	int input_size = 0;
	int c;
	while ((c = fgetc(file)) != EOF) {
		if (c == '\0')
			break;

		if (c == ',')
			continue;

		int age = c - '0';
		if ((age < 0) || (age > 10)) {
			break;
		}

		ages[age]++;
		input_size++;
	}

	return input_size;
}

void print_input(int ages[MAX_LIFE_SPAN], int day) {
	printf("Day %d:\t", day);
	for (int i = 0; i < MAX_LIFE_SPAN; i++) {
		printf("%d:%d ", i, ages[i]);
	}
	printf("\n");
}

int main() {
	// Group fishes by age
	uint64_t ages[MAX_LIFE_SPAN] = {0};
	int input_size = parse_input(ages);

	printf("Number of fishes: %d\n", input_size);
	/* print_input(ages, 0); */

	int days = 256;
	for (int day = 0; day < days; day++) {
		uint64_t prev_age_count = 0;
		for (int age = MAX_LIFE_SPAN-1; age >= 0; age--) {
			// Get the number of fishes in the current age
			uint64_t current = ages[age];
			ages[age] = prev_age_count;
			prev_age_count = current;

			/* printf("Age: %d / Prev age count: %d\n", age, prev_age_count); */

			if (age == 0) {
				// Reset all the fish to 6
				ages[6] += current;
				// Add new fishes in 8
				ages[8] += current;
			}
		}

		/* print_input(ages, day+1); */
	}

	uint64_t sum = 0;
	for (int i = 0; i < MAX_LIFE_SPAN; i++)
		sum += ages[i];

	printf("Number of fishes: %ld\n", sum);

	return 0;
}
