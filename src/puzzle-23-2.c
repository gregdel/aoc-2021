#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <search.h>

#define MIN(a,b)    ((a)<(b)?(a):(b))
#define ABS(x)      ((x)<0?-(x):(x))
#define IS_EMPTY(c) ((c)=='.'?1:0)

int min_cost = 100000000;

// Here's the map
//    0           12
// 0: #############
// 1: #...........#
// 2: ###B#C#B#D###
// 3    #D#C#B#A#
// 4    #D#B#A#C#
// 5:   #A#D#C#A#
// 6:   #########
#define HASH_SIZE 100000
#define ROOMS_COUNT 4
#define ROOMS_SIZE 4
#define MAP_ROWS 7
#define MAP_COLUMNS 13
#define HALLWAY_ROW 1

#define ROOM_ROW_START 2
#define ROOM_ROW_END 5
#define BOTTOM_LINE 5

#define AVAILABLE_HALLWAY_SPOTS 7
#define MAX_KEY_SIZE AVAILABLE_HALLWAY_SPOTS + ROOMS_COUNT * ROOMS_SIZE
#define MAX_SPOTS AVAILABLE_HALLWAY_SPOTS + ROOMS_SIZE

const uint8_t available_hallway_spots[] = {1, 2, 4, 6, 8, 10, 11};
const int amphipod_target[ROOMS_COUNT]  = {3, 5, 7, 9};
const int amphipod_cost[ROOMS_COUNT]    = {1, 10, 100, 1000};
const char amphipod_dict[ROOMS_COUNT]   = {'A', 'B', 'C', 'D'};

typedef	char map[MAP_ROWS][MAP_COLUMNS];

struct coord {
	uint8_t row,col;
	bool good;
};

typedef struct coord coords[MAX_KEY_SIZE];

void hash(map m, char map_key[MAX_KEY_SIZE]) {
	int sz = 0;
	for (int i = 0; i < AVAILABLE_HALLWAY_SPOTS; i++) {
		map_key[sz++] = m[HALLWAY_ROW][available_hallway_spots[i]];
	}

	for (int i = 0; i < ROOMS_COUNT; i++) {
		for (int j = ROOM_ROW_START; j <= ROOM_ROW_END; j++) {
			map_key[sz++] = m[j][amphipod_target[i]];
		}
	}

	map_key[sz++] = '\0';
}

void print_map(map m) {
	for (int row = 0; row < MAP_ROWS; row++) {
		printf("%d|", row);
		for (int col = 0; col < MAP_COLUMNS; col++) {
			printf("%c", m[row][col]);
		}
		printf("\n");
	}
	printf("  ");
	for (int col = 0; col < MAP_COLUMNS; col++) {
		printf("%d", col % 10);
	}
	printf("\n");
}

void print_coord(map m, struct coord c) {
	printf("%c at row:%d col:%d\n",
	       m[c.row][c.col], c.row, c.col);
}

void print_coords(coords c, int len) {
	for (int i = 0; i < len; i++) {
		printf("(%d,%d) ", c[i].row, c[i].col);
	}
	printf("\n");
}

// check_map checks that all amphipods are in place
bool check_map(map m) {
	for (int room = 0; room < ROOMS_COUNT; room++) {
		int col = amphipod_target[room];
		for (int row = ROOM_ROW_START; row <= ROOM_ROW_END; row++) {
			if (m[row][col] != amphipod_dict[room]) return false;
		}
	}
	return true;
}

bool is_stuck(map m, struct coord c) {
	if (c.row == HALLWAY_ROW) {
		return false;
	}

	for (int row = c.row - 1; row >= ROOM_ROW_START; row--) {
		if (!IS_EMPTY(m[row][c.col])) return true;
	}

	return false;
};

int path_len(struct coord from, struct coord to) {
	int len = ABS(from.col - to.col);

	len += ABS(HALLWAY_ROW - to.row);
	if (from.row != HALLWAY_ROW) {
		len += ABS(HALLWAY_ROW - from.row);
	}

	return len;
}

int energy_cost(map m, struct coord from, struct coord to) {
	char amphipod = m[from.row][from.col];
	int factor = amphipod_cost[amphipod - 'A'];
	return path_len(from, to) * factor;
}

bool is_in_right_spot(map m, struct coord c) {
	int col = amphipod_target[m[c.row][c.col] - 'A'];
	if (c.col != col) return false;

	char expected = m[c.row][col];
	for (int row = c.row; row <= ROOM_ROW_END; row++) {
		if (m[row][col] != expected) return false;
	}

	return true;
}

void move(map m, struct coord *from, struct coord to) {
	m[to.row][to.col] = m[from->row][from->col];
	m[from->row][from->col] = '.';
	from->row = to.row;
	from->col = to.col;
	from->good = is_in_right_spot(m, *from);
}

void get_amphipods(map m, coords coords, int *len) {
	for (int i = 0; i < AVAILABLE_HALLWAY_SPOTS; i++) {
		int col = available_hallway_spots[i];
		if (IS_EMPTY(m[HALLWAY_ROW][col])) continue;
		coords[(*len)++] = (struct coord){
			.row = HALLWAY_ROW,
			.col = col,
		};
	}

	for (int room = 0; room < ROOMS_COUNT; room++) {
		int col = amphipod_target[room];
		for (int row = ROOM_ROW_START; row <= ROOM_ROW_END; row++) {
			if (IS_EMPTY(m[row][col])) continue;
			coords[(*len)++] = (struct coord){
				.row = row,
				.col = col,
			};
		}
	}
}

