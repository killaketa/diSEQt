#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rseq_parse.h"
#include "common.h"

#define DefaultDelimiters "/ :,<>=\"\t\n"

#pragma warning(disable : 4996) // fuck you vc++ im not using strtok_s

int LineNum = 0;
int OffsetCount = 0;
int CmdOffsetCount = 0;
int LabelsParsedCount = 0;

// Parses VLQ (variable length quantity) and writes bytes to file.
int parse_vlq(FILE* ByteStream, long num, int iter) {
	int mask = (num & 0x7f);

	if (iter > 0) {
		mask = mask | 0x80;
	}

	if ((num >> 7) != 0) {
		iter++;
		parse_vlq(ByteStream, num >> 7, iter);
	}

	fputc(mask, ByteStream);
}

// Parses UInt24, a 24 bit integer datatype.
int parse_uint24(FILE* ByteStream, int num) {
	if (num > 16777215) {
		printf("Line %i: Number %i is too big for type UInt24. Maximum Value: 16777215.", LineNum, num);
		exit(0);
	}

	fputc((num >> 16 & 0xFF), ByteStream);
	fputc((num >> 8 & 0xFF), ByteStream);
	fputc((num & 0xFF), ByteStream);
}

// Parses command offset string, properly queues it, and reallocs the CmdOffsetsPtr.
int parse_cmdoffset(FILE* ByteStream, char* str, cmdoffset_t** CmdOffsetsPtr) {
	CmdOffsetCount++;
	cmdoffset_t* temp = realloc(*CmdOffsetsPtr, CmdOffsetCount * sizeof(cmdoffset_t));
	if (temp == NULL) {
		printf("Line %i: CmdOffsetsPtr is NULL.", LineNum);
		exit(-1);
	}

	int strsize = strnlen(str, 100) * sizeof(char) + 1;
	printf("CMD %x--------------------------------------------------------------\n", &temp[CmdOffsetCount - 1].String);

	temp[CmdOffsetCount - 1].CmdValueOffset = ftell(ByteStream);
	strncpy(temp[CmdOffsetCount - 1].String, str, 100);
	temp[CmdOffsetCount - 1].String[99] = '\0';


	parse_uint24(ByteStream, CmdOffsetCount);

	*CmdOffsetsPtr = temp;
}

// Parses tag offset string, properly queues it, and reallocs the OffsetsPtr.
int parse_tagoffset(FILE* ByteStream, char* str, offset_t** OffsetsPtr) {
	OffsetCount++;
	offset_t* temp = realloc(*OffsetsPtr, OffsetCount * sizeof(offset_t));
	if (temp == NULL) {
		printf("Line %i: OffsetsPtr is NULL.", LineNum);
		exit(-1);
	}

	int strsize = strnlen(str, 100) * sizeof(char) + 1;

	temp[OffsetCount - 1].TagOffset = ftell(ByteStream);
	strncpy(temp[OffsetCount - 1].String, str, 100);
	temp[OffsetCount - 1].String[99] = '\0';

	printf("tag %s--------------------------------------------------------------\n", temp[OffsetCount - 1].String);

	*OffsetsPtr = temp;
}

// returns 0 if str is prefix and 1 if it isnt. also sets PrefixControlsArgs.
int prefix_check(char* PrefixPtr, int* PrefixControlsArgs) { 
	if (PrefixPtr == NULL) return 1;

	if (strncmp(PrefixPtr, "rand", 7 * sizeof(char)) == 0) {
		*PrefixControlsArgs = 1;
		return 0;
	}
	else if (strncmp(PrefixPtr, "var", 9 * sizeof(char)) == 0) {
		*PrefixControlsArgs = 1;
		return 0;
	}
	else if (strncmp(PrefixPtr, "if", 3 * sizeof(char)) == 0) {
		return 0;
	}
	else if (strncmp(PrefixPtr, "time", 5 * sizeof(char)) == 0) {
		return 0;
	}
	else if (strncmp(PrefixPtr, "timerand", 9 * sizeof(char)) == 0) {
		return 0;
	}
	else if (strncmp(PrefixPtr, "timevar", 8 * sizeof(char)) == 0) {
		return 0;
	}
	else {
		return 1;
	}
}

