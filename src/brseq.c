#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "brseq.h"
#include "brseq_commands.h"
#include "rseq_parse.h"
#include "common.h"

// Reads from StartBuffer until 0xFF, 0xFC, or 0xFD is hit (fin, loop_end, return commands)
// Start at 0x0C because thats the end of the DATA Header (0x00: DATA, 0x04: Length of Section, 0x08: Offset to DATA from start of header (always 0x0C))
char* decode_DATASection(brseq_t BRSEQ, LABLInfo_t LABL, FILE* TextStream, int* ImportantOffsets) {
	unsigned int HighestOffset = 0;

	for (unsigned int i2 = 0; i2 < LABL.LabelCount; i2++) {
		if (LABL.Labels[i2].SndDATA_Offset > HighestOffset) {
			HighestOffset = LABL.Labels[i2].SndDATA_Offset;
		}
	}

	for (unsigned int i = 0; i < LABL.LabelCount; i++) {
		fprintf(TextStream, "<newtrack name=\"%s\" dataoffset=\"%u\">\n", LABL.Labels[i].String, LABL.Labels[i].SndDATA_Offset);

		int IsFin = 0;

		for (unsigned int i2 = LABL.Labels[i].SndDATA_Offset + 0x0C; i2 < BRSEQ.DATA_Size; i2++) {
			char* cmd;
			unsigned char cmdbyte = BRSEQ.DATAStruct.DATA_Section[i2];

			for (unsigned int i3 = 0; i3 < LABL.LabelCount; i3++) {
				if ((i2 - 0x0C) == (LABL.Labels[i3].SndDATA_Offset) && i != i3 && (LABL.Labels[i].SndDATA_Offset) != (LABL.Labels[i3].SndDATA_Offset)) {
					IsFin = 1;
					break;
				}
			}

			if (ImportantOffsets[i2] != 0 && IsFin == 0) {
				fprintf(TextStream, "\t<offset name=\"%x\"/> ;; Offset %x\n", i2 - 0x0C, i2 - 0x0C);
			}

			if (IsFin == 0) {
				//printf("%x\n", BRSEQ.DATAStruct.DATA_Section[i2]);
				decode_command(BRSEQ, &i2, &cmd, ImportantOffsets);
				fprintf(TextStream, "\t%s\n", cmd);
				free(cmd);
			}
			else {
				break;
			}

			if ((cmdbyte == 0xFF | cmdbyte == 0xFD) && (i2 >= HighestOffset + 0x0C) && (BRSEQ.DATAStruct.DATA_Section[i2] == 0xFF)) {
				IsFin = 1;
			}

		}
		fprintf(TextStream, "</newtrack>\n\n");
	}
}

char* predecode_DATASection(brseq_t BRSEQ, LABLInfo_t LABL, int* ImportantOffsets) {
	unsigned int HighestOffset = 0;

	for (unsigned int i2 = 0; i2 < LABL.LabelCount; i2++) {
		if (LABL.Labels[i2].SndDATA_Offset > HighestOffset) {
			HighestOffset = LABL.Labels[i2].SndDATA_Offset;
		}
	}

	for (unsigned int i = 0; i < LABL.LabelCount; i++) {

		int IsFin = 0;

		for (unsigned int i2 = LABL.Labels[i].SndDATA_Offset + 0x0C; i2 < BRSEQ.DATA_Size; i2++) {
			char* cmd;
			unsigned char cmdbyte = BRSEQ.DATAStruct.DATA_Section[i2];

			for (unsigned int i3 = 0; i3 < LABL.LabelCount; i3++) {
				if ((i2 - 0x0C) == LABL.Labels[i3].SndDATA_Offset && i != i3) {
					IsFin = 1;
					break;
				}
			}

			if (IsFin == 0) {
				decode_command(BRSEQ, &i2, &cmd, ImportantOffsets);
				free(cmd);
			}
			else {
				break;
			}

			if ((cmdbyte == 0xFF | cmdbyte == 0xFD) && (i2 >= HighestOffset + 0x0C) && (BRSEQ.DATAStruct.DATA_Section[i2] == 0xFF)) {
				IsFin = 1;
			}
		}
	}
}

