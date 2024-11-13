#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "brseq_commands.h"
#include "midi.h"

#define MICROSECONDS_TO_BPM(microseconds) round((1000000/microseconds)*60) // (MICROSECONDS_IN_SECOND / microseconds_per_beat_arg) * 60 = round(BPM)

#define MIDIWAIT_TO_RSEQWAIT(MidiWaitTime) round(MidiWaitTime/2) // RSEQ wait time is equivilant to the MIDI wait time divided by 2.

int decode_midi_ctrlmsg(unsigned char Byte, FILE* ByteStream) {
	// Appendix 1.2 (Table of MIDI Controller Messages) https://midimusic.github.io/tech/midispec.html#BMA1_2
	switch (Byte)
	{
	default:
		break;
	case 0x00: // Bank Select. 0-127. MSB.
		break;
	case 0x01: // * Modulation wheel. 0-127. MSB.
		break;
	case 0x02: // Breath control. 0-127. MSB.
		break;
	case 0x04: // Foot controller. 0-127. MSB.
		break;
	case 0x05: // Portamento time. 0-127. MSB.
		break;
	}
}

int decode_midi_metaevent(FILE* ByteStream) {

}

// Last 4 bits of the command byte is the channel number the command corresponds to.
int decode_midi_syscmd(unsigned char Byte, FILE* ByteStream) {
	// Appendix 1.1 (System Common Messages & System Real-Time Messages) https://midimusic.github.io/tech/midispec.html#BMA1_1
	switch (Byte & 0xF) // switch case for 4 bits from MSB
	{
	default:
		break;
	// System Common Messages
	case 0x0: { // System Exclusive. Ignore.
		unsigned char manufacturerID = fgetc(ByteStream);
		unsigned char message = fgetc(ByteStream);
		unsigned char nextbyte;
		while (nextbyte = fgetc(ByteStream)) {
			if (nextbyte != message) continue;
			if (fgetc(ByteStream) == 0xF7) break;
		}
		break;
	}
	case 0x2: { // Song Position Pointer. 0LLLLLLL 0MMMMMMM, L = LSB, M = MSB, ((M << 7) & L) = Number of MIDI beats (1 beat= six MIDI clocks) since the start of the song.
		unsigned char lsb = fgetc(ByteStream);
		unsigned char msb = fgetc(ByteStream);
		int beatsSinceStart = (msb << 7) & lsb;
		break;
	}
	case 0x3: { // Song Select. 0SSSSSSS, S = Sequence/Song(?). The Song Select specifies which sequence or song is to be played.
		unsigned char sequence = fgetc(ByteStream);
		break;
	}
	case 0x6: // Tune Request. Upon receiving a Tune Request, all analog synthesisers should tune their oscillators. Ignore(?).
		break;
	case 0x7: // End of Exclusive. Ignore.
		break;
	// System Real-Time Messages
	case 0x8: // Timing Clock. Sent 24 times per quarter note when synchronisation is required. Ignore(?).
		break;
	case 0xA: // Start. Start the current sequence playing. (This message will be followed with Timing Clocks).
		break;
	case 0xB: // Continue. Continue at the point the sequence was Stopped.
		break;
	case 0xC: // Stop. Stop the current sequence.
		break;
	case 0xE: // Active Sensing. Ignore.
		break;
	// Meta Event
	case 0xF: // Meta Event Escape. Command Byte is 0xFF total (1111 MSB and 1111 LSB) so escape into a meta event check.
		decode_midi_metaevent(ByteStream);
		break;
	}
}

dec_midi decode_midicmds(FILE* ByteStream, FILE* TextStream) { // Return VLQ on successful detection of command (hence the long type)
	long TrackOffset = 0; // 
	long VLQ = decode_vlq_bytestream(ByteStream);
	unsigned char cmdbyte = fgetc(ByteStream);
	int channelnum = (cmdbyte & 0xF);

	dec_midi CmdStruct = { .VLQ = VLQ, .CmdByte = cmdbyte >> 4, .Key = 255 };

	printf("%u\n", VLQ);

	if (VLQ != 0) {
		fprintf(TextStream, "\twait %ld\n",VLQ);
	}

	// Appendix 1.1 (Channel Voice Messages) https://midimusic.github.io/tech/midispec.html#BMA1_1
	switch (cmdbyte >> 4) // switch case for 4 bits from MSB
	{
	default:
		break;
	case 0x8: {// 1000 Note Off: 0kkkkkkk 0vvvvvvv (k = note, v = velocity). Return VLQ since Note On seeks Note Off then parses it into RSEQ note command length.
		unsigned char key = fgetc(ByteStream);
		unsigned char velocity = fgetc(ByteStream);
		CmdStruct.Key = key;
		break;
	}
	case 0x9: { // 1001 Note On: 0kkkkkkk 0vvvvvvv (k = note, v = velocity)
		int textpos = ftell(TextStream);
		unsigned char key = fgetc(ByteStream);
		unsigned char velocity = fgetc(ByteStream);
		CmdStruct.Key = key;

		long totalWaitVLQ = 0;
		dec_midi offCmdStruct = CmdStruct;

		while (feof(ByteStream) == 0) {
			offCmdStruct = decode_midicmds(ByteStream, TextStream);
			totalWaitVLQ += offCmdStruct.VLQ;

			if (offCmdStruct.CmdByte == 0x8) {
				fseek(TextStream, textpos, SEEK_SET);
				char NoteName[6] = { 0 };
				decode_notebyte(key, NoteName);

				fprintf(TextStream, "%s %u, %u\n", NoteName, velocity, MIDIWAIT_TO_RSEQWAIT(totalWaitVLQ));

				fseek(TextStream, 0, SEEK_END);
				break;
			}
		}
		break;
	}
	case 0xA: { // 1010 Polyphonic Key Pressure (Aftertouch). 0kkkkkkk 0vvvvvvv (k = note, v = pressure value)
		unsigned char key = fgetc(ByteStream);
		unsigned char pressure = fgetc(ByteStream);
		break;
	}
	case 0xB: { // 1011 Control Change: 0ccccccc 0vvvvvvv (c = controller value (defined in Appendix 1.2), v = new value)
		unsigned char controller = fgetc(ByteStream);
		unsigned char value = fgetc(ByteStream);
		break;
	}
	case 0xC: { // 1100 Program Change: 0ppppppp (p = new program number)
		unsigned char prognum = fgetc(ByteStream);
		break;
	}
	case 0xD: { // 1101 Channel Pressure (After-touch). 0vvvvvvv (v = pressure value)
		unsigned char pressure = fgetc(ByteStream);
		break;
	}
	case 0xE: { // 1110 Pitch Wheel Change. 0lllllll 0mmmmmmm (l = LSB, m = MSB, forms to mmmmmmmlllllll)
		unsigned char lsb = fgetc(ByteStream);
		unsigned char msb = fgetc(ByteStream);
		int pitch = (msb << 7) & lsb;
		break;
	}
	case 0xF: // 1111 Escape to system messages + meta events.
		decode_midi_syscmd(cmdbyte, ByteStream);
		break;
	}

	return CmdStruct;
}