// Get the prefix arg strings, convert them into their respective datatypes, and write them to the ByteStream
char* parse_prefix_args(FILE* ByteStream, char* PrefixPtr, char* Token) {
	char* EndPtr;
	if (PrefixPtr == NULL) return Token;

	if (strncmp(PrefixPtr, "rand", 7 * sizeof(char)) == 0) {
		int16_t Int16_1 = byteswap16(strtol(Token, &EndPtr, 10));
		Token = strtok(NULL, DefaultDelimiters);
		int16_t Int16_2 = byteswap16(strtol(Token, &EndPtr, 10));

		fwrite(&Int16_1, sizeof(Int16_1), 1, ByteStream);
		fwrite(&Int16_2, sizeof(Int16_2), 1, ByteStream);
	}
	else if (strncmp(PrefixPtr, "var", 9 * sizeof(char)) == 0) {
		int8_t Int8 = strtol(Token, &EndPtr, 10);

		fputc(Int8, ByteStream);
	}
	else if (strncmp(PrefixPtr, "time", 5 * sizeof(char)) == 0) {
		int16_t Int16 = byteswap16(strtol(Token, &EndPtr, 10));

		fwrite(&Int16, sizeof(Int16), 1, ByteStream);
	}
	else if (strncmp(PrefixPtr, "timerand", 9 * sizeof(char)) == 0) {
		int16_t Int16_1 = byteswap16(strtol(Token, &EndPtr, 10));
		Token = strtok(NULL, DefaultDelimiters);
		int16_t Int16_2 = byteswap16(strtol(Token, &EndPtr, 10));

		fwrite(&Int16_1, sizeof(Int16_1), 1, ByteStream);
		fwrite(&Int16_2, sizeof(Int16_2), 1, ByteStream);
	}
	else if (strncmp(PrefixPtr, "timevar", 8 * sizeof(char)) == 0) {
		int8_t Int8 = strtol(Token, &EndPtr, 10);

		fputc(Int8, ByteStream);
	}

	Token = strtok(NULL, DefaultDelimiters);
	return Token;
}

// Write the prefix strings command bytes to ByteStream.
char* parse_prefix_names(FILE* ByteStream, char* PrefixPtr) {
	if (PrefixPtr == NULL) return 1;

	if (strncmp(PrefixPtr, "rand", 7 * sizeof(char)) == 0) {
		fputc(0xA0, ByteStream);
	}
	else if (strncmp(PrefixPtr, "var", 9 * sizeof(char)) == 0) {
		fputc(0xA1, ByteStream);
	}
	else if (strncmp(PrefixPtr, "if", 3 * sizeof(char)) == 0) {
		fputc(0xA2, ByteStream);
	}
	else if (strncmp(PrefixPtr, "time", 5 * sizeof(char)) == 0) {
		fputc(0xA3, ByteStream);
	}
	else if (strncmp(PrefixPtr, "timerand", 9 * sizeof(char)) == 0) {
		fputc(0xA4, ByteStream);
	}
	else if (strncmp(PrefixPtr, "timevar", 8 * sizeof(char)) == 0) {
		fputc(0xA5, ByteStream);
	}
	else {
		return 1;
	}
}

