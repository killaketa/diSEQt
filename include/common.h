#ifndef COMMONFUNCSHEADER
#define COMMONFUNCSHEADER

#define XMLSEQ_START_STRING "// Created by diSEQt\n// Program made by Keta\n// https://github.com/killaketa/diSEQt\n\n"

#include <stdint.h>

uint16_t byteswap16(uint16_t src);
uint32_t byteswap32(uint32_t src);

int decode_vlq(unsigned char* Buffer, int* Offset);
int decode_vlq_bytestream(FILE* Buffer);

#endif // !COMMONFUNCSHEADER
