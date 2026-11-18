#include <stdio.h>
#include <stdlib.h>

#include "App.h"

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;

	if(!App_Init()) {
		exit(1);
	}

	App_Run();

	App_Quit();

	return 0;
}