// Parse note commands, done seperately to keep parse_command() shorter.
char* parse_notecmd(FILE* ByteStream, char* CmdPtr, char** Token, int* PrefixControlsArgs) {
	if (CmdPtr == NULL) return 1;
	char* EndPtr;

	int8_t base = 0;

	if (CmdPtr[0] == 'c' & CmdPtr[0 == 'n']) {
		base = 0x00;
	}
	else if (CmdPtr[0] == 'c' & CmdPtr[1] == 's') {
		base = 0x01;
	}
	else if (CmdPtr[0] == 'd' & CmdPtr[1] == 'n') {
		base = 0x02;
	}
	else if (CmdPtr[0] == 'd' & CmdPtr[1] == 's') {
		base = 0x03;
	}
	else if (CmdPtr[0] == 'e' & CmdPtr[1] == 'n') {
		base = 0x04;
	}
	else if (CmdPtr[0] == 'f' & CmdPtr[1] == 'n') {
		base = 0x05;
	}
	else if (CmdPtr[0] == 'f' & CmdPtr[1] == 's') {
		base = 0x06;
	}
	else if (CmdPtr[0] == 'g' & CmdPtr[1] == 'n') {
		base = 0x07;
	}
	else if (CmdPtr[0] == 'g' & CmdPtr[1] == 's') {
		base = 0x08;
	}
	else if (CmdPtr[0] == 'a' & CmdPtr[1] == 'n') {
		base = 0x09;
	}
	else if (CmdPtr[0] == 'a' & CmdPtr[1] == 's') {
		base = 0x0A;
	}
	else if (CmdPtr[0] == 'b' & CmdPtr[1] == 'n') {
		base = 0x0B;
	}
	else {
		return 1;
	}

	if (CmdPtr[2] != 'm') {
		int8_t mult = CmdPtr[2] - 48; // subtract char by 48 to get its actual int value instead of ASCII.
		base = base + ((mult + 1) * 12);
	}
	fputc(base, ByteStream);

	int8_t Int8 = strtol(*Token, &EndPtr, 10);
	fputc(Int8, ByteStream);
	*Token = strtok(NULL, DefaultDelimiters);

	if (*PrefixControlsArgs == 0) {
		long VLQ = strtol(*Token, &EndPtr, 10);
		parse_vlq(ByteStream, VLQ, 0);
	}
}

