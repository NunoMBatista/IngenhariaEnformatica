#include <string.h>
#include <stdlib.h>

char * reverse_str(char * orig, int len)
{
	// Allocates space in dynamic memory for the reversed string
	char * result = (char *) malloc(len + 1);
	
	for (int i = 0; i < len; i++)
	{
		result[i] = orig[len - 1 - i];
	}
	
	result[len] = '\0';
	return result;
}
