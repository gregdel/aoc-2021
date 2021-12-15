#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_INPUT 128

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


	uint8_t data[MAX_INPUT][MAX_INPUT] = {0};

	char *line = NULL;
	size_t n;
	uint8_t max_length = 0, max_height = 0;
	while(getline(&line, &n, file) != -1) {
		for (int i = 0; i < MAX_INPUT; i++) {
			char c = line[i];
			if (c == '\n') {
				max_length = i;
				break;
			}

			data[max_height][i] = c - '0';
		}

		max_height++;
	}
	free(line);
	fclose(file);

	int ret = 0;
	for (int i = 0; i < max_height; i++) {
		for (int j = 0; j < max_length; j++) {
			uint8_t up = 10, left = 10,
				down = 10, right = 10,
				value = data[i][j];

			if (i > 0) up=data[i-1][j];
			if (j > 0) left=data[i][j-1];
			if (j+1 < max_length) right=data[i][j+1];
			if (i+1 < max_height) down=data[i+1][j];

			if ((value < up) && (value < down)
			    && (value < right) && (value < left)) {
				ret += 1 + value;
			}
		}
	}

	printf("%d\n", ret);
	return 0;
}
