#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "init.h"

int main(int argc, char** argv) {

	init_multicore();

	while(1) {
		// Exercice 1 - 2
		//start_core();
        // Exercice 3
        start_core_semaphore();
	}


	return EXIT_SUCCESS;
}

