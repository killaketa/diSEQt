#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "brseq.h"

#define CMDNAMEBUFFERSIZE 100
#define CMDARGSBUFFERSIZE 100

// Anything bitwise with int is decoding some numbers. They're decoded into little endian from whatever amount of bytes sorted in big endian.

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


char* decode_notebyte(unsigned char NoteByte, char* NoteNameBuffer) {
	char notename[6] = { "\0" };
	int NoteDivided = (int)floor((__int8)NoteByte / 12);

	switch ((__int8)NoteByte % 12) 
	{
	default:
		notename[0] = 'x';
		break;
	case(0):
		notename[0] = 'c';
		notename[1] = 'n';
		break;
	case(1):
		notename[0] = 'c';
		notename[1] = 's';
		break;
	case(2):
		notename[0] = 'd';
		notename[1] = 'n';
		break;
	case(3):
		notename[0] = 'd';
		notename[1] = 's';
		break;
	case(4):
		notename[0] = 'e';
		notename[1] = 'n';
		break;
	case(5):
		notename[0] = 'f';
		notename[1] = 'n';
		break;
	case(6):
		notename[0] = 'f';
		notename[1] = 's';
		break;
	case(7):
		notename[0] = 'g';
		notename[1] = 'n';
		break;
	case(8):
		notename[0] = 'g';
		notename[1] = 's';
		break;
	case(9):
		notename[0] = 'a';
		notename[1] = 'n';
		break;
	case(10):
		notename[0] = 'a';
		notename[1] = 's';
		break;
	case(11):
		notename[0] = 'b';
		notename[1] = 'n';
		break;
	}


	char numchar[] = "0123456789";
	if ((int)floor(NoteDivided) > 0) {
		notename[2] = numchar[NoteDivided - 1];
	}
	else
	{
		notename[2] = 'm';
		notename[3] = '1';
	}

	sprintf_s(NoteNameBuffer, 6 * sizeof(char), "%s", notename);
}