void decode_midi(const char* FilePath, char* DestTextPath) { // Keep in mind that MIDI VLQ times are added upon (EX. if synthfont says event 1 = when 0, event 2 = when 10, event 3 = when 20, event 4 = when 30 then
	char* PeriodPtr = strrchr(FilePath, '.');																	 //  event 1 = 0, event 2 = 10, event 3 = 10, event 4 = 10.
	char* DestPeriodPtr = strrchr(DestTextPath, '.');

	if (DestTextPath == NULL) {
		perror("DestTextPath is NULL! You must provide a destination path as a 3rd argument to the executable.");
		exit(0);
	}

	if (strncmp(PeriodPtr + 1, "mid", 4) != 0) {
		perror("Invalid File Type! You can decode BRSEQ files only.");
		exit(0);
	}

	if (strncmp(DestPeriodPtr + 1, "xmlseq", 7) != 0) {
		perror("Invalid File Type! Output file must be filetype xmlseq.");
		exit(0);
	}

	FILE* ByteStream = fopen(FilePath, "rb");
	if (ByteStream == NULL) {
		perror("Invalid File Path to MIDI! fopen() failed");
		exit(-1);
	}

	FILE* TextStream = fopen(DestTextPath, "w");
	if (TextStream == NULL) {
		perror("Invalid File Path to XMLSeq! fopen() failed.");
		exit(-1);
	}

	char MTHDBytes[4] = {0};
	fread(MTHDBytes, 4 * sizeof(char), 1, ByteStream);
	if (strncmp(MTHDBytes, "MThd", 4) != 0) {
		perror("Invalid MIDI file! MThd header not found.");
		exit(0);
	}

	fseek(ByteStream, 4, SEEK_CUR); // Skip MThd length check since its ALWAYS 0x00 0x00 0x00 0x06 (6 in uint32_t decimal).

	uint16_t MIDIformat = 0;
	uint16_t TracksNum = 0;
	uint16_t BPM = 0;
	// Division var notes
	// if the MSB of the uint16_t is 1 then it looks 7 bits past msb for the negative SMPTE format (-24, -25, -29, or -30 in int16 decimal). then bits 7-0 (7 to LSB) is the seconds per beat in microseconds.
	// if the MSB of the uint16_t is 0 then the rest of the bits are used to make a uint16_t which represents the beats per minute.
	fread(&MIDIformat, sizeof(uint16_t), 1, ByteStream);
	fread(&TracksNum, sizeof(uint16_t), 1, ByteStream);
	fread(&BPM, sizeof(uint16_t), 1, ByteStream);
	MIDIformat = byteswap16(MIDIformat);
	TracksNum = byteswap16(TracksNum);
	BPM = byteswap16(BPM);

	if (BPM >= 0x8000) { // Check if MSB of BPM is 1. If so do some bitwise operations to grab the microseconds per beat and the negative SMPTE format.
		perror("MIDI Division byte has an MSB of 1, negative SMPTE format operations are not supported at the moment.");
		exit(0);
	}

	printf("%u %u %u\n", MIDIformat, TracksNum, BPM);

	fprintf(TextStream, XMLSEQ_START_STRING);

	char MTRKBytes[4] = {0};
	int TrackCount = 0;
	while (feof(ByteStream) == 0) {
		fread(MTRKBytes, 4 * sizeof(char), 1, ByteStream);
		if (strncmp(MTRKBytes, "MTrk", 4) != 0) {
			printf("Invalid MIDI file! MTrk header not found. Current MIDI Byte Position: %x", ftell(ByteStream));
			exit(0);
		}

		fprintf(TextStream, "<newtrack name=\"MID_Track_%i\" dataoffset=\"%u\">\n", TrackCount, ftell(ByteStream));
		decode_midicmds(ByteStream, TextStream, 255); // Hacky, set key to 255 since thats an impossible range for note on/off key detection.
		fprintf(TextStream, "</newtrack>\n\n");

		TrackCount++;
	}


	fclose(ByteStream);
	fclose(TextStream);
}