// Parse the command strings (duh)
int parse_command(FILE* ByteStream, FILE* TextStream, offset_t** OffsetsPtr, cmdoffset_t** CmdOffsetsPtr) {
	char line[1000];
	fgets(line, sizeof(line), TextStream);
	LineNum++;

	char* PrefixPtrs[3] = { '\0' }; // char** to the prefix strings (can only be up to 3 prefixs, EX. _random_timerand_if is max scope)
	int PrefixCount = 0;
	int PrefixControlsArgs = 0; // 0/1 bool value 1 = true
	
	char* CmdPtr;
	char* EndPtr;

	char* Tokens = strtok(line, DefaultDelimiters);
	CmdPtr = Tokens;
	Tokens = strtok(NULL, DefaultDelimiters);

	if (CmdPtr == NULL & Tokens == NULL) return 1;

	if (strncmp(CmdPtr, "newtrack", 10 * sizeof(char)) == 0) { // Keep recursive if line isnt "/newtrack" (end of newtrack tag).
		return 1;
	}

	printf("%s\n", CmdPtr);

	while (prefix_check(Tokens, &PrefixControlsArgs) != 1){ // Looping until we reach a token which isnt a prefix
		PrefixPtrs[PrefixCount] = Tokens;
		Tokens = strtok(NULL, DefaultDelimiters);
		PrefixCount++;
	}

	for (int i = 1; i <= 3; i++) {
		int index = 3 - i;
		if (PrefixPtrs[index] == NULL) continue;
		parse_prefix_names(ByteStream, PrefixPtrs[index]);
	}



	if (strncmp(CmdPtr, "wait", 5 * sizeof(char)) == 0) { // Start of command else if chain
		fputc(0x80, ByteStream);
		if (PrefixControlsArgs == 0) {
			long VLQ = strtol(Tokens, &EndPtr, 10);
			parse_vlq(ByteStream, VLQ, 0);
		}
	}
	else if (strncmp(CmdPtr, "prg", 4 * sizeof(char)) == 0) {
		fputc(0x81, ByteStream);
		if (PrefixControlsArgs == 0) {
			long VLQ = strtol(Tokens, &EndPtr, 10);
			parse_vlq(ByteStream, VLQ, 0);
		}
	}
	else if (strncmp(CmdPtr, "opentrack", 10 * sizeof(char)) == 0) {
		fputc(0x88, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
			Tokens = strtok(NULL, DefaultDelimiters);
			parse_cmdoffset(ByteStream, Tokens, CmdOffsetsPtr);
		}
	}
	else if (strncmp(CmdPtr, "jump", 5 * sizeof(char)) == 0) {
		fputc(0x89, ByteStream);
		if (PrefixControlsArgs == 0) {
			parse_cmdoffset(ByteStream, Tokens, CmdOffsetsPtr);
		}
	}
	else if (strncmp(CmdPtr, "call", 5 * sizeof(char)) == 0) {
		fputc(0x8A, ByteStream);
		if (PrefixControlsArgs == 0) {
			parse_cmdoffset(ByteStream, Tokens, CmdOffsetsPtr);
		}
	}
	else if (strncmp(CmdPtr, "timebase", 9 * sizeof(char)) == 0) {
		fputc(0xB0, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "envhold", 8 * sizeof(char)) == 0) {
		fputc(0xB1, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "monophonic_bool", 11 * sizeof(char)) == 0) {
		fputc(0xB2, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "velocityrange", 14 * sizeof(char)) == 0) {
		fputc(0xB3, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "pan", 3 * sizeof(char)) == 0) {
		fputc(0xC0, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "volume", 7 * sizeof(char)) == 0) {
		fputc(0xC1, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "mainvolume", 11 * sizeof(char)) == 0) {
		fputc(0xC2, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "transpose", 10 * sizeof(char)) == 0) {
		fputc(0xC3, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "pitchbend", 10 * sizeof(char)) == 0) {
		fputc(0xC4, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "bendrange", 10 * sizeof(char)) == 0) {
		fputc(0xC5, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "prio", 5 * sizeof(char)) == 0) {
		fputc(0xC6, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "notewait_bool", 9 * sizeof(char)) == 0) {
		fputc(0xC7, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "tie_bool", 4 * sizeof(char)) == 0) {
		fputc(0xC8, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "prtmnto", 8 * sizeof(char)) == 0) {
		fputc(0xC9, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "moddepth", 9 * sizeof(char)) == 0) {
		fputc(0xCA, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "modspeed", 9 * sizeof(char)) == 0) {
		fputc(0xCB, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "modtype", 8 * sizeof(char)) == 0) {
		fputc(0xCC, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "modrange", 9 * sizeof(char)) == 0) {
		fputc(0xCD, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "prtmnto_bool", 8 * sizeof(char)) == 0) {
		fputc(0xCE, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "prtmntotime", 12 * sizeof(char)) == 0) {
		fputc(0xCF, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "attack", 7 * sizeof(char)) == 0) {
		fputc(0xD0, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "decay", 6 * sizeof(char)) == 0) {
		fputc(0xD1, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "sustain", 8 * sizeof(char)) == 0) {
		fputc(0xD2, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "release", 8 * sizeof(char)) == 0) {
		fputc(0xD3, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "loopstart", 8 * sizeof(char)) == 0) {
		fputc(0xD4, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "volume2", 8 * sizeof(char)) == 0) {
		fputc(0xD5, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "printvar", 9 * sizeof(char)) == 0) {
		fputc(0xD6, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "srndpan", 8 * sizeof(char)) == 0) {
		fputc(0xD7, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "lpfcutoff", 10 * sizeof(char)) == 0) {
		fputc(0xD8, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "fxsenda", 8 * sizeof(char)) == 0) {
		fputc(0xD9, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "fxsendb", 8 * sizeof(char)) == 0) {
		fputc(0xDA, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "mainsend", 9 * sizeof(char)) == 0) {
		fputc(0xDB, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "initpan", 8 * sizeof(char)) == 0) {
		fputc(0xDC, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "mute", 5 * sizeof(char)) == 0) {
		fputc(0xDD, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "fxsendc", 8 * sizeof(char)) == 0) {
		fputc(0xDE, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "damper", 7 * sizeof(char)) == 0) {
		fputc(0xDF, ByteStream);
		if (PrefixControlsArgs == 0) {
			int8_t Int8 = strtol(Tokens, &EndPtr, 10);
			fputc(Int8, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "moddelay", 9 * sizeof(char)) == 0) {
		fputc(0xE0, ByteStream);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "tempo", 6 * sizeof(char)) == 0) {
		fputc(0xE1, ByteStream);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "sweeppitch", 11 * sizeof(char)) == 0) {
		fputc(0xE3, ByteStream);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "setvar", 7 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x80, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "addvar", 7 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x81, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "subvar", 7 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x82, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "mulvar", 7 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x83, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "divvar", 7 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x84, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "binshiftvar", 12 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x85, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "randvar", 8 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x86, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "binandvar", 10 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x87, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "binorvar", 9 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x87, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "binxorvar", 10 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x89, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "notvar", 7 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x8A, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "modvar", 7 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x8B, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "cmp_eq", 7 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x90, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "cmp_ge", 7 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x91, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "cmp_gt", 7 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x92, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "cmp_le", 7 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x93, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "cmp_lt", 7 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x94, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "cmp_ne", 7 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0x95, ByteStream);
		int8_t Int8 = strtol(Tokens, &EndPtr, 10);
		fputc(Int8, ByteStream);
		Tokens = strtok(NULL, DefaultDelimiters);
		if (PrefixControlsArgs == 0) {
			int16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "userproc", 9 * sizeof(char)) == 0) {
		fputc(0xF0, ByteStream);
		fputc(0xE0, ByteStream);

		uint16_t Int16 = byteswap16(strtol(Tokens, &EndPtr, 10));
		fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
	}
	else if (strncmp(CmdPtr, "loopend", 8 * sizeof(char)) == 0) {
		fputc(0xFC, ByteStream);
	}
	else if (strncmp(CmdPtr, "return", 7 * sizeof(char)) == 0) {
		fputc(0xFD, ByteStream);
	}
	else if (strncmp(CmdPtr, "alloctrack", 11 * sizeof(char)) == 0) {
		fputc(0xFE, ByteStream);
		if (PrefixControlsArgs == 0) { // Anything -48 is because ASCII numbers start at 48. EX. 0x31 represents 1 in ASCII and is 49 in decimal. 49-48 = 1.
			if (Tokens[15]-48 != 0 & Tokens[15]-48 != 1) {
				printf("Line %i: alloctrack argument must be a 16 bit binary number. EX. 0000000000000001", LineNum);
				exit(0);
			}

			int16_t Int16 = 0;

			for (int i = 0; i < 16; i++) {
				Int16 = Int16 | (Tokens[i] - 48);
				if (i < 15) Int16 = Int16 << 1;
			}
			Int16 = byteswap16(Int16);
			fwrite(&Int16, sizeof(int16_t), 1, ByteStream);
		}
	}
	else if (strncmp(CmdPtr, "fin", 4 * sizeof(char)) == 0) {
		fputc(0xFF, ByteStream);
	}
	else if (CmdPtr[1] == 'n' | CmdPtr[1] == 's') {
		parse_notecmd(ByteStream, CmdPtr, &Tokens, &PrefixControlsArgs);
	}
	else if (strncmp(CmdPtr, "offset", 7 * sizeof(char)) == 0) {
		Tokens = strtok(NULL, DefaultDelimiters);
		parse_tagoffset(ByteStream, Tokens, OffsetsPtr);
	}

	if (PrefixControlsArgs == 0) {
		Tokens = strtok(NULL, DefaultDelimiters);
	}

	for (int i = 0; i < 3; i++) {
		if (PrefixPtrs[i] == NULL) continue;
		Tokens = parse_prefix_args(ByteStream, PrefixPtrs[i], Tokens);
	}

	return 0;
}


