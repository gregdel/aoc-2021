#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

enum TimidType {
	Amber  = 1,
	Bronze = 10,
	Copper = 100,
	Desert = 1000,
};

// Here's the map
//    0           12
// 0: #############
// 1: #...........#
// 2: ###B#C#B#D###
// 3:   #A#D#C#A#
// 4:   #########
#define MAP_LINES 5
#define MAP_COLUMNS 13

void print_map(char map[MAP_LINES][MAP_COLUMNS]) {
	for (int i = 0; i < MAP_LINES; i++) {
		for (int j = 0; j < MAP_COLUMNS; j++) {
			printf("%c", map[i][j]);
		}
		printf("\n");
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

	char map[MAP_LINES][MAP_COLUMNS];

	char *line = NULL;
	size_t n;

	int count = 0;
	while(getline(&line, &n, file) != -1) {
		memcpy(map[count++], line, MAP_COLUMNS*sizeof(char));
		if (count > 3) {
			map[count-1][MAP_COLUMNS-2] = ' ';
			map[count-1][MAP_COLUMNS-1] = ' ';
		}
	}
	free(line);
	fclose(file);

	printf("\n");
	print_map(map);

	printf("%d\n", 0);
	return 0;
}
