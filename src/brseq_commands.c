#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "brseq.h"

#define CMDNAMEBUFFERSIZE 100
#define CMDARGSBUFFERSIZE 100


char* decode_notebyte(unsigned char NoteByte, char* NoteNameBuffer) {
	char notename[6] = { "\0" };
	int NoteDivided = (int)floor((int8_t)NoteByte / 12);

	switch ((int8_t)NoteByte % 12) 
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

	snprintf(NoteNameBuffer, 6 * sizeof(char), "%s", notename);
}


char decode_extended_command(brseq_t BRSEQ, int* Offset, char* NameBuffer, size_t NameBufferSize, char* ArgsBuffer, size_t ArgsBufferSize, int PrefixDeterminesArgs, int* BytesAdded) {
	unsigned char Byte = "\0";
	uint16_t UInt16 = 0;
	int16_t Int16 = 0;

	switch (BRSEQ.DATAStruct.DATA_Section[*Offset])
	{
	case 0x80:
		snprintf(NameBuffer, NameBufferSize, "setvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x81:
		snprintf(NameBuffer, NameBufferSize, "addvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x82:
		snprintf(NameBuffer, NameBufferSize, "subvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x83:
		snprintf(NameBuffer, NameBufferSize, "mulvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x84:
		snprintf(NameBuffer, NameBufferSize, "divvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x85:
		snprintf(NameBuffer, NameBufferSize, "binshiftvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x86:
		snprintf(NameBuffer, NameBufferSize, "randvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x87:
		snprintf(NameBuffer, NameBufferSize, "binandvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x88:
		snprintf(NameBuffer, NameBufferSize, "binorvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x89:
		snprintf(NameBuffer, NameBufferSize, "binxorvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x8A:
		snprintf(NameBuffer, NameBufferSize, "notvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x8B:
		snprintf(NameBuffer, NameBufferSize, "modvar%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x90:
		snprintf(NameBuffer, NameBufferSize, "cmp_eq%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x91:
		snprintf(NameBuffer, NameBufferSize, "cmp_ge%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x92:
		snprintf(NameBuffer, NameBufferSize, "cmp_gt%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x93:
		snprintf(NameBuffer, NameBufferSize, "cmp_le%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x94:
		snprintf(NameBuffer, NameBufferSize, "cmp_lt%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0x95:
		snprintf(NameBuffer, NameBufferSize, "cmp_ne%%s%%s");
		*Offset += 1;
		Byte = BRSEQ.DATAStruct.DATA_Section[*Offset];
		snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", Byte);
		*BytesAdded += 1;
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u, %i%%s", Byte, Int16);
		}
		break;
	case 0xE0:
		snprintf(NameBuffer, NameBufferSize, "userproc%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			UInt16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			*BytesAdded += 2;
			snprintf(ArgsBuffer, ArgsBufferSize, " %u,%%s", UInt16);
		}
		break;
	}
}


char decode_prefix(brseq_t BRSEQ, int* Offset, char** PrefixStr, char** PrefixArgsStr, int* PrefixDeterminesArgs, int* CmdOffset, int* BytesAdded) {
	int PrefixCmdOffset = *CmdOffset;
	unsigned char PrefixCmdByte = BRSEQ.DATAStruct.DATA_Section[PrefixCmdOffset];
	unsigned char Byte[4] = "\0";
	int16_t Int16[2] = { 0 };
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
		snprintf(PrefixName2, 1, "\0");
		snprintf(PrefixArgs2, 1, "\0");
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
		snprintf(prefixname, CMDNAMEBUFFERSIZE,"%%s::rand\0"); // %i %i // randomcmdbyte commandbyte commandargs(...) randMin(Byte[2]) randMax(Byte[2])
		snprintf(prefixargs, CMDARGSBUFFERSIZE, "%%s %i, %i", (int16_t)((Byte[0] << 8) | Byte[1]), (int16_t)((Byte[2] << 8) | Byte[3]));
		break;
	case 0xA1:
		*Offset += 1;
		Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
		*BytesAdded += 1;
		snprintf(prefixname, CMDNAMEBUFFERSIZE, "%%s::var\0"); // %u // varcmdbyte commandbyte commandargs(...) variable(Byte)
		snprintf(prefixargs, CMDARGSBUFFERSIZE, "%%s %u", Byte[0]);
		break;;
	case 0xA2:
		snprintf(prefixname, CMDNAMEBUFFERSIZE, "%%s::if\0");
		snprintf(prefixargs, CMDARGSBUFFERSIZE, "%%s\0");
		break;
	case 0xA3: // PCommandByte (FollowingCommand) TimeFactorInt16
		*Offset += 1;
		Int16[0] = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
		*Offset += 1;
		*BytesAdded += 2;
		snprintf(prefixname, CMDNAMEBUFFERSIZE, "%%s::time\0"); // %i // randomcmdbyte commandbyte commandargs(...) time(Int16)
		snprintf(prefixargs, CMDARGSBUFFERSIZE, "%%s, %i", Int16[0]);
		break;
	case 0xA4:
		*Offset += 1;
		Int16[0] = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
		*Offset += 2;
		Int16[1] = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
		*Offset += 1;
		*BytesAdded += 4;
		snprintf(prefixname, CMDNAMEBUFFERSIZE, "%%s::timerand\0"); // %i %i // randomcmdbyte commandbyte commandargs(...) timeMin(Int16) randMax(Int16)
		snprintf(prefixargs, CMDARGSBUFFERSIZE, "%%s, %i, %i", Int16[0], Int16[1]);
		break;
	case 0xA5:
		*Offset += 1;
		Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
		*BytesAdded += 1;
		snprintf(prefixname, CMDNAMEBUFFERSIZE, "%%s::timevar\0"); // %u // randomcmdbyte commandbyte commandargs(...) timeVariable(Byte)
		snprintf(prefixargs, CMDARGSBUFFERSIZE, "%%s, %u", Byte[0]);
		break;
	}

	if (*PrefixDeterminesArgs != 1) {
		*PrefixDeterminesArgs = 0;
	}

	char strbuffer[CMDNAMEBUFFERSIZE] = { "\0" };
	char argsbuffer[CMDARGSBUFFERSIZE] = "%s";
	snprintf(strbuffer, CMDNAMEBUFFERSIZE, "%s", prefixname);
	if (PrefixDeterminesArgs == 0) {
		snprintf(prefixargs, CMDARGSBUFFERSIZE, prefixargs, "%s,");
	}
	else {
		snprintf(prefixargs, CMDARGSBUFFERSIZE, prefixargs, "%s");
	}
	snprintf(argsbuffer, CMDARGSBUFFERSIZE, "%s", prefixargs);

	if (strcmp(prefixname, "\0") != 0) {
		snprintf(strbuffer, CMDNAMEBUFFERSIZE, prefixname, PrefixName2);
		snprintf(argsbuffer, CMDARGSBUFFERSIZE, prefixargs, PrefixArgs2);
	}

	free(PrefixName2);
	free(PrefixArgs2);

	size_t NameBufferSize = snprintf(NULL, 0, "%s", strbuffer) + 1;
	size_t ArgsBufferSize = snprintf(NULL, 0, "%s", argsbuffer) + 1;

	*PrefixStr = calloc(NameBufferSize, sizeof(char));
	*PrefixArgsStr = calloc(ArgsBufferSize, sizeof(char));
	snprintf(*PrefixStr, NameBufferSize, "%s", strbuffer);
	snprintf(*PrefixArgsStr, ArgsBufferSize, "%s", argsbuffer);
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
	int16_t Int16 = "\0";
	uint16_t UInt16 = "\0";
	uint32_t UInt24 = "\0";
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
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "%s%%s%%s", NoteName);

		BytesPassed = *Offset;
		*Offset += 1;
		Byte[1] = BRSEQ.DATAStruct.DATA_Section[*Offset];
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			VLQ = decode_vlq(BRSEQ.DATAStruct.DATA_Section, Offset);
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u, %u%%s", Byte[1], VLQ);
		}
		else{
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u,%%s", Byte[1]);
		}
		BytesPassed = *Offset - BytesPassed;
		break;
	case 0x80:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "wait%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			BytesPassed = *Offset;
			*Offset += 1;
			VLQ = decode_vlq(BRSEQ.DATAStruct.DATA_Section, Offset);
			BytesPassed = *Offset - BytesPassed;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", VLQ);
		}
		break;
	case 0x81:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "prg%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			BytesPassed = *Offset;
			*Offset += 1;
			VLQ = decode_vlq(BRSEQ.DATAStruct.DATA_Section, Offset);
			BytesPassed = *Offset - BytesPassed;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", VLQ);
		}
		break;
	case 0x88:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "opentrack%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			*Offset += 1;
			UInt24 = (((BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1]) << 8) | (BRSEQ.DATAStruct.DATA_Section[*Offset + 2]);
			*Offset += 2;
			ImportantOffsets[UInt24 + 0x0C] = 1;
			BytesPassed += 4;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u, %x%%s", Byte[0], UInt24);
		}
		break;
	case 0x89:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "jump%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			UInt24 = (((BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1]) << 8) | (BRSEQ.DATAStruct.DATA_Section[*Offset + 2]);
			*Offset += 2;
			ImportantOffsets[UInt24 + 0x0C] = 1;
			BytesPassed += 3;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %x%%s", UInt24);
		}
		break;
	case 0x8A:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "call%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			UInt24 = (((BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1]) << 8) | (BRSEQ.DATAStruct.DATA_Section[*Offset + 2]);
			*Offset += 2;
			ImportantOffsets[UInt24 + 0x0C] = 1;
			BytesPassed += 3;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %x%%s", UInt24);
		}
		break;
	case 0xB0:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "timebase%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xB1:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "envhold%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xB2:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "monophonic_bool%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xB3:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "velocityrange%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC0:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "pan%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC1:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "volume%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC2:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "mainvolume%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC3:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "transpose%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC4:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "pitchbend%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC5:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "bendrange%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC6:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "prio%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC7:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "notewait_bool%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC8:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "tie_bool%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xC9:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "prtmnto%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xCA:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "moddepth%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xCB:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "modspeed%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xCC:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "modtype%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xCD:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "modrange%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xCE:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "prtmnto_bool%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xCF:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "prtmntotime%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD0:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "attack%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD1:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "decay%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD2:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "sustain%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD3:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "release%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD4:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "loopstart%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD5:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "volume2%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD6:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "printvar%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD7:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "srndpan%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD8:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "lpfcutoff%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xD9:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "fxsenda%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xDA:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "fxsendb%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xDB:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "mainsend%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xDC:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "initpan%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xDD:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "mute%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xDE:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "fxsendc%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xDF:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "damper%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Byte[0] = BRSEQ.DATAStruct.DATA_Section[*Offset];
			BytesPassed += 1;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Byte[0]);
		}
		break;
	case 0xE0:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "moddelay%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			UInt16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			BytesPassed += 2;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", UInt16);
		}
		break;
	case 0xE1:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "tempo%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			BytesPassed += 2;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Int16);
		}
		break;
	case 0xE3:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "sweeppitch%%s%%s");
		if (PrefixDeterminesArgs == 0) {
			*Offset += 1;
			Int16 = (BRSEQ.DATAStruct.DATA_Section[*Offset] << 8) | BRSEQ.DATAStruct.DATA_Section[*Offset + 1];
			*Offset += 1;
			BytesPassed += 2;
			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %u%%s", Int16);
			}
		break;
	case 0xF0:
		*Offset += 1;
		CmdOffset += 1;
		BytesPassed += 1;
		decode_extended_command(BRSEQ, Offset, CmdName, CMDNAMEBUFFERSIZE, CmdArgs, CMDARGSBUFFERSIZE, PrefixDeterminesArgs, &BytesPassed);
		break; // 1, 1, Int16; // Follows F0 AA BB CC CC in hex, with AA being the Extended Command Byte (see decode_extcommand function), BB being a Byte, and CC CC being an Int16 value.
	case 0xFC:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "loopend%%s");
		break;
	case 0xFD:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "return%%s");
		break;
	case 0xFE:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "alloctrack%%s%%s");
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

			snprintf(CmdArgs, CMDARGSBUFFERSIZE, " %s%%s", bits);
		}
		break;
	case 0xFF:
		snprintf(CmdName, CMDNAMEBUFFERSIZE, "fin%%s");
		break;
	}

	char* prefixstr;
	char* prefixargs;
	decode_prefix(BRSEQ, Offset, &prefixstr, &prefixargs, &PrefixDeterminesArgs, &FirstPrefixCmdOffset, &BytesPassed);

	char FormattedCmdArgs[CMDARGSBUFFERSIZE] = { "\0" };

	snprintf(FormattedCmdArgs, CMDARGSBUFFERSIZE, CmdArgs, prefixargs);

	int NeededBufferSize = snprintf(NULL, 0, CmdName, prefixstr, FormattedCmdArgs) + sizeof(char);
	*StrBuffer = calloc(NeededBufferSize, sizeof(char));
	if (*StrBuffer == 0) {
		printf("Command StrBuffer calloc() failed. (decode_command)\n");
		exit(-1);
	}

	snprintf(*StrBuffer, NeededBufferSize, CmdName, prefixstr, FormattedCmdArgs);

	free(prefixstr);
	free(prefixargs);
}