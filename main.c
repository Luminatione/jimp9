#include "alex.h"
#include "parser.h"

#include <stdlib.h>

int main(int argv, char** argc)
{

	if (argv != 2)
	{
		fprintf(stderr, "Niepoprawna liczba argumentow");
		return EXIT_FAILURE;
	}
	analizatorSkladni(argc[1]);
	return EXIT_SUCCESS;
}