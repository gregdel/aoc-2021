#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INPUT 655

uint64_t result = 0;

struct reader {
	uint8_t data[MAX_INPUT];
	unsigned int bytes, pos;
};

uint64_t read_bit(struct reader *r) {
	int byte = r->pos / 8;
	int pos = r->pos % 8;
	r->pos++;
	return (r->data[byte] >> (7 - pos)) & 1;
}

uint64_t read_bits(struct reader *r, uint8_t n) {
	if (n >= 64) return 0;
	uint64_t ret = 0;
	for (int i = n - 1; i >= 0; i--) {
		ret |= read_bit(r) << i;
	}
	return ret;
}

uint64_t decode_literal_value(struct reader *r) {
	uint64_t group_bit = 1;
	uint64_t ret = 0;
	while (group_bit != 0) {
		group_bit = read_bit(r);
		ret = (ret << 4) | read_bits(r, 4);
	}
	return ret;
}

uint64_t decode_packet(struct reader *r) {
	uint64_t start_pos = r->pos;
	uint64_t version = read_bits(r, 3);
	result += version;
	uint64_t type_id = read_bits(r, 3);
	if (type_id == 4) {
		decode_literal_value(r);
	} else {
		if (read_bit(r) == 0) {
			uint64_t len_bits = read_bits(r, 15);
			uint64_t bits_read = 0;
			while (bits_read < len_bits) {
				bits_read += decode_packet(r);
			}
		} else {
			uint64_t sub_packets = read_bits(r, 11);
			for (int i = 0; i < sub_packets; i++)
				decode_packet(r);
		}
	}

	return r->pos - start_pos;
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

	struct reader r;
	r.bytes = 0;
	r.pos = 0;

	unsigned int c;
	for (;;) {
		if (fscanf(file, "%2x", &c) == EOF) break;
		r.data[r.bytes++] = c;
	}
	fclose(file);

	decode_packet(&r);
	printf("%ld\n", result);
	return 0;
}
