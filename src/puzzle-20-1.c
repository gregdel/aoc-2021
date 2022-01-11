#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define unlikely(x) __builtin_expect((x),0)

#define ALGO_LEN 512
#define ITER_NB 2
#define MATRIX_SIZE 1000

struct image {
	char data[MATRIX_SIZE][MATRIX_SIZE];
	char tmp_data[MATRIX_SIZE][MATRIX_SIZE];
	char algo[ALGO_LEN];
	int width;
	int start_l, start_c;
};

void init_image(struct image *i) {
	memset(i->data, '.', sizeof(char) * MATRIX_SIZE * MATRIX_SIZE);
	memset(i->tmp_data, '.', sizeof(char) * MATRIX_SIZE * MATRIX_SIZE);
	i->width = 0;
	i->start_l = 0;
	i->start_c = 0;
}

void enhance_pixel(struct image *img, int i, int j) {
	uint64_t value = 0;
	int shift = 8;
	for (int l = i-1; l <= i+1; l++) {
		for (int c = j-1; c <= j+1; c++) {
			value |= (img->data[l][c] == '#' ? 1 : 0) << shift;
			shift--;
		}
	}

	img->tmp_data[i][j] = img->algo[value];
}

int count_pixels(struct image *img) {
	int value = 0;
	for (int i = img->start_l; i < img->start_l + img->width; i++) {
		for (int j = img->start_c; j < img->start_c + img->width; j++) {
			value += img->data[i][j] == '#';
		}
	}
	return value;
}

void display_image(struct image *img) {
	for (int i = img->start_l-2; i < img->start_l + img->width + 2; i++) {
		for (int j = img->start_c - 2; j < img->start_c + img->width + 2; j++) {
			printf("%c", img->data[i][j]);
		}
		printf("\n");
	}
	printf("image width:%d start_l:%d, start_l:%d\n",
	       img->width, img->start_l, img->start_c);
}

void handle_iteration(struct image *img, int iter) {
	for (int i = img->start_l-10; i < img->start_l + img->width + 10; i++) {
		for (int j = img->start_c - 10; j < img->start_c + img->width + 10; j++) {
			enhance_pixel(img, i, j);
		}
	}
	memcpy(&img->data, &img->tmp_data, sizeof(img->data));
	img->start_l -= 2;
	img->start_c -= 2;
	img->width += 4;
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

	bool algo_read = false;

	struct image image;
	init_image(&image);

	char *line = NULL;
	size_t n;
	int i = 0;
	while(getline(&line, &n, file) != -1) {
		if (!algo_read) {
			memcpy(image.algo, line, sizeof(char) * ALGO_LEN);
			algo_read = true;
			continue;
		}
		if (line[0] == '\n') continue;

		if (unlikely(image.width == 0)) {
			image.width = strlen(line) - 1;
			image.start_l = MATRIX_SIZE/2;
			image.start_c = MATRIX_SIZE/2;
		}
		memcpy(&image.data[image.start_l+i][image.start_c],
		       line, sizeof(char) * image.width);
		i++;

	}
	free(line);
	fclose(file);

	for (int iter = 1; iter <= ITER_NB; iter++) {
		handle_iteration(&image, iter);
	}

	printf("%d\n", count_pixels(&image));
	return 0;
}
