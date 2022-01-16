#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MIN_POINTS 12
#define MAX_POINTS 30
#define MAX_SCANNERS 35
#define MAX_ROTATION 24

int abs(int n) { return n >= 0 ? n : n * -1; }

typedef struct vector { int x, y, z; } vector;
typedef struct matrix { vector a, b, c; } matrix;
typedef struct transformation {
	bool found;
	bool translated;
	vector u;
	int rotation;
} transformation;

// Rotation of 90° around z
matrix rotations[MAX_ROTATION] = {
	{{ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }},
	{{ 0, 0, 1 }, { 0, 1, 0 }, {-1, 0, 0 }},
	{{-1, 0, 0 }, { 0, 1, 0 }, { 0, 0,-1 }},
	{{ 0, 0,-1 }, { 0, 1, 0 }, { 1, 0, 0 }},
	{{ 0,-1, 0 }, { 1, 0, 0 }, { 0, 0, 1 }},
	{{ 0, 0, 1 }, { 1, 0, 0 }, { 0, 1, 0 }},
	{{ 0, 1, 0 }, { 1, 0, 0 }, { 0, 0,-1 }},
	{{ 0, 0,-1 }, { 1, 0, 0 }, { 0,-1, 0 }},
	{{ 0, 1, 0 }, {-1, 0, 0 }, { 0, 0, 1 }},
	{{ 0, 0, 1 }, {-1, 0, 0 }, { 0,-1, 0 }},
	{{ 0,-1, 0 }, {-1, 0, 0 }, { 0, 0,-1 }},
	{{ 0, 0,-1 }, {-1, 0, 0 }, { 0, 1, 0 }},
	{{ 1, 0, 0 }, { 0, 0,-1 }, { 0, 1, 0 }},
	{{ 0, 1, 0 }, { 0, 0,-1 }, {-1, 0, 0 }},
	{{-1, 0, 0 }, { 0, 0,-1 }, { 0,-1, 0 }},
	{{ 0,-1, 0 }, { 0, 0,-1 }, { 1, 0, 0 }},
	{{ 1, 0, 0 }, { 0,-1, 0 }, { 0, 0,-1 }},
	{{ 0, 0,-1 }, { 0,-1, 0 }, {-1, 0, 0 }},
	{{-1, 0, 0 }, { 0,-1, 0 }, { 0, 0, 1 }},
	{{ 0, 0, 1 }, { 0,-1, 0 }, { 1, 0, 0 }},
	{{ 1, 0, 0 }, { 0, 0, 1 }, { 0,-1, 0 }},
	{{ 0,-1, 0 }, { 0, 0, 1 }, {-1, 0, 0 }},
	{{-1, 0, 0 }, { 0, 0, 1 }, { 0, 1, 0 }},
	{{ 0, 1, 0 }, { 0, 0, 1 }, { 1, 0, 0 }}
};

vector multiply(matrix m, vector v) {
	return (vector) {
		.x = m.a.x * v.x + m.a.y * v.y + m.a.z * v.z,
		.y = m.b.x * v.x + m.b.y * v.y + m.b.z * v.z,
		.z = m.c.x * v.x + m.c.y * v.y + m.c.z * v.z,
	};
}

vector multiply_vector(vector u, int s) {
	return (vector) {
		.x = s * u.x,
		.y = s * u.y,
		.z = s * u.z,
	};
}

vector add_vector(vector u, vector v) {
	return (vector){
		.x = u.x + v.x,
		.y = u.y + v.y,
		.z = u.z + v.z,
	};
}

vector sub_vector(vector u, vector v) {
	return (vector){
		.x = u.x - v.x,
		.y = u.y - v.y,
		.z = u.z - v.z,
	};
}

void print_vector(vector u) {
	printf("(%d, %d, %d)", u.x, u.y, u.z);
}

int distance(vector u, vector v) {
	return abs(u.x - v.x) + abs(u.y - v.y) + abs(u.z - v.z);
}

typedef struct scanner scanner;
struct scanner {
	int id;
	int count;
	vector position;
	vector vectors[MAX_POINTS];
	transformation ts[MAX_SCANNERS];
	bool aligned_with_base;
};

void init_scanner(scanner *s, int id) {
	s->id = id;
	s->count = 0;
	s->aligned_with_base = false;
	s->position = (vector){ .x = 0, .y = 0, .z = 0 };
	memset(s->vectors, 0, sizeof(vector[MAX_POINTS]));
	memset(s->ts, 0, sizeof(transformation[MAX_SCANNERS]));
}

vector transform_vector(scanner *from, scanner *to, vector u) {
	vector v = multiply(rotations[to->ts[from->id].rotation], u);
	return add_vector(to->ts[from->id].u, v);
}

bool eq_vectors(vector v1, vector v2) {
	return ((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z));
}

bool match_distances(int distances[MAX_POINTS], int count, scanner *s2, int i) {
	int matches = 0;
	int tmp[MAX_POINTS];
	memcpy(tmp, distances, sizeof(tmp));
	for (int j = 0; j < s2->count; j++) {
		if (i == j) continue;
		int d = distance(s2->vectors[i], s2->vectors[j]);
		for (int l = 0; l < count; l++) {
			if (d != tmp[l]) continue;
			tmp[l] = -1;
			matches++;
			if (matches >= (MIN_POINTS - 1)) return true;
		}
	}
	return false;
}

