#ifndef RSEQPARSEHEADER // Don't want to load this multiple times since this file is called in multiple places.
#define RSEQPARSEHEADER
#include "brseq.h"

// Used to store the offsets of offset tags 
typedef struct {
	unsigned int TagOffset; // Offset in bytes from 0x0C of the DATA section. Gets set when an offset tag with the corresponding string is found.
	char String[100]; // The string associated with this offset. Set when an offset tag has a not seen before offset name.
}offset_t;

// Used to store the offset of the command value and offset string requested
typedef struct {
	unsigned int CmdValueOffset; // Offset of the jump/call commands value.
	char String[100]; // The string whos offset will be searched for after all commands finish writing.
}cmdoffset_t;


char* parse_textstream(FILE* TextStream, FILE* ByteStream, brseq_t BRSEQ);

#endif