LABLInfo_t decode_LABLSection(brseq_t BRSEQ, FILE* TextStream) {
	LABLInfo_t LABL;
	LABLInfo_t LABLOrdered;
	LABL.LabelLength = BRSEQ.LABLStruct.LABL_Section[4] << 24 | BRSEQ.LABLStruct.LABL_Section[5] << 16 | BRSEQ.LABLStruct.LABL_Section[6] << 8 | BRSEQ.LABLStruct.LABL_Section[7];
	LABL.LabelCount = BRSEQ.LABLStruct.LABL_Section[8] << 24 | BRSEQ.LABLStruct.LABL_Section[9] << 16 | BRSEQ.LABLStruct.LABL_Section[10] << 8 | BRSEQ.LABLStruct.LABL_Section[11];
	LABL.Labels = calloc(LABL.LabelCount, sizeof(label_t));

	LABLOrdered.LabelLength = BRSEQ.LABLStruct.LABL_Section[4] << 24 | BRSEQ.LABLStruct.LABL_Section[5] << 16 | BRSEQ.LABLStruct.LABL_Section[6] << 8 | BRSEQ.LABLStruct.LABL_Section[7];
	LABLOrdered.LabelCount = BRSEQ.LABLStruct.LABL_Section[8] << 24 | BRSEQ.LABLStruct.LABL_Section[9] << 16 | BRSEQ.LABLStruct.LABL_Section[10] << 8 | BRSEQ.LABLStruct.LABL_Section[11];
	LABLOrdered.Labels = calloc(LABL.LabelCount, sizeof(label_t));
	unsigned int i;

	for (i = 0; i < LABL.LabelCount; i++) {
		// Offset to Label Data (relative to 0x08 from the start of LABL header)
		LABL.Labels[i].LabelOffset = BRSEQ.LABLStruct.LABL_Section[0x0C + (i * 4)] << 24 | BRSEQ.LABLStruct.LABL_Section[0x0C + (i * 4) + 1] << 16 | BRSEQ.LABLStruct.LABL_Section[0x0C + (i * 4) + 2] << 8 | BRSEQ.LABLStruct.LABL_Section[0x0C + (i * 4) + 3];
		LABL.Labels[i].LabelOffset += 0x08;
		int i2;
		LABL.Labels[i].SndDATA_Offset = BRSEQ.LABLStruct.LABL_Section[LABL.Labels[i].LabelOffset] << 24 | BRSEQ.LABLStruct.LABL_Section[LABL.Labels[i].LabelOffset + 1] << 16 | BRSEQ.LABLStruct.LABL_Section[LABL.Labels[i].LabelOffset + 2] << 8 | BRSEQ.LABLStruct.LABL_Section[LABL.Labels[i].LabelOffset + 3];
		LABL.Labels[i].StringLen = BRSEQ.LABLStruct.LABL_Section[LABL.Labels[i].LabelOffset + 4] << 24 | BRSEQ.LABLStruct.LABL_Section[LABL.Labels[i].LabelOffset + 5] << 16 | BRSEQ.LABLStruct.LABL_Section[LABL.Labels[i].LabelOffset + 6] << 8 | BRSEQ.LABLStruct.LABL_Section[LABL.Labels[i].LabelOffset + 7];
		if (LABL.Labels[i].StringLen > 100) {
			printf("Label name is too long! Please make sure no Label name is over 100 characters.");
			exit(0);
		};
		for (i2 = 0; i2 < LABL.Labels[i].StringLen; i2++) {
			LABL.Labels[i].String[i2] = BRSEQ.LABLStruct.LABL_Section[LABL.Labels[i].LabelOffset + 8 + i2];
		}
	}

	int min_elem;

	for (i = 0; i < LABL.LabelCount-1; i++) {

		min_elem = i;

		for (unsigned int i2 = i + 1; i2 < LABL.LabelCount; i2++) {
			if (LABL.Labels[min_elem].SndDATA_Offset > LABL.Labels[i2].SndDATA_Offset) {
				min_elem = i2;
			}
		}

		if (min_elem != i) {
			label_t temp = LABL.Labels[min_elem];
			LABL.Labels[min_elem] = LABL.Labels[i];
			LABL.Labels[i] = temp;
		}
	}

	for (i = 0; i < LABL.LabelCount; i++) {
		printf("DATA Offset: %u, StringLen: %u, String: %s\n", LABL.Labels[i].SndDATA_Offset, LABL.Labels[i].StringLen, LABL.Labels[i].String);
	}


	return LABL;
}

