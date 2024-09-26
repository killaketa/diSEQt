#ifndef BRSEQCMDHEADER // Don't want to load this multiple times since this file is called in multiple places.
#define BRSEQCMDHEADER

#include "brseq.h"
char* decode_command(brseq_t BRSEQ, int* Offset, char** StrBuffer, int* ImportantOffsets);

#endif