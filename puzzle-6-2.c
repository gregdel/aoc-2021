#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_LIFE_SPAN 9

int main() {
	FILE *file = fopen("inputs/input-6", "r");
	if (file == NULL) {
		printf("Failed to open the input file");
		return 1;
	}

	// Group fishes by age
	uint64_t ages[MAX_LIFE_SPAN] = {0};
	int i;
	while (fscanf(file, "%d", &i) > 0) ages[i]++;

	int days = 256;
	for (int day = 0; day < days; day++) {
		uint64_t prev_age_count = 0;
		for (int age = MAX_LIFE_SPAN-1; age >= 0; age--) {
			// Get the number of fishes in the current age
			uint64_t current = ages[age];
			ages[age] = prev_age_count;
			prev_age_count = current;

			if (age == 0) {
				// Reset all the fish to 6
				ages[6] += current;
				// Add new fishes in 8
				ages[8] += current;
			}
		}
	}

	uint64_t sum = 0;
	for (int i = 0; i < MAX_LIFE_SPAN; i++) sum += ages[i];

	printf("Number of fishes: %ld\n", sum);
	return 0;
}
