#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define WINNING_SCORE 1000

struct dice {
	int rolls;
	int value;
};

int roll(struct dice *d) {
	d->rolls++;
	d->value++;
	if (d->value > 100) d->value %= 100;
	return d->value;
}

struct player {
	int score;
	int position;
};

int play(struct player *p, struct dice *d) {
	int v = p->position;
	for (int i = 0; i < 3; i++) v += roll(d);
	v = ((v-1) % 10) + 1;
	p->position = v;
	p->score += v;
	return p->score;
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

	struct dice dice;
	dice.rolls = 0;
	dice.value = 0;
	struct player players[2];

	char *line = NULL;
	size_t n;
	while(getline(&line, &n, file) != -1) {
		int p = 0, s = 0;
		sscanf(line, "Player %d starting position: %d\n", &p, &s);
		players[p-1].position = s;
		players[p-1].score = 0;
	}
	free(line);
	fclose(file);

	int s = 0;
	int loser = 0;
	bool c = true;
	while (c) {
		for (int j = 0; j < 2; j++) {
			s = play(&players[j], &dice);
			if (s >= WINNING_SCORE) {
				loser = (j == 0) ? 1 : 0;
				c = false;
				break;
			}
		}
	}

	printf("%d\n", dice.rolls * players[loser].score);
	return 0;
}