char decode_extended_command(brseq_t BRSEQ, int* Offset, char* NameBuffer, size_t NameBufferSize, char* ArgsBuffer, size_t ArgsBufferSize, int PrefixDeterminesArgs, int* BytesAdded) {
	unsigned char Byte = "\0";
	unsigned __int16 UInt16 = 0;
	__int16 Int16 = 0;

	switch (BRSEQ.DATAStruct.DATA_Section[*Offset])
	{
	case 0x80:
		sprintf_s(NameBuffer, NameBufferSize, "setvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x81:
		sprintf_s(NameBuffer, NameBufferSize, "addvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x82:
		sprintf_s(NameBuffer, NameBufferSize, "subvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x83:
		sprintf_s(NameBuffer, NameBufferSize, "mulvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x84:
		sprintf_s(NameBuffer, NameBufferSize, "divvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x85:
		sprintf_s(NameBuffer, NameBufferSize, "binshiftvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x86:
		sprintf_s(NameBuffer, NameBufferSize, "randvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x87:
		sprintf_s(NameBuffer, NameBufferSize, "binandvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x88:
		sprintf_s(NameBuffer, NameBufferSize, "binorvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x89:
		sprintf_s(NameBuffer, NameBufferSize, "binxorvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x8A:
		sprintf_s(NameBuffer, NameBufferSize, "notvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x8B:
		sprintf_s(NameBuffer, NameBufferSize, "modvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x90:
		sprintf_s(NameBuffer, NameBufferSize, "cmp_eq%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x91:
		sprintf_s(NameBuffer, NameBufferSize, "cmp_ge%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x92:
		sprintf_s(NameBuffer, NameBufferSize, "cmp_gt%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x93:
		sprintf_s(NameBuffer, NameBufferSize, "cmp_le%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x94:
		sprintf_s(NameBuffer, NameBufferSize, "cmp_lt%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x95:
		sprintf_s(NameBuffer, NameBufferSize, "cmp_ne%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0xE0:
		sprintf_s(NameBuffer, NameBufferSize, "userproc%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			UInt16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			sprintf_s(ArgsBuffer, ArgsBufferSize, " %u,%%s", UInt16);
		}
		break;
	}
}


char decode_prefix(brseq_t BRSEQ, int* Offset, char** PrefixStr, char** PrefixArgsStr, int* PrefixDeterminesArgs, int* CmdOffset, int* BytesAdded) {
	int PrefixCmdOffset = *CmdOffset;
	unsigned char PrefixCmdByte = BRSEQ.DATAStruct.DATA_Section[PrefixCmdOffset];
	unsigned char Byte[4] = "\0";
	__int16 Int16[2] = { 0 };
	char prefixname[CMDNAMEBUFFERSIZE] = { "\0" };
	char prefixargs[CMDARGSBUFFERSIZE] = { "\0" };

	char* PrefixName2;
	char* PrefixArgs2;
	int NextCmdOffset = PrefixCmdOffset + 1;
	unsigned char NextByte = BRSEQ.DATAStruct.DATA_Section[NextCmdOffset];
	if ((NextByte == 0xA0 | NextByte == 0xA1 | NextByte == 0xA2 | NextByte == 0xA3 | NextByte == 0xA4 | NextByte == 0xA5) &&
		(PrefixCmdByte == 0xA0 | PrefixCmdByte == 0xA1 | PrefixCmdByte == 0xA2 | PrefixCmdByte == 0xA3 | PrefixCmdByte == 0xA4 | PrefixCmdByte == 0xA5)) {
		*Offset += 1;
		decode_prefix(BRSEQ, Offset, &PrefixName2, &PrefixArgs2, PrefixDeterminesArgs, &NextCmdOffset, BytesAdded);
		*CmdOffset = NextCmdOffset;
		*Offset = *CmdOffset + *BytesAdded;
	}
	else
	{
		*CmdOffset = NextCmdOffset;
		*Offset = *CmdOffset + *BytesAdded;
		PrefixName2 = calloc(1, sizeof(char));
		PrefixArgs2 = calloc(1, sizeof(char));
		sprintf_s(PrefixName2, 1, "\0");
		sprintf_s(PrefixArgs2, 1, "\0");
	}

	switch (BRSEQ.DATAStruct.DATA_Section[PrefixCmdOffset])
	{
	default:
		*Offset -= 1;
		*CmdOffset -= 1;
		break;
	case 0xA0:
		*Offset += 1;
		Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
		*Offset += 1;
		Byte[1] = BRSEQ.DATAStruct.DATA_Section[*Offset];
		*Offset += 1;
		Byte[2] = BRSEQ.DATAStruct.DATA_Section[*Offset];
		*Offset += 1;
		Byte[3] = BRSEQ.DATAStruct.DATA_Section[*Offset];
		*BytesAdded += 4;
		sprintf_s(prefixname, CMDNAMEBUFFERSIZE,"%%s::rand\0"); // %i %i // randomcmdbyte commandbyte commandargs(...) randMin(Byte[2]) randMax(Byte[2])
		sprintf_s(prefixargs, CMDARGSBUFFERSIZE, "%%s %i, %i", (__int16)((Byte[0] << 8) | Byte[1]), (__int16)((Byte[2] << 8) | Byte[3]));
		break;
	case 0xA1:
		*Offset += 1;
		Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
		*BytesAdded += 1;
		sprintf_s(prefixname, CMDNAMEBUFFERSIZE, "%%s::var\0"); // %u // varcmdbyte commandbyte commandargs(...) variable(Byte)
		sprintf_s(prefixargs, CMDARGSBUFFERSIZE, "%%s %u", Byte[0]);
		break;;
	case 0xA2:
		sprintf_s(prefixname, CMDNAMEBUFFERSIZE, "%%s::if\0");
		sprintf_s(prefixargs, CMDARGSBUFFERSIZE, "%%s\0");
		break;
	case 0xA3: // PCommandByte (FollowingCommand) TimeFactorInt16
		*Offset += 1;
		Int16[0] = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
		*Offset += 1;
		*BytesAdded += 2;
		sprintf_s(prefixname, CMDNAMEBUFFERSIZE, "%%s::time\0"); // %i // randomcmdbyte commandbyte commandargs(...) time(Int16)
		sprintf_s(prefixargs, CMDARGSBUFFERSIZE, "%%s, %i", Int16[0]);
		break;
	case 0xA4:
		*Offset += 1;
		Int16[0] = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
		*Offset += 2;
		Int16[1] = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
		*Offset += 1;
		*BytesAdded += 4;
		sprintf_s(prefixname, CMDNAMEBUFFERSIZE, "%%s::timerand\0"); // %i %i // randomcmdbyte commandbyte commandargs(...) timeMin(Int16) randMax(Int16)
		sprintf_s(prefixargs, CMDARGSBUFFERSIZE, "%%s, %i, %i", Int16[0], Int16[1]);
		break;
	case 0xA5:
		*Offset += 1;
		Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
		*BytesAdded += 1;
		sprintf_s(prefixname, CMDNAMEBUFFERSIZE, "%%s::timevar\0"); // %u // randomcmdbyte commandbyte commandargs(...) timeVariable(Byte)
		sprintf_s(prefixargs, CMDARGSBUFFERSIZE, "%%s, %u", Byte[0]);
		break;
	}

	if (*PrefixDeterminesArgs != 1) {
		*PrefixDeterminesArgs = 0;
	}

	char strbuffer[CMDNAMEBUFFERSIZE] = { "\0" };
	char argsbuffer[CMDARGSBUFFERSIZE] = "%s";
	sprintf_s(strbuffer, CMDNAMEBUFFERSIZE, "%s", prefixname);
	if (PrefixDeterminesArgs == 0) {
		sprintf_s(prefixargs, CMDARGSBUFFERSIZE, prefixargs, "%s,");
	}
	else {
		sprintf_s(prefixargs, CMDARGSBUFFERSIZE, prefixargs, "%s");
	}
	sprintf_s(argsbuffer, CMDARGSBUFFERSIZE, "%s", prefixargs);

	if (strcmp(prefixname, "\0") != 0) {
		sprintf_s(strbuffer, CMDNAMEBUFFERSIZE, prefixname, PrefixName2);
		sprintf_s(argsbuffer, CMDARGSBUFFERSIZE, prefixargs, PrefixArgs2);
	}

	free(PrefixName2);
	free(PrefixArgs2);

	size_t NameBufferSize = snprintf(NULL, 0, "%s", strbuffer) + 1;
	size_t ArgsBufferSize = snprintf(NULL, 0, "%s", argsbuffer) + 1;

	*PrefixStr = calloc(NameBufferSize, sizeof(char));
	*PrefixArgsStr = calloc(ArgsBufferSize, sizeof(char));
	sprintf_s(*PrefixStr, NameBufferSize, "%s", strbuffer);
	sprintf_s(*PrefixArgsStr, ArgsBufferSize, "%s", argsbuffer);
}


char prefix_determinesargs(brseq_t BRSEQ, int* Offset, int* PrefixDeterminesArgs, int* CmdOffset, int* BytesAdded) {
	int PrefixCmdOffset = *CmdOffset;
	unsigned char PrefixCmdByte = BRSEQ.DATAStruct.DATA_Section[PrefixCmdOffset];

	int NextCmdOffset = PrefixCmdOffset + 1;
	unsigned char NextByte = BRSEQ.DATAStruct.DATA_Section[NextCmdOffset];
	if ((NextByte == 0xA0 | NextByte == 0xA1 | NextByte == 0xA2 | NextByte == 0xA3 | NextByte == 0xA4 | NextByte == 0xA5) &&
		(PrefixCmdByte == 0xA0 | PrefixCmdByte == 0xA1 | PrefixCmdByte == 0xA2 | PrefixCmdByte == 0xA3 | PrefixCmdByte == 0xA4 | PrefixCmdByte == 0xA5)) {
		*Offset += 1;
		prefix_determinesargs(BRSEQ, Offset, PrefixDeterminesArgs, &NextCmdOffset, BytesAdded);
		*CmdOffset = NextCmdOffset;
	}
	else
	{
		*Offset = NextCmdOffset;
		*CmdOffset = NextCmdOffset;
	}


	switch (BRSEQ.DATAStruct.DATA_Section[PrefixCmdOffset])
	{
	default:
		*Offset -= 1;
		*CmdOffset -= 1;
		break;
	case 0xA0:
		*PrefixDeterminesArgs = 1;
		break;
	case 0xA1:
		*PrefixDeterminesArgs = 1;
		break;;
	case 0xA2:
		break;
	case 0xA3:
		break;
	case 0xA4:
		break;
	case 0xA5:
		break;
	}
}


char* decode_command(brseq_t BRSEQ, int* Offset, char** StrBuffer, int* ImportantOffsets) {
	int FirstPrefixCmdOffset = *Offset;
	int CmdOffset = *Offset;
	unsigned char Byte[2] = { "\0" };
	__int16 Int16 = "\0";
	unsigned __int16 UInt16 = "\0";
	unsigned __int32 UInt24 = "\0";
	unsigned long VLQ = "\0";

	char CmdName[CMDNAMEBUFFERSIZE] = { "\0" };
	char CmdArgs[CMDARGSBUFFERSIZE] = "%s";

	int PrefixDeterminesArgs = 0;
	int BytesPassed = 0;

	prefix_determinesargs(BRSEQ, Offset, &PrefixDeterminesArgs, &CmdOffset, &BytesPassed);

	switch (BRSEQ.DATAStruct.DATA_Section[CmdOffset])
	{
	default: // If byte is undefined command then its a note command
		Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
		char NoteName[6] = { "\0" };
		decode_notebyte(Byte[0], NoteName);
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "%s%%s%%s", NoteName);

		BytesPassed = *Offset;
		*Offset += 1;
		Byte[1] = BRSEQ.DATAStruct.DATA_Section[*Offset];
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			VLQ = decode_vlq(BRSEQ.DATAStruct.DATA_Section, Offset);
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u, %u%%s", Byte[1], VLQ);
		}
		else{
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u,%%s", Byte[1]);
		}
		BytesPassed = *Offset - BytesPassed;
		break;
	case 0x80:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "wait%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			BytesPassed = *Offset;
			*Offset += 1;
			VLQ = decode_vlq(BRSEQ.DATAStruct.DATA_Section, Offset);
			BytesPassed = *Offset - BytesPassed;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", VLQ);
		}
		break;
	case 0x81:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "prg%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			BytesPassed = *Offset;
			*Offset += 1;
			VLQ = decode_vlq(BRSEQ.DATAStruct.DATA_Section, Offset);
			BytesPassed = *Offset - BytesPassed;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", VLQ);
		}
		break;
	case 0x88:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "opentrack%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			*Offset += 1;
			UInt24 = (((BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1]) << 8) | (BRSEQ.DATAStruct.DATA_Section[*Offset + 2]);
			*Offset += 2;
			ImportantOffsets[UInt24 + 0x0C] = 1;
			BytesPassed += 4;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u, %x%%s", Byte[0], UInt24);
		}
		break;
	case 0x89:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "jump%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			UInt24 = (((BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1]) << 8) | (BRSEQ.DATAStruct.DATA_Section[*Offset + 2]);
			*Offset += 2;
			ImportantOffsets[UInt24 + 0x0C] = 1;
			BytesPassed += 3;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %x%%s", UInt24);
		}
		break;
	case 0x8A:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "call%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			UInt24 = (((BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1]) << 8) | (BRSEQ.DATAStruct.DATA_Section[*Offset + 2]);
			*Offset += 2;
			ImportantOffsets[UInt24 + 0x0C] = 1;
			BytesPassed += 3;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %x%%s", UInt24);
		}
		break;
	case 0xB0:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "timebase%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xB1:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "envhold%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xB2:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "monophonic_bool%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xB3:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "velocityrange%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC0:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "pan%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC1:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "volume%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC2:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "mainvolume%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC3:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "transpose%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC4:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "pitchbend%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC5:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "bendrange%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC6:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "prio%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC7:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "notewait_bool%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC8:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "tie_bool%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC9:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "prtmnto%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xCA:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "moddepth%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xCB:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "modspeed%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xCC:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "modtype%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xCD:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "modrange%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xCE:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "prtmnto_bool%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xCF:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "prtmntotime%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD0:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "attack%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD1:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "decay%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD2:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "sustain%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD3:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "release%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD4:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "loopstart%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD5:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "volume2%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD6:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "printvar%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD7:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "srndpan%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD8:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "lpfcutoff%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD9:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "fxsenda%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xDA:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "fxsendb%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xDB:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "mainsend%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xDC:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "initpan%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xDD:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "mute%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xDE:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "fxsendc%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xDF:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "damper%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xE0:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "moddelay%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			UInt16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			BytesPassed += 2;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", UInt16);
		}
		break;
	case 0xE1:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "tempo%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			BytesPassed += 2;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Int16);
		}
		break;
	case 0xE3:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "sweeppitch%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			BytesPassed += 2;
			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Int16);
			}
		break;
	case 0xF0:
		*Offset += 1;
		CmdOffset += 1;
		BytesPassed += 1;
		decode_extended_command(BRSEQ, Offset, CmdName, CMDNAMEBUFFERSIZE, CmdArgs, CMDARGSBUFFERSIZE, PrefixDeterminesArgs, &BytesPassed);
		break; // 1, 1, Int16; // Follows F0 AA BB CC CC in hex, with AA being the Extended Command Byte (see decode_extcommand function), BB being a Byte, and CC CC being an Int16 value.
	case 0xFC:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "loopend%%s");
		break;
	case 0xFD:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "return%%s");
		break;
	case 0xFE:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "alloctrack%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1; // The binary of the Int16 is used as a Bitmask to allocate tracks. (EX. 0000111111111111 would allocate tracks 0-3, note that track 0 is always allocated no matter what its bit is set to.)
			BytesPassed += 2;
			char bits[17] = { '\0' };
			for (int i = 0; i < 16; i++) {
				if ((0x8000 & (Int16 << i)) == 0) {
					bits[i] = '0';
				}
				else
				{
					bits[i] = '1';
				}
			}

			sprintf_s(CmdArgs, CMDARGSBUFFERSIZE, " %s%%s", bits);
		}
		break;
	case 0xFF:
		sprintf_s(CmdName, CMDNAMEBUFFERSIZE, "fin%%s");
		break;
	}

	char* prefixstr;
	char* prefixargs;
	decode_prefix(BRSEQ, Offset, &prefixstr, &prefixargs, &PrefixDeterminesArgs, &FirstPrefixCmdOffset, &BytesPassed);

	char FormattedCmdArgs[CMDARGSBUFFERSIZE] = { "\0" };

	sprintf_s(FormattedCmdArgs, CMDARGSBUFFERSIZE, CmdArgs, prefixargs);

	int NeededBufferSize = snprintf(NULL, 0, CmdName, prefixstr, FormattedCmdArgs) + sizeof(char);
	*StrBuffer = calloc(NeededBufferSize, sizeof(char));
	if (*StrBuffer == 0) {
		printf("Command StrBuffer calloc() failed. (decode_command)\n");
		exit(-1);
	}

	sprintf_s(*StrBuffer, NeededBufferSize, CmdName, prefixstr, FormattedCmdArgs);

	free(prefixstr);
	free(prefixargs);
}