bool find_transformation(scanner *s1, scanner *s2, int matches[3][2]) {
	if (s1->ts[s2->id].found) return true;

	//
	int i = matches[0][0];
	int j = matches[0][1];
	vector p_s1 = s1->vectors[i];
	vector p_s2 = s2->vectors[j];
	for (int r = 0; r < MAX_ROTATION; r++) {
		vector rp_s2 = multiply(rotations[r], p_s2);
		vector u = sub_vector(p_s1, rp_s2);

		int match_count = 0;
		for (int k = 1; k < 3; k++) {
			int tmp_i = matches[k][0];
			int tmp_j = matches[k][1];
			vector tmp_p_s1 = s1->vectors[tmp_i];
			vector tmp_p_s2 = s2->vectors[tmp_j];
			vector tmp_rp_s2 = multiply(rotations[r], tmp_p_s2);
			vector v = sub_vector(tmp_p_s1, tmp_rp_s2);

			if (eq_vectors(u, v)) {
				match_count++;
				if (match_count == 2) {
					s1->ts[s2->id].u = u;
					s1->ts[s2->id].found = true;
					s1->ts[s2->id].rotation = r;
					return true;
				}
			}
		}
	}

	return false;
}

bool align_scanners(scanner *s1, scanner *s2) {
	int matches[3][2];
	int match_count = 0;
	for (int i = 0; i < s1->count; i++) {
		int distances[MAX_POINTS] = {0};
		int count = 0;
		for (int j = 0; j < s1->count; j++) {
			if (i == j) continue;
			distances[count++] = distance(
			      s1->vectors[i], s1->vectors[j]);
		}

		for (int j = 0; j < s2->count; j++) {
			if (!match_distances(distances, count, s2, j)) continue;
			matches[match_count][0] = i;
			matches[match_count][1] = j;
			match_count++;

			if (match_count < 3) continue;
			if (find_transformation(s1, s2, matches)) {
				return true;
			}
			match_count = 0;
		}
	}

	return false;
}

void print_scanner(scanner *s, int max) {
	printf("scanner %d (%d, %d, %d):\n",
	       s->id, s->position.x, s->position.y, s->position.z);
	for (int i = 0; i <= max; i++) {
		if (i == s->id) continue;
		if (!s->ts[i].found) continue;
		printf("\t%d: ", i);
		print_vector(s->ts[i].u);
		printf("[%d][%s]\n", s->ts[i].rotation,
		       s->aligned_with_base ? "x" : " ");
	}
	printf("Points:\n");
	for (int i = 0; i < s->count; i++) {
		printf("\t");
		print_vector(s->vectors[i]);
		printf("\n");
	}
}

void translate_scanner(scanner scanners[MAX_SCANNERS], int count,
		       int list[MAX_SCANNERS], scanner *base, int depth) {
	if (base->aligned_with_base) return;
	for (int i = 0; i < depth; i++) {
		if (list[i] == base->id) return;
	}
	list[depth++] = base->id;

	base->position = scanners[list[depth-1]].ts[base->id].u;
	for (int d = depth - 1; d > 0; d--) {
		scanner *from = &scanners[list[d]];
		scanner *to = &scanners[list[d-1]];
		for (int i = 0; i < base->count; i++) {
			base->vectors[i] = transform_vector(from, to,
							    base->vectors[i]);

		}
		base->position = transform_vector(from, to,
						  base->position);
	}
	scanners[base->id].aligned_with_base = true;

	for (int i = 0; i <= count; i++) {
		if (i == base->id) continue;
		if (scanners[i].ts[base->id].found == false) continue;
		translate_scanner(scanners, count, list, &scanners[i], depth);
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

	scanner scanners[MAX_SCANNERS];

	char *line = NULL;
	size_t n;
	int count, x, y, z;
	scanner *s = NULL;
	while(getline(&line, &n, file) != -1) {
		if (s == NULL) {
			if (sscanf(line, "--- scanner %d ---\n", &count) <= 0) break;
			s = &scanners[count];
			init_scanner(s, count);
			continue;
		}

		if (line[0] == '\n') {
			s = NULL;
			continue;
		}

		if (sscanf(line, "%d,%d,%d\n", &x, &y, &z) <= 0) break;
		s->vectors[s->count++] = (vector){.x = x, .y = y, .z = z};
	}
	free(line);
	fclose(file);

	for (int i = 0; i <= count; i++) {
		for (int j = 0; j <= count; j++) {
			if (i == j) continue;
			if (scanners[i].ts[j].found == true) continue;
			if (!align_scanners(&scanners[i], &scanners[j]))
				continue;
		}
	}

	int list[MAX_SCANNERS];
	translate_scanner(scanners, count ,list, &scanners[0], 0);

	bool all_aligned = true;
	for (int i = 0; i <= count; i++) {
		if (scanners[i].aligned_with_base == true) continue;
		all_aligned = false;
		printf("Scanner %d not aligned !\n", i);
	}
	if (!all_aligned) return 0;

	int vcount = 0;
	int already_in_list = 0;
	vector vlist[MAX_POINTS * MAX_SCANNERS];
	for (int i = 0; i <= count; i++) {
		scanner *s = &scanners[i];
		for (int j = 0; j < s->count; j++) {
			vector u = s->vectors[j];
			bool found = false;
			for (int k = 0; k < vcount; k++) {
				if (eq_vectors(u, vlist[k])) {
					found = true;
					already_in_list++;
					break;
				};
			}

			if (found) continue;
			vlist[vcount++] = u;
		}
	}

	printf("%d\n", vcount);
	return 0;
}
