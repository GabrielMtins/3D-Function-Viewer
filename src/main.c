#include <stdlib.h>

#include "App.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

static void main_loop(void) {
	if(!App_Loop()) {
		App_Quit();
		emscripten_cancel_main_loop();
	}
}
#endif

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;

	if(!App_Init()) {
		exit(1);
	}

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
#else
	App_Run();
#endif

	App_Quit();

	return 0;
}