char* decode_section_data(FILE* ByteStream, unsigned char** Buffer, uint32_t Offset, uint32_t Size) {
	*Buffer = calloc(Size,sizeof(char));
	fseek(ByteStream, Offset, SEEK_SET);
	fread(*Buffer, Size * sizeof(char), 1, ByteStream);
	fseek(ByteStream, 0, SEEK_SET);
	if (*Buffer == NULL) {
		printf("Null Section Data!");
		exit(0);
	}

	return *Buffer;
}

brseq_t decode_sections(FILE* ByteStream, FILE* TextStream) {
	brseq_t BRSEQ = {.DATA_Offset = 0, .DATA_Size = 0, .LABL_Offset = 0, .LABL_Size = 0};
	
	// Get DATA Header offset & size
	fseek(ByteStream, 16, SEEK_SET);
	fread(&BRSEQ.DATA_Offset, sizeof(uint32_t), 1, ByteStream);
	fseek(ByteStream, 20, SEEK_SET);
	fread(&BRSEQ.DATA_Size, sizeof(uint32_t), 1, ByteStream);

	// Get LABL Header offset & size
	fseek(ByteStream, 24, SEEK_SET);
	fread(&BRSEQ.LABL_Offset, sizeof(uint32_t), 1, ByteStream);
	fseek(ByteStream, 28, SEEK_SET);
	fread(&BRSEQ.LABL_Size, sizeof(uint32_t), 1, ByteStream);

	// Byteswap all the retrieved data since fread_s stores it as Little Endian (x86 processors use Little Endian, MKW uses Big Endian)
	BRSEQ.DATA_Offset = byteswap32(BRSEQ.DATA_Offset);
	BRSEQ.DATA_Size = byteswap32(BRSEQ.DATA_Size);

	BRSEQ.LABL_Offset = byteswap32(BRSEQ.LABL_Offset);
	BRSEQ.LABL_Size = byteswap32(BRSEQ.LABL_Size);

	decode_section_data(ByteStream, &BRSEQ.DATAStruct.DATA_Section, BRSEQ.DATA_Offset, BRSEQ.DATA_Size);
	decode_section_data(ByteStream, &BRSEQ.LABLStruct.LABL_Section, BRSEQ.LABL_Offset, BRSEQ.LABL_Size);

	printf("DATA Offset: %u, DATA Size: %u, LABL Offset: %u, LABL Size: %u\n\n", BRSEQ.DATA_Offset, BRSEQ.DATA_Size, BRSEQ.LABL_Offset, BRSEQ.LABL_Size);

	fseek(ByteStream, 0x04, SEEK_SET);
	fread(&BRSEQ.EndianBytes[0], sizeof(unsigned char), 1, ByteStream);
	fseek(ByteStream, 0x05, SEEK_SET);
	fread(&BRSEQ.EndianBytes[1], sizeof(unsigned char), 1, ByteStream);
	fprintf(TextStream, "// Endian: %02x%02x\n", BRSEQ.EndianBytes[0], BRSEQ.EndianBytes[1]);

	return BRSEQ;
}

