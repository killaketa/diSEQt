#ifndef COMMONFUNCSHEADER
#define COMMONFUNCSHEADER

#include <stdint.h>

uint16_t byteswap16(uint16_t src);
uint32_t byteswap32(uint32_t src);

int decode_vlq(unsigned char* Buffer, int* Offset);

#endif // !COMMONFUNCSHEADER
