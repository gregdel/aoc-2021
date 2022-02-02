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
// 1: #.....D.....#
// 2: ###B#C#B#D###
// 3:   #A#D#C#A#
// 4:   #########
#define HASH_SIZE 100000
#define ROOMS_COUNT 4
#define ROOMS_SIZE 2
#define MAP_LINES 5
#define MAP_COLUMNS 13
#define HALLWAY_LINE 1
#define BOTTOM_LINE 3
#define AVAILABLE_HALLWAY_SPOTS 7
#define MAX_KEY_SIZE AVAILABLE_HALLWAY_SPOTS + ROOMS_COUNT * ROOMS_SIZE
#define MAX_SPOTS AVAILABLE_HALLWAY_SPOTS + 2

const uint8_t available_hallway_spots[] = {1, 2, 4, 6, 8, 10, 11};
const int amphipod_target[ROOMS_COUNT]  = {3, 5, 7, 9};
const int amphipod_cost[ROOMS_COUNT]    = {1, 10, 100, 1000};
const char amphipod_dict[ROOMS_COUNT]   = {'A', 'B', 'C', 'D'};

typedef	char map[MAP_LINES][MAP_COLUMNS];

struct coord {
	uint8_t x,y;
	bool good;
};

typedef struct coord coords[MAX_SPOTS];

void hash(map m, char map_key[MAX_KEY_SIZE]) {
	int sz = 0;
	for (int i = 0; i < AVAILABLE_HALLWAY_SPOTS; i++) {
		map_key[sz++] = m[HALLWAY_LINE][available_hallway_spots[i]];
	}

	for (int i = 0; i < ROOMS_COUNT; i++) {
		for (int j = BOTTOM_LINE - 1; j <= BOTTOM_LINE; j++) {
			map_key[sz++] = m[j][amphipod_target[i]];
		}
	}

	map_key[sz++] = '\0';
}

void print_map(map m) {
	for (int i = 0; i < MAP_LINES; i++) {
		for (int j = 0; j < MAP_COLUMNS; j++) {
			printf("%c", m[i][j]);
		}
		printf("\n");
	}
}

void print_coords(coords c, int len) {
	for (int i = 0; i < len; i++) {
		printf("(%d,%d) ", c[i].x, c[i].y);
	}
	printf("\n");
}

// check_map checks that all amphipods are in place
int check_map(map m) {
	for (int i = 0; i < 4; i++) {
		for (int j = 2; j < 4; j++) {
			int idx = amphipod_target[i];
			if (m[j][idx] != amphipod_dict[i]) return 0;
		}
	}
	return 1;
}

bool is_stuck(map m, struct coord c) {
	if ((c.x == BOTTOM_LINE) && !IS_EMPTY(m[c.x-1][c.y])) {
		return true;
	}

	return false;
};

int path_len(struct coord from, struct coord to) {
	int len = ABS(from.y - to.y);

	len += ABS(HALLWAY_LINE - to.x);
	if (from.x != HALLWAY_LINE) {
		len += ABS(HALLWAY_LINE - from.x);
	}

	return len;
}

int energy_cost(map m, struct coord from, struct coord to) {
	char amphipod = m[from.x][from.y];
	int factor = amphipod_cost[amphipod - 'A'];
	return path_len(from, to) * factor;
}

bool is_in_right_spot(map m, struct coord c) {
	int col = amphipod_target[m[c.x][c.y] - 'A'];
	if (c.y != col) return false;

	if (c.x == BOTTOM_LINE) return true;
	if ((c.x == BOTTOM_LINE-1) && (m[BOTTOM_LINE][c.y] == m[c.x][c.y]))
		return true;

	return false;
}


void move(map m, struct coord *from, struct coord to) {
	m[to.x][to.y] = m[from->x][from->y];
	m[from->x][from->y] = '.';
	from->x = to.x;
	from->y = to.y;
	from->good = is_in_right_spot(m, *from);
}

