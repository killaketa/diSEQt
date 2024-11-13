#ifndef MIDIHEADER
#define MIDIHEADER

typedef struct {
	long VLQ;
	unsigned char CmdByte;
	unsigned char Key; // Note byte
}dec_midi;

void decode_midi(const char* FilePath, char* DestTextPath);

#endif // !MIDIHEADER
