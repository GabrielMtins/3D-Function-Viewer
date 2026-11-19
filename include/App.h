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
} App;

bool App_Init(void);

void App_Loop(void);

void App_Run(void);

void App_Quit(void);

#endif
