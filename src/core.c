#include <stdio.h>
#include <stdlib.h>
#include "init.h"

int main(int argc, char** argv) {

	init_multicore();

	while(1) {
		start_core();
	}


	return EXIT_SUCCESS;
}

