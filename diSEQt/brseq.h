#ifndef BRSEQSTRUCTHEADER // Don't want to load this multiple times since this file is called in multiple places.
#define BRSEQSTRUCTHEADER

// Multipurpose, used for both decoding and encoding.
typedef struct {
	char DATAHeaderStr[4]; // Always DATA.
	unsigned __int32 DATA_Size; // Size of DATA Header in bytes
	unsigned __int32 DATA_Offset; // Offset to DATA Header
	unsigned char* DATA_Section; // Entire DATA Section stored from fread_s
}DATA_t;

typedef struct {
	char LABLHeaderStr[4]; // Always LABL.
	unsigned __int32 LABL_Size; // Size of LABL Header in bytes
	unsigned __int32 LABL_Offset; // Offset to LABL Header
	unsigned char* LABL_Section; // Entire LABL Section stored from fread_s
}LABL_t;

typedef struct {
	char RSEQHeaderStr[4]; // Always RSEQ.
	unsigned char EndianBytes[2]; // 0xFEFF or 0xFFFE (Big / Little Endian respectively)
	unsigned __int16 VersionNum; // Always stored as Little Endian, regardless of EndianBytes[2]. Everything after this is stored with EndianBytes[2] in consideration.
	unsigned __int32 FileLength; // Length of the entire file in bytes.
	unsigned __int16 RSEQ_Size; // Size of the RSEQ Header in bytes. Always 32.
	unsigned __int16 NumberOfSections; // Number of headers. Always 2.
	unsigned __int32 DATA_Offset; // Offset to DATA Header
	unsigned __int32 DATA_Size; // Size of DATA Header in bytes
	unsigned __int32 LABL_Offset; // Offset to LABL Header
	unsigned __int32 LABL_Size; // Size of LABL Header in bytes
	DATA_t DATAStruct;
	LABL_t LABLStruct;
}brseq_t;


typedef struct {
	unsigned __int32 SndDATA_Offset; // Offset to the labels commands from 0x0C of the DATA Header
	unsigned __int32 LabelOffset; // Offset to the labels data from 0x08 of the LABL Header
	unsigned __int32 StringLen; // Length of the string (obv)
	char String[100]; // String Name of the label.
}label_t;

typedef struct {
	unsigned __int32 LabelLength; // Length of the LABL section.
	unsigned __int32 LabelCount; // Amount of labels inside of the LABL Section
	label_t* Labels; // Pointers to the decoded labels. (EX. Labels[3] would be the 3rd label decoded into a label_t)
}LABLInfo_t;

brseq_t decode_brseq(const char* FilePath);
brseq_t encode_brseq(char* TextFilePath);


#endif // !BRSEQSTRUCTHEADER