char* parse_tag_descriptor(char* Token, FILE* TextStream, label_t* LabelsPtr) {
	if (strncmp(Token, "name", 5 * sizeof(char)) == 0) {
		label_t* temp = LabelsPtr;
		if (temp == NULL) {
			printf("Line %i: LabelsPtr is NULL.", LineNum);
			exit(-1);
		}
		printf("Descriptor: %s\n", Token);
		Token = strtok(NULL, " <>\"=");
		strncpy(temp[LabelsParsedCount - 1].String, Token, strnlen(Token, 99) + 1);
		temp[LabelsParsedCount - 1].StringLen = strnlen(Token,100);
		printf("Value: %s\n", Token);
		LabelsPtr = temp;
	}
}

char* parse_tag(char* Token, FILE* ByteStream, FILE* TextStream, offset_t** OffsetsPtr, cmdoffset_t** CmdOffsetsPtr, label_t** LabelsPtr, brseq_t BRSEQ){
	printf("Tag: %s ----------------------------------\n", Token);

	if (strncmp(Token, "newtrack", 9 * sizeof(char)) == 0) {
		LabelsParsedCount++;
		label_t* temp = realloc(*LabelsPtr, LabelsParsedCount * sizeof(label_t));
		int endloop = 0;

		temp[LabelsParsedCount - 1].SndDATA_Offset = ftell(ByteStream) - (BRSEQ.DATA_Offset + 0x0C);

		Token = strtok(NULL, " <>\"=");
		parse_tag_descriptor(Token, TextStream, temp);
		while (endloop == 0) {
			endloop = parse_command(ByteStream, TextStream, OffsetsPtr, CmdOffsetsPtr);
		}
		
		*LabelsPtr = temp;
	}
}

