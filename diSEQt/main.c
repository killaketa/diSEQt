#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "brseq.h"
#include "brseq_commands.h"


int main(int argc, char** argv) {
	if (argv[2] == NULL) {
		printf("No file provided as second argument.");
		exit(0);
	}
	if (argc < 3) {
		printf("Invalid arguments.\nUsage is as follows:\nbrseqtool decode path/to/file\nbrseqtool encode path/to/file\nbrseqtool dc path/to/file\nbrseqtool ec path/to/file");
		exit(0);
	}
	if (strncmp(argv[1],"decode",7*sizeof(char)) == 0 || strncmp(argv[1], "dc", 3 * sizeof(char)) == 0) {
		decode_brseq(argv[2], argv[3]);
	}
	else if (strncmp(argv[1], "encode", 7 * sizeof(char)) == 0 || strncmp(argv[1], "ec", 3 * sizeof(char)) == 0) {
		encode_brseq(argv[2], argv[3]);
	}
}