#ifndef APP_H
#define APP_H

#include "Global.h"
#include "TextHandle.h"

typedef struct App {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *font;

	TextInput input;

	bool quit;
} App;

bool App_Init(void);

void App_Loop(void);

void App_Run(void);

void App_Quit(void);

#endif
