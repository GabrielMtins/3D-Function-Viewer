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

#ifndef APP_H
#define APP_H

#include "Global.h"
#include "TextHandle.h"
#include "MeshData.h"

typedef enum InputState {
	INPUT_IDLE,
	INPUT_SCROLLBAR,
	INPUT_VIEW3D,
} InputState;

typedef struct App {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *font;
	SDL_Texture *view_texture;

	TextInput text_input;
	CharVector expr_str;
	MeshData mesh_data;

	InputState input_state;

	float scroll_perc;

	uint32_t next_view_update;

	bool quit;
	bool update_view_texture;
	bool fullscreen;
} App;

bool App_Init(void);

bool App_Loop(void);

void App_Run(void);

void App_Quit(void);

#endif