brseq_t decode_brseq(const char* FilePath, char* DestTextPath) {
	int i;
	char* PeriodPtr = strrchr(FilePath, '.');
	char* DestPeriodPtr = strrchr(DestTextPath, '.');

	if (DestTextPath == NULL) {
		perror("DestTextPath is NULL! You must provide a destination path as a 3rd argument to the executable.");
		exit(0);
	}

	if (strncmp(PeriodPtr + 1, "brseq", 6) != 0) {
		perror("Invalid File Type! You can decode BRSEQ files only.");
		exit(0);
	}

	if (strncmp(DestPeriodPtr + 1, "xmlseq", 7) != 0) {
		perror("Invalid File Type! Output file must be filetype xmlseq.");
		exit(0);
	}

	FILE* ByteStream = fopen(FilePath, "rb");
	if (ByteStream == NULL) {
		perror("Invalid File Path to BRSEQ! fopen() failed");
		exit(-1);
	}

	char RSEQBytes[4] = {0};
	fread(RSEQBytes, 4 * sizeof(char), 1, ByteStream);
	if (strncmp(RSEQBytes, "RSEQ", 4) != 0) {
		perror("Invalid BRSEQ file! RSEQ header not found.");
		exit(0);
	}

	FILE* TextStream = fopen(DestTextPath, "w");
	if (TextStream == NULL) {
		perror("Invalid File Path to XMLSeq! fopen() failed.");
		exit(-1);
	}

	fprintf(TextStream, XMLSEQ_START_STRING);


	brseq_t BRSEQ = decode_sections(ByteStream, TextStream); // Write BOM (whether file uses Big or Little Endian) plus other RSEQ header bs to the TextStream before passing to decode_LABLSection.
	fclose(ByteStream);

	LABLInfo_t LABL = decode_LABLSection(BRSEQ, TextStream);

	// Print LABL info to the file
	fprintf(TextStream, "// Label Count: %u\n", LABL.LabelCount);
	fprintf(TextStream, "// Label Length: %u\n\n", LABL.LabelLength);

	int* ImportantOffsets = calloc(BRSEQ.DATA_Size, sizeof(int));
	predecode_DATASection(BRSEQ, LABL, ImportantOffsets);
	decode_DATASection(BRSEQ, LABL, TextStream, ImportantOffsets);

	fclose(TextStream);
	free(LABL.Labels);
	free(ImportantOffsets);

	return BRSEQ;
}


brseq_t encode_brseq(char* TextFilePath, char* DestBRSEQPath) {
	int i;
	char* PeriodPtr = strrchr(TextFilePath, '.');
	char* DestPeriodPtr = strrchr(DestBRSEQPath, '.');

	if (DestBRSEQPath == NULL) {
		perror("DestBRSEQPath is NULL! You must provide a destination path as a 3rd argument to the executable.");
		exit(0);
	}

	if (strncmp(PeriodPtr + 1, "xmlseq", 7) != 0) {
		perror("Invalid File Type! You can decode BRSEQ files only.");
		exit(0);
	}

	if (strncmp(DestPeriodPtr + 1, "brseq", 6) != 0) {
		perror("Invalid File Type! Output file must be filetype xmlseq.");
		exit(0);
	}

	FILE* TextStream = fopen(TextFilePath, "r");
	if (TextStream == NULL) {
		perror("Invalid File Path to XMLSeq! fopen() failed.");
		exit(-1);
	}

	FILE* ByteStream = fopen(DestBRSEQPath, "wb+");
	if (ByteStream == NULL) {
		perror("ByteStream is NULL! fopen() failed.");
		exit(-1);
	}

	DATA_t DATAStruct = { .DATAHeaderStr = "DATA", .DATA_Offset = byteswap32(0x0C), .DATA_Section = 0, .DATA_Size = 0};
	LABL_t LABLStruct = { .LABLHeaderStr = "LABL", .LABL_Offset = 0, .LABL_Section = 0, .LABL_Size = 0 };
	brseq_t BRSEQ = { .RSEQHeaderStr = "RSEQ", .EndianBytes = {0xfe,0xff}, .VersionNum = 1, .FileLength = 0, .RSEQ_Size = byteswap16(0x20), .NumberOfSections = byteswap16(2), .DATA_Offset = byteswap32(0x20), .DATA_Size = 0, .LABL_Offset = 0, .LABL_Size = 0, .DATAStruct = DATAStruct, .LABLStruct = LABLStruct};
	
	parse_textstream(TextStream, ByteStream, BRSEQ);

	fclose(TextStream);
	fclose(ByteStream);

	return BRSEQ;
}