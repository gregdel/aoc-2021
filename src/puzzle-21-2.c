#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define WINNING_SCORE 21

const int dice_count[10] = { 0, 0, 0, 1, 3, 6, 7, 6, 3, 1 };

struct state {
	uint64_t versions;
	int p1_pos;
	int p1_score;
	int p2_pos;
	int p2_score;
};

struct games {
	uint64_t p1_wins;
	uint64_t p2_wins;
	struct state states[WINNING_SCORE][10*21*10*21];
	int state_count[WINNING_SCORE];
	struct state *cache[11][22][11][22];
};

struct state *get(struct games *g, struct state s) {
	return g->cache[s.p1_pos][s.p1_score][s.p2_pos][s.p2_score];
}

void set(struct games *g, struct state *s) {
	g->cache[s->p1_pos][s->p1_score][s->p2_pos][s->p2_score] = s;
}

void reset_cache(struct games *g) {
	memset(g->cache, 0, sizeof(g->cache));
}

void print_state(struct state *s) {
	printf("p1:(pos:%d score:%d)\tp2:(pos:%d score:%d)\t* %ld\n",
	       s->p1_pos, s->p1_score, s->p2_pos, s->p2_score, s->versions);
}

void fork_state(struct state *s, int run, struct games *g) {
	for (int dp1 = 3; dp1 < 10; dp1++) {
		struct state tmp1;
		memcpy(&tmp1, s, sizeof(struct state));

		tmp1.p1_pos += dp1;
		if (tmp1.p1_pos > 10) tmp1.p1_pos -= 10;
		tmp1.p1_score += tmp1.p1_pos;
		tmp1.versions *= dice_count[dp1];
		if (tmp1.p1_score >= WINNING_SCORE) {
			g->p1_wins += tmp1.versions;
			continue;
		}

		for (int dp2 = 3; dp2 < 10; dp2++) {
			struct state tmp2;
			memcpy(&tmp2, &tmp1, sizeof(struct state));

			tmp2.p2_pos += dp2;
			if (tmp2.p2_pos > 10) tmp2.p2_pos -= 10;
			tmp2.p2_score += tmp2.p2_pos;
			tmp2.versions *= dice_count[dp2];
			if (tmp2.p2_score >= WINNING_SCORE) {
				g->p2_wins += tmp2.versions;
				continue;
			}

			struct state *s = get(g, tmp2);
			if (s != NULL) {
				s->versions += tmp2.versions;
			} else {
				int i = g->state_count[run]++;
				memcpy(&g->states[run][i],
				       &tmp2, sizeof(struct state));
				set(g, &g->states[run][i]);
			}
		}
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

	struct games *games = malloc(sizeof(struct games));
	games->p1_wins = 0;
	games->p2_wins = 0;
	games->state_count[0] = 1;
	games->states[0][0].p1_score = 0;
	games->states[0][0].p2_score = 0;
	games->states[0][0].versions = 1;

	char *line = NULL;
	size_t n;
	while(getline(&line, &n, file) != -1) {
		int p = 0, s = 0;
		sscanf(line, "Player %d starting position: %d\n", &p, &s);
		if (p == 1) games->states[0][0].p1_pos = s;
		if (p == 2) games->states[0][0].p2_pos = s;
	}
	free(line);
	fclose(file);

	int run = 1;
	while (games->state_count[run-1] != 0) {
		games->state_count[run] = 0;
		reset_cache(games);
		for (int i = 0; i < games->state_count[run-1]; i++) {
			fork_state(&games->states[run-1][i], run, games);
		}
		run++;
	}

	printf("%ld\n", (games->p1_wins > games->p2_wins)
	       ? games->p1_wins : games->p2_wins);
	free(games);
	return 0;
}
