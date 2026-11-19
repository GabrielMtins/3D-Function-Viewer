/*
This file is part of 3D Function Viewer.

3D Function Viewer is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

3D Function Viewer is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
3D Function Viewer. If not, see <https://www.gnu.org/licenses/>.
*/

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