void get_hallway_spots(map m, struct coord c, coords coords, int *len) {
	int starting_index = 0;
	for (int i = 0; i < AVAILABLE_HALLWAY_SPOTS; i++) {
		if (c.col <= available_hallway_spots[i]) {
			starting_index = i;
			break;
		}
	}

	for (int i = starting_index; i < AVAILABLE_HALLWAY_SPOTS; i++) {
		uint8_t idx = available_hallway_spots[i];
		if (idx == c.col) continue;
		if (!IS_EMPTY(m[HALLWAY_ROW][idx])) {
			break;
		}

		coords[(*len)++] = (struct coord){ .row = HALLWAY_ROW, .col = idx };
	}

	for (int i = starting_index - 1; i >= 0; i--) {
		uint8_t idx = available_hallway_spots[i];
		if (idx == c.col) continue;
		if (!IS_EMPTY(m[HALLWAY_ROW][idx])) {
			break;
		}

		coords[(*len)++] = (struct coord){ .row = HALLWAY_ROW, .col = idx };
	}
}

void get_room_spots(map m, struct coord c, coords coords, int *len) {
	// a room is available if it's empty, or if one of the correct amphipod
	// is already in there and the place above is empty
	int col = amphipod_target[m[c.row][c.col] - 'A'];

	// if we're already in the good column, return
	if (col == c.col) return;

	char a = m[c.row][c.col];

	for (int row = ROOM_ROW_END; row >= ROOM_ROW_START; row--) {
		if (IS_EMPTY(m[row][col])) {
			coords[(*len)++] = (struct coord){ .row = row, .col = col };
		}

		if (m[row][col] == a) {
			continue;
		} else {
			return;
		};
	}
}

uint8_t get_room_entry(map m, struct coord c) {
	int from = c.col;
	int to = amphipod_target[m[c.row][c.col] - 'A'];
	return (from < to) ? to-1 : to+1;
}

void get_spots(map m, struct coord c, coords co, int *len) {
	// printf("Getting spots for:\n");
	// print_coord(m, c);

	if (is_stuck(m, c)) {
		return;
	}

	coords tmp;
	memcpy(tmp, co, sizeof(coords));
	int tmp_len = 0;

	get_hallway_spots(m, c, tmp, &tmp_len);
	if (ABS(c.col - amphipod_target[m[c.row][c.col] - 'A']) == 1) {
		get_room_spots(m, c, co, len);
		return;
	}

	uint8_t idx = get_room_entry(m, c);
	if (!idx) {
		return;
	}

	if (c.row != HALLWAY_ROW) {
		memcpy(co, tmp, sizeof(coords));
		*len = tmp_len;
	}

	for (int i = 0; i < tmp_len; i++) {
		if (tmp[i].col == idx) {
			get_room_spots(m, c, co, len);
			return;
		}
	}
}

void solve(map m, coords amphipods, int amphipod_count, int cost) {
	char h_key[MAX_KEY_SIZE];
	hash(m, h_key);

	ENTRY entry = { .key = h_key };
	ENTRY *found = hsearch(entry, FIND);
	if (found != NULL) {
		if (cost >= *(int *)found->data) return;
		*(int *)found->data = cost;
	} else {
		int *data = (int *) malloc(sizeof(int));
		*data = cost;
		entry.data = data;
		hsearch(entry, ENTER);
	}

	for (int i = 0; i < amphipod_count; i++) {
		struct coord amphipod = amphipods[i];

		int moves_count = 0;
		coords moves;
		get_spots(m, amphipod, moves, &moves_count);

		for (int j = 0; j < moves_count; j++) {
			map new_map;
			memcpy(new_map, m, sizeof(map));

			coords new_amphipods;
			memcpy(new_amphipods, amphipods, sizeof(coords));
			int new_count = amphipod_count;

			struct coord *a = &new_amphipods[i];
			int new_cost = cost + energy_cost(new_map, *a, moves[j]);
			move(new_map, a, moves[j]);
			if (a->good) {
				int s = new_count - 1 - i;
				if (s > 0) {
					memmove(&new_amphipods[i],
						&new_amphipods[i+1],
						s * sizeof(struct coord));
				}
				new_count--;
			}
			if (new_cost >= min_cost) {
				continue;
			}

			if (check_map(new_map)) {
				min_cost = MIN(min_cost, new_cost);
				continue;
			}

			solve(new_map, new_amphipods, new_count, new_cost);
		}
	}

	return;
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

	map m;
	int row = 0;
	char *line = NULL;
	size_t n;
	while(getline(&line, &n, file) != -1) {
		memcpy(m[row++], line, MAP_COLUMNS*sizeof(char));
		if (row == 3) {
			memcpy(m[row++], "  #D#C#B#A#  ", MAP_COLUMNS*sizeof(char));
			memcpy(m[row++], "  #D#B#A#C#  ", MAP_COLUMNS*sizeof(char));
		}
		if (row - 1 > ROOM_ROW_START) {
			m[row-1][MAP_COLUMNS-2] = ' ';
			m[row-1][MAP_COLUMNS-1] = ' ';
		}
	}
	free(line);
	fclose(file);

	coords tmp, amphipods;
	int tmp_count = 0, amphipod_count = 0;
	get_amphipods(m, tmp, &tmp_count);

	for (int i = tmp_count - 1; i >= 0; i--) {
		struct coord *amphipod = &tmp[i];
		amphipod->good = is_in_right_spot(m, *amphipod);
		if (!amphipod->good) {
			memcpy(&amphipods[amphipod_count++],
			       amphipod,
			       sizeof(struct coord));
		}
	}

	if (hcreate(HASH_SIZE) < 0) {
		printf("\nFailed to create hash\n");
		return 1;
	};

	solve(m, amphipods, amphipod_count, 0);

	hdestroy();

	printf("%d\n", min_cost);
	return 0;
}
