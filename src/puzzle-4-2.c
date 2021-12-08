#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BINGO_ROW_SIZE 5
#define BINGO_BOARD_SIZE BINGO_ROW_SIZE * BINGO_ROW_SIZE
#define BINGO_BOARD_COUNT_MAX 200
#define BINGO_NUMBERS_MAX 100

struct bingo_board {
	bool resolved;
	int data[BINGO_ROW_SIZE * BINGO_ROW_SIZE];
	bool marked[BINGO_ROW_SIZE * BINGO_ROW_SIZE];
};

void init_board(struct bingo_board *board) {
	board->resolved = false;
	for (int i = 0; i < BINGO_BOARD_SIZE; i++) {
		board->data[i] = 0;
		board->marked[i] = false;
	}
}

int unmarked_sum(struct bingo_board *board) {
	int sum = 0;
	for (int i = 0; i < BINGO_BOARD_SIZE; i++) {
		if (board->marked[i])
			continue;

		sum += board->data[i];
	}

	return sum;
}

bool is_row_complete(struct bingo_board *board, int row) {
	for (int i = 0; i < BINGO_ROW_SIZE; i++) {
		if (!board->marked[row * BINGO_ROW_SIZE + i])
			return false;
	}

	return true;
}

bool is_col_complete(struct bingo_board *board, int col) {
	for (int i = 0; i < BINGO_ROW_SIZE; i++) {
		if (!board->marked[col + i * BINGO_ROW_SIZE])
			return false;
	}

	return true;
}

bool is_complete(struct bingo_board *board) {
	if (board->resolved)
		return true;

	for (int i = 0; i < BINGO_ROW_SIZE; i++) {
		if (is_row_complete(board, i)) {
			board->resolved = true;
			break;
		}

		if (is_col_complete(board, i)) {
			board->resolved = true;
			break;
		}
	}

	return board->resolved;
}

void mark_board(struct bingo_board *board, int number) {
	for (int i = 0; i < BINGO_BOARD_SIZE; i++) {
		if (board->data[i] == number)
			board->marked[i] = true;
	}
}

void print_board(struct bingo_board *board) {
	printf("---------\n");
	for (int i = 0; i < BINGO_ROW_SIZE * BINGO_ROW_SIZE; i++) {
		if (i % 5 == 0)
			printf("\n");


		if (board->marked[i]) {
			printf("(%d)\t", board->data[i]);
		} else {
			printf("%d\t", board->data[i]);
		}

	}
	printf("\n");
}


int parse_numbers(char *input, int output[]) {
	int pos = 0;
	int current;

	int i = 0;
	int j = 0;
	char buf[3];

	bool running = true;
	while (running) {
		switch (input[i]) {
		case '\0':
			running = false;
			// Continue as if the last char was a ","
		case ',':
			buf[j] = '\0';
			j = 0;
			current = atoi(buf);
			output[pos] = current;
			pos++;
			break;
		default:
			buf[j] = input[i];
			j++;
		}

		i++;
	}

	return pos;
}

int parse_input(FILE *file,
		int numbers[],
		int *number_count,
		struct bingo_board boards[],
		int *board_count) {


	char numbers_raw[1024];
	fscanf(file, "%s\n", numbers_raw);

	int input_count = parse_numbers(numbers_raw, numbers);
	if (input_count <= 0) {
		printf("failed to parse numbers\n");
		return 1;
	}
	*number_count = input_count;

	int row = 0;
	int i = 0;
	int x_1, x_2, x_3, x_4, x_5;
	while(fscanf(file, "%d %d %d %d %d\n", &x_1, &x_2, &x_3, &x_4,&x_5) != EOF) {
		if (row == 5) {
			i++;
			row = 0;
		}

		boards[i].data[row * BINGO_ROW_SIZE + 0] = x_1;
		boards[i].data[row * BINGO_ROW_SIZE + 1] = x_2;
		boards[i].data[row * BINGO_ROW_SIZE + 2] = x_3;
		boards[i].data[row * BINGO_ROW_SIZE + 3] = x_4;
		boards[i].data[row * BINGO_ROW_SIZE + 4] = x_5;

		row++;
	}
	i++;
	*board_count = i;

	fclose(file);

	return 0;
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

	int numbers_count, board_count;
	int numbers[BINGO_NUMBERS_MAX];
	struct bingo_board boards[BINGO_BOARD_COUNT_MAX];
	for (int i = 0; i < BINGO_BOARD_COUNT_MAX; i++) {
		init_board(&boards[i]);
	}

	if (parse_input(file, numbers, &numbers_count, boards, &board_count) != 0) {
		printf("Failed to parse input...\n");
		return 1;
	}

	int resolved = 0;
	for (int n = 0; n < numbers_count; n++) {
		for (int i = 0; i < board_count; i++) {
			if (is_complete(&boards[i]))
				continue;

			mark_board(&boards[i], numbers[n]);
			if (is_complete(&boards[i])) {
				resolved++;
				if (resolved != board_count)
					continue;

				int sum = unmarked_sum(&boards[i]);
				printf("%d\n", sum * numbers[n]);

				return 0;
			}
		}
	}

	return 0;
}
