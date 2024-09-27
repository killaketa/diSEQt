#include <stdio.h>
#include "common.h"

uint16_t byteswap16(uint16_t src) {
	const unsigned char* src2 = (unsigned char*)&src;
	return (uint16_t)(src2[0] << 8) | (uint16_t)(src2[1] << 0);
	return src2;
}
uint32_t byteswap32(uint32_t src) {
	const unsigned char* src2 = (unsigned char*)&src;
	return ((uint32_t)src2[0] << 24) | ((uint32_t)src2[1] << 16) | ((uint32_t)src2[2] << 8) | ((uint32_t)src2[3] << 0);
	return src2;
}

int decode_vlq(unsigned char* Buffer, int* Offset) {
	unsigned char* Byte = &Buffer[*Offset];
	int holder = 0;
	int done = 0;

	while (done == 0) {
		// Compares the binary of Byte (EX. 10010100) with 10000000 (done by getting the binary of 1 (00000001) and shifting it left 7 times.). Is 0 if MSB is 0 or 128 if MSB is 1.
		int firstbit = *Byte & (1 << 7);

		holder = (holder << 7) | (*Byte & (0x7F)); // Set the holder to the current binary shifted right 7 times bitwise OR'd with 01111111. (EX. 01111111 and 01111111 to 01111111 1111111)

		if (firstbit == 0) {
			done = 1;
			break;
		}
		else {
			*Offset += 1;
			Byte++;
		}
	}

	return holder;
}

int decode_vlq_bytestream(FILE* Buffer) {
	unsigned char* Byte = fgetc(Buffer);
	int holder = 0;
	int done = 0;

	while (done == 0) {
		// Compares the binary of Byte (EX. 10010100) with 10000000 (done by getting the binary of 1 (00000001) and shifting it left 7 times.). Is 0 if MSB is 0 or 128 if MSB is 1.
		int firstbit = *Byte & (1 << 7);

		holder = (holder << 7) | (*Byte & (0x7F)); // Set the holder to the current binary shifted right 7 times bitwise OR'd with 01111111. (EX. 01111111 and 01111111 to 01111111 1111111)

		if (firstbit == 0) {
			done = 1;
			break;
		}
		else {
			Byte = fgetc(Buffer);
		}
	}

	return holder;
}