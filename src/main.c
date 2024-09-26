#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "brseq.h"
#include "brseq_commands.h"


int main(int argc, char** argv) {
	printf("%s\n", argv[1]);
	if (argc < 3) {
		printf("Invalid arguments.\nUsage is as follows:\ndiSEQt decode file.brseq destfile.xmlseq\ndiSEQt encode file.xmlseq destfile.brseq\ndiSEQt dc file.brseq destfile.xmlseq\ndiSEQt ec file.xmlseq destfile.brseq");
		exit(0);
	}
	if (strncmp(argv[1],"decode",7*sizeof(char)) == 0 || strncmp(argv[1], "dc", 3 * sizeof(char)) == 0) {
		decode_brseq(argv[2], argv[3]);
	}
	else if (strncmp(argv[1], "encode", 7 * sizeof(char)) == 0 || strncmp(argv[1], "ec", 3 * sizeof(char)) == 0) {
		encode_brseq(argv[2], argv[3]);
	}
}