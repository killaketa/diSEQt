#ifndef MIDIHEADER
#define MIDIHEADER

typedef struct {
	long VLQ;
	unsigned char CmdByte;
	unsigned char Key; // Note byte
}dec_midi;

typedef struct {
	double Offset; // Offset from start of file in Bytes
	int Channel;
	// Channel the track is associated with (1024 tracks possible but only 16 channels possible). Each track chooses a channel number to operate on, meaning we must sort maximum 1024 MIDI tracks down to maximum 16 RSEQ tracks by MIDI channel use.
}trk_info;

void decode_midi(const char* FilePath, char* DestTextPath);

#endif // !MIDIHEADER