void get_amphipods(map m, coords coords, int *len) {
	for (int i = 0; i < AVAILABLE_HALLWAY_SPOTS; i++) {
		int idx = available_hallway_spots[i];
		if (IS_EMPTY(m[HALLWAY_LINE][idx])) continue;
		coords[(*len)++] = (struct coord){
			.x = HALLWAY_LINE,
			.y = idx,
		};
	}

	for (int i = 0; i < ROOMS_COUNT; i++) {
		int col = amphipod_target[i];
		for (int j = BOTTOM_LINE - 1; j <= BOTTOM_LINE; j++) {
			if (IS_EMPTY(m[j][col])) continue;
			coords[(*len)++] = (struct coord){ .x = j, .y = col };
		}
	}
}

void get_hallway_spots(map m, struct coord c, coords coords, int *len) {
	int starting_index = 0;
	for (int i = 0; i < AVAILABLE_HALLWAY_SPOTS; i++) {
		if (c.y <= available_hallway_spots[i]) {
			starting_index = i;
			break;
		}
	}

	for (int i = starting_index; i < AVAILABLE_HALLWAY_SPOTS; i++) {
		uint8_t idx = available_hallway_spots[i];
		if (idx == c.y) continue;
		if (!IS_EMPTY(m[HALLWAY_LINE][idx])) {
			break;
		}

		coords[(*len)++] = (struct coord){ .x = HALLWAY_LINE, .y = idx };
	}

	for (int i = starting_index - 1; i >= 0; i--) {
		uint8_t idx = available_hallway_spots[i];
		if (idx == c.y) continue;
		if (!IS_EMPTY(m[HALLWAY_LINE][idx])) {
			break;
		}

		coords[(*len)++] = (struct coord){ .x = HALLWAY_LINE, .y = idx };
	}
}

void get_room_spots(map m, struct coord c, coords coords, int *len) {
	// a room is available if it's empty, or if one of the correct amphipod
	// is already in there and the place above is empty
	int idx = amphipod_target[m[c.x][c.y] - 'A'];

	// if we're already in the good column, return
	if (idx == c.y) return;

	if (IS_EMPTY(m[BOTTOM_LINE][idx])) {
		coords[(*len)++] = (struct coord){ .x = BOTTOM_LINE, .y = idx };
		return;
	}

	if ((m[BOTTOM_LINE][idx] == m[c.x][c.y]) && (IS_EMPTY(m[BOTTOM_LINE-1][idx]))) {
		coords[(*len)++] = (struct coord){ .x = BOTTOM_LINE - 1, .y = idx };
		return;
	}
}

uint8_t get_room_entry(map m, struct coord c) {
	int from = c.y;
	int to = amphipod_target[m[c.x][c.y] - 'A'];
	return (from < to) ? to-1 : to+1;
}

void get_spots(map m, struct coord c, coords co, int *len) {
	if (is_stuck(m, c)) {
		return;
	}

	coords tmp;
	memcpy(tmp, co, sizeof(coords));
	int tmp_len = 0;

	get_hallway_spots(m, c, tmp, &tmp_len);
	if (ABS(c.y - amphipod_target[m[c.x][c.y] - 'A']) == 1) {
		get_room_spots(m, c, co, len);
		return;
	}

	uint8_t idx = get_room_entry(m, c);
	if (!idx) {
		return;
	}

	if (c.x != HALLWAY_LINE) {
		memcpy(co, tmp, sizeof(coords));
		*len = tmp_len;
	}

	for (int i = 0; i < tmp_len; i++) {
		if (tmp[i].y == idx) {
			get_room_spots(m, c, co, len);
			return;
		}
	}
}

// is_room_available returns 1 if the char c is allowed to go to its room
bool is_room_available(map m, char c) {
	// a room is available if it's empty, or if one of the correct amphipod
	// is already in there and the place above is empty
	int idx = amphipod_target[c - 'A'];
	if (IS_EMPTY(m[BOTTOM_LINE][idx]) ||
	    (
	     (m[BOTTOM_LINE][idx] == c) &&
	     (IS_EMPTY(m[BOTTOM_LINE+1][idx]))
	    )) return true;
	return false;
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
	int count = 0;
	char *line = NULL;
	size_t n;
	while(getline(&line, &n, file) != -1) {
		memcpy(m[count++], line, MAP_COLUMNS*sizeof(char));
		if (count > 3) {
			m[count-1][MAP_COLUMNS-2] = ' ';
			m[count-1][MAP_COLUMNS-1] = ' ';
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
