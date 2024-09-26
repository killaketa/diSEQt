#ifndef BRSEQSTRUCTHEADER // Don't want to load this multiple times since this file is called in multiple places.
#define BRSEQSTRUCTHEADER

#include <stdint.h>

// Multipurpose, used for both decoding and encoding.
typedef struct {
	char DATAHeaderStr[4]; // Always DATA.
	uint32_t DATA_Size; // Size of DATA Header in bytes
	uint32_t DATA_Offset; // Offset to DATA Header
	unsigned char* DATA_Section; // Entire DATA Section stored from fread_s
}DATA_t;

typedef struct {
	char LABLHeaderStr[4]; // Always LABL.
	uint32_t LABL_Size; // Size of LABL Header in bytes
	uint32_t LABL_Offset; // Offset to LABL Header
	unsigned char* LABL_Section; // Entire LABL Section stored from fread_s
}LABL_t;

typedef struct {
	char RSEQHeaderStr[4]; // Always RSEQ.
	unsigned char EndianBytes[2]; // 0xFEFF or 0xFFFE (Big / Little Endian respectively)
	uint16_t VersionNum; // Always stored as Little Endian, regardless of EndianBytes[2]. Everything after this is stored with EndianBytes[2] in consideration.
	uint32_t FileLength; // Length of the entire file in bytes.
	uint16_t RSEQ_Size; // Size of the RSEQ Header in bytes. Always 32.
	uint16_t NumberOfSections; // Number of headers. Always 2.
	uint32_t DATA_Offset; // Offset to DATA Header
	uint32_t DATA_Size; // Size of DATA Header in bytes
	uint32_t LABL_Offset; // Offset to LABL Header
	uint32_t LABL_Size; // Size of LABL Header in bytes
	DATA_t DATAStruct;
	LABL_t LABLStruct;
}brseq_t;


typedef struct {
	uint32_t SndDATA_Offset; // Offset to the labels commands from 0x0C of the DATA Header
	uint32_t LabelOffset; // Offset to the labels data from 0x08 of the LABL Header
	uint32_t StringLen; // Length of the string (obv)
	char String[100]; // String Name of the label.
}label_t;

typedef struct {
	uint32_t LabelLength; // Length of the LABL section.
	uint32_t LabelCount; // Amount of labels inside of the LABL Section
	label_t* Labels; // Pointers to the decoded labels. (EX. Labels[3] would be the 3rd label decoded into a label_t)
}LABLInfo_t;

brseq_t decode_brseq(const char* FilePath, char* DestTextPath);
brseq_t encode_brseq(char* TextFilePath, char* DestBRSEQPath);

#endif // !BRSEQSTRUCTHEADER