#include "App.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stb_image.h>

#include "TextHandle.h"
#include "monogram_bitmap.h"

static App *app = NULL;

static void App_LoadFont(void);

static void App_LoadFont(void) {
	int w, h;

	uint8_t *data = stbi_load_from_memory(
			monogram_bitmap_png,
			monogram_bitmap_png_len,
			&w,
			&h,
			NULL,
			4
			);

	SDL_Surface *surface = SDL_CreateSurfaceFrom(
			w, h,
			SDL_PIXELFORMAT_RGBA32,
			data,
			w * 4
			);

	app->font = SDL_CreateTextureFromSurface(
			app->renderer,
			surface
			);

	SDL_DestroySurface(surface);

	stbi_image_free(data);

	SDL_SetTextureScaleMode(app->font, SDL_SCALEMODE_NEAREST);
}

bool App_Init(void) {
	bool success = true;

	app = calloc(1, sizeof(App));

	if(app == NULL) {
		fprintf(stderr, "Failed to allocate memory.\n");
		success = false;
		goto app_init_end;
	}

	if(!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Failed to init SDL: %s\n", SDL_GetError());
		success = false;
		goto app_init_end;
	}

	success |= SDL_CreateWindowAndRenderer(
			"3D Function Viewer",
			960,
			720,
			SDL_WINDOW_RESIZABLE,
			&app->window,
			&app->renderer
			);

	if(!success) {
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		goto app_init_end;
	}

	SDL_SetRenderVSync(app->renderer, 1);

	App_LoadFont();

	app->quit = false;

	SDL_StartTextInput(app->window);

app_init_end:
	return success;
}

void App_Loop(void) {
	SDL_Event event;

	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_EVENT_QUIT) {
			app->quit = true;
		}

		TextHandle_HandleInput(&event, &app->input);
	}

	SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(app->renderer);

	TextHandle_RenderStr(app, 0.0f, 0.0f, &app->input);

	SDL_RenderPresent(app->renderer);
}

void App_Run(void) {
	while(!app->quit) {
		App_Loop();
	}
}

void App_Quit(void) {
	SDL_DestroyRenderer(app->renderer);
	SDL_DestroyWindow(app->window);
	SDL_Quit();
}
