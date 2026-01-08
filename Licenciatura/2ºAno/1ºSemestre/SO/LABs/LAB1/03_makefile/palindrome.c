#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "aux_file.h"

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("Missing parameter.\nUsage: %s <word>\n", argv[0]);
		return -1;
	}

	char * input = argv[1];
	char * reversed = reverse_str(input, strlen(input));
	if (strcmp(reversed, input) == 0) {
		printf("The string %s is a palindrome\n", input);
	} else {
		printf("The string %s is NOT a palindrome\n", input);
	}
	if (reversed != NULL)
		free(reversed);
	return 0;
}