char* parse_textstream(FILE* TextStream, FILE* ByteStream, brseq_t BRSEQ) {
	char line[1000];
	offset_t* OffsetsPtr = calloc(1, sizeof(offset_t));
	cmdoffset_t* CmdOffsetsPtr = calloc(1, sizeof(cmdoffset_t));
	label_t* LabelsPtr = calloc(1, sizeof(label_t));

	// Beginning of writing RSEQ data.

	fwrite(&BRSEQ.RSEQHeaderStr, 4 * sizeof(char), 1, ByteStream);
	fwrite(&BRSEQ.EndianBytes, 2 * sizeof(char), 1, ByteStream);
	fwrite(&BRSEQ.VersionNum, 1 * sizeof(int16_t), 1, ByteStream);
	fwrite(&BRSEQ.FileLength, 1 * sizeof(int32_t), 1, ByteStream);
	fwrite(&BRSEQ.RSEQ_Size, 1 * sizeof(int16_t), 1, ByteStream);
	fwrite(&BRSEQ.NumberOfSections, 1 * sizeof(int16_t), 1, ByteStream);
	fwrite(&BRSEQ.DATA_Offset, 1 * sizeof(int32_t), 1, ByteStream);
	fwrite(&BRSEQ.DATA_Size, 1 * sizeof(int32_t), 1, ByteStream);
	fwrite(&BRSEQ.LABL_Offset, 1 * sizeof(int32_t), 1, ByteStream);
	fwrite(&BRSEQ.LABL_Size, 1 * sizeof(int32_t), 1, ByteStream);

	// Beginning of writing DATA data.

	BRSEQ.DATA_Offset = ftell(ByteStream);
	fwrite("DATA", 4 * sizeof(char), 1, ByteStream);
	fwrite(&BRSEQ.DATA_Size, 1 * sizeof(int32_t), 1, ByteStream);
	fwrite(&BRSEQ.DATAStruct.DATA_Offset, 1 * sizeof(int32_t), 1, ByteStream);

	while (fgets(line, sizeof(line), TextStream))
	{
		LineNum++;
		char* Tokens = strtok(line, " <>\"=");

		if (line[0] == '<') {
			parse_tag(Tokens, ByteStream, TextStream, &OffsetsPtr, &CmdOffsetsPtr, &LabelsPtr, BRSEQ);
		}
	}

	for (int i = 0; i < CmdOffsetCount; i++) {
		if (CmdOffsetsPtr == NULL) break;
		if (OffsetsPtr == NULL) break;
		if (CmdOffsetCount == 0) break;
		if (CmdOffsetCount == 0) break;
		for (int i2 = 0; i2 < OffsetCount; i2++) {
			if (strncmp(CmdOffsetsPtr[i].String, OffsetsPtr[i2].String, 100) == 0) {
				printf("%s %s\n", CmdOffsetsPtr[i].String, OffsetsPtr[i2].String);
				fseek(ByteStream, CmdOffsetsPtr[i].CmdValueOffset, SEEK_SET);
				parse_uint24(ByteStream, OffsetsPtr[i2].TagOffset - (BRSEQ.DATA_Offset + 0x0C));
			}
		}
	}

	fseek(ByteStream, -1, SEEK_END);
	if (fgetc(ByteStream) != 0xFF) {
		fseek(ByteStream, 0, SEEK_END);
		fputc(0xFF, ByteStream);
	}
	fseek(ByteStream, 0, SEEK_END);
	printf("%x\n", fgetc(ByteStream));
	fputc(0xFF, ByteStream);

	int IncompleteByteRowCount = ftell(ByteStream) % 16;
	for (int i = 1; i <= 16 - IncompleteByteRowCount; i++) { // Fill in the end of the DATA section with 0x00 until a full row of 16 bytes is formed.
		fputc(0x00, ByteStream);
	}

	BRSEQ.DATA_Size = ftell(ByteStream) - BRSEQ.DATA_Offset;

	// Beginning of writing LABL data.

	BRSEQ.LABL_Offset = ftell(ByteStream);

	LabelsParsedCount = byteswap32(LabelsParsedCount);
	fwrite("LABL", 4 * sizeof(char), 1, ByteStream);
	fwrite(&BRSEQ.LABL_Size, 1 * sizeof(int32_t), 1, ByteStream);
	fwrite(&LabelsParsedCount, 1 * sizeof(int32_t), 1, ByteStream);
	LabelsParsedCount = byteswap32(LabelsParsedCount);

	for (int i = 0; i < LabelsParsedCount; i++) {
		fputc(0x00, ByteStream);
		fputc(0x00, ByteStream);
		fputc(0x00, ByteStream);
		fputc(0x00, ByteStream);
	}

	for (int i = 0; i < LabelsParsedCount; i++) {
		LabelsPtr[i].LabelOffset = ftell(ByteStream) - (BRSEQ.LABL_Offset + 0x08);

		LabelsPtr[i].SndDATA_Offset = byteswap32(LabelsPtr[i].SndDATA_Offset);
		LabelsPtr[i].StringLen = byteswap32(LabelsPtr[i].StringLen);
		LabelsPtr[i].LabelOffset = byteswap32(LabelsPtr[i].LabelOffset);
		fwrite(&LabelsPtr[i].SndDATA_Offset, sizeof(int32_t), 1, ByteStream);
		fwrite(&LabelsPtr[i].StringLen, sizeof(int32_t), 1, ByteStream);
		fseek(ByteStream, (BRSEQ.LABL_Offset + 0x0C) + (i * 4), SEEK_SET);
		fwrite(&LabelsPtr[i].LabelOffset, sizeof(int32_t), 1, ByteStream);
		fseek(ByteStream, 0, SEEK_END);
		LabelsPtr[i].SndDATA_Offset = byteswap32(LabelsPtr[i].SndDATA_Offset);
		LabelsPtr[i].StringLen = byteswap32(LabelsPtr[i].StringLen);
		LabelsPtr[i].LabelOffset = byteswap32(LabelsPtr[i].LabelOffset);

		fwrite(LabelsPtr[i].String, strlen(LabelsPtr[i].String) * sizeof(char), 1, ByteStream);
	}

	fseek(ByteStream, 0, SEEK_END);

	IncompleteByteRowCount = ftell(ByteStream) % 16;
	for (int i = 1; i <= 16 - IncompleteByteRowCount; i++) { // Fill in the end of the DATA section with 0x00 until a full row of 16 bytes is formed.
		fputc(0x00, ByteStream);
	}

	BRSEQ.LABL_Size = ftell(ByteStream) - BRSEQ.LABL_Offset;

	// End of writing new data to sections.
	// Beginning of modifying wrong fields of sections.

	uint32_t FileLength = byteswap32(ftell(ByteStream));
	BRSEQ.DATA_Size = byteswap32(BRSEQ.DATA_Size);
	BRSEQ.LABL_Offset = byteswap32(BRSEQ.LABL_Offset);
	BRSEQ.LABL_Size = byteswap32(BRSEQ.LABL_Size);

	fseek(ByteStream, 0x08, SEEK_SET);
	fwrite(&FileLength, sizeof(int32_t), 1, ByteStream);

	fseek(ByteStream, 0x14, SEEK_SET);
	fwrite(&BRSEQ.DATA_Size, sizeof(int32_t), 1, ByteStream);

	fseek(ByteStream, BRSEQ.DATA_Offset + 0x04, SEEK_SET);
	fwrite(&BRSEQ.DATA_Size, sizeof(int32_t), 1, ByteStream);

	fseek(ByteStream, 0x18, SEEK_SET);
	fwrite(&BRSEQ.LABL_Offset, sizeof(int32_t), 1, ByteStream);
	BRSEQ.LABL_Offset = byteswap32(BRSEQ.LABL_Offset);

	fseek(ByteStream, 0x1C, SEEK_SET);
	fwrite(&BRSEQ.LABL_Size, sizeof(int32_t), 1, ByteStream);

	fseek(ByteStream, BRSEQ.LABL_Offset + 0x04, SEEK_SET);
	fwrite(&BRSEQ.LABL_Size, sizeof(int32_t), 1, ByteStream);


	free(CmdOffsetsPtr);
	free(OffsetsPtr);
	free(LabelsPtr);
}