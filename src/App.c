#include "App.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stb_image.h>

#include "monogram_bitmap.h"

#define PIF 3.141592f

#define POINT_IN_RECT(x, y, rect) SDL_PointInRectFloat(&(SDL_FPoint){x, y}, &rect)

static App *app = NULL;

static bool App_LoadFont(void);
static void App_ComputeVertices(void);
static void App_UpdateViewTexture(void);
static void App_HandleInputIdle(SDL_Event *event);
static void App_HandleInputView3d(SDL_Event *event);
static void App_HandleInput(SDL_Event *event);
static void App_Render(void);

static bool App_LoadFont(void) {
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

	if(app->font == NULL) {
		return false;
	}

	SDL_DestroySurface(surface);

	stbi_image_free(data);

	SDL_SetTextureScaleMode(app->font, SDL_SCALEMODE_NEAREST);

	return true;
}

static void App_ComputeVertices(void) {
	arrclear(app->expr_str);
	arraddnindex(app->expr_str, arrlen(app->text_input.str));
	memcpy(app->expr_str, app->text_input.str, arrlen(app->text_input.str));
	arrpush(app->expr_str, '\0');

	MeshData_ComputeVertices(&app->mesh_data, app->expr_str);
}

static void App_UpdateViewTexture(void) {
	if(!app->update_view_texture) return;

	app->update_view_texture = false;

	SDL_SetRenderTarget(
			app->renderer,
			app->view_texture
			);

	SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(app->renderer);

	MeshData_ConvertToDraw(&app->mesh_data);

	SDL_RenderGeometry(
			app->renderer,
			NULL,
			app->mesh_data.draw_vertices,
			arrlen(app->mesh_data.draw_vertices),
			NULL,
			0
			);

	SDL_SetRenderTarget(app->renderer, NULL);
}

static void App_HandleInputIdle(SDL_Event *event) {
	if(TextHandle_HandleInput(event, &app->text_input)) {
		App_ComputeVertices();
		app->update_view_texture = true;
	}

	if(event->type != SDL_EVENT_MOUSE_BUTTON_DOWN) {
		return;
	}

	if(POINT_IN_RECT(event->button.x, event->button.y, view3d)) {
		app->input_state = INPUT_VIEW3D;
	}
}

static void App_HandleInputView3d(SDL_Event *event) {
	switch(event->type) {
		case SDL_EVENT_MOUSE_MOTION:
			app->mesh_data.rotations.raw[0] += PIF * event->motion.yrel / SCREEN_HEIGHT;
			app->mesh_data.rotations.raw[1] += PIF * event->motion.xrel / SCREEN_WIDTH;
			app->update_view_texture = true;
			break;

		case SDL_EVENT_MOUSE_BUTTON_UP:
			app->input_state = INPUT_IDLE;
			break;
	}
}

static void App_HandleInput(SDL_Event *event) {
	switch(app->input_state) {
		case INPUT_IDLE:
			App_HandleInputIdle(event);
			break;

		case INPUT_VIEW3D:
			App_HandleInputView3d(event);
			break;

		case INPUT_SCROLLBAR:
			break;
	}
}

static void App_Render(void) {
	App_UpdateViewTexture();

	SDL_SetRenderDrawColor(app->renderer, 0x20, 0x20, 0x20, 0xff);
	SDL_RenderClear(app->renderer);

	TextHandle_RenderStr(app, 0.0f, 0.0f, " 3D Function Viewer", -1);

	SDL_RenderTexture(
			app->renderer,
			app->view_texture,
			NULL,
			&view3d
			);

	TextHandle_RenderInput(app, 0.0f, SCREEN_HEIGHT - SPACE_FONT_HEIGHT, &app->text_input);

	SDL_RenderPresent(app->renderer);
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
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			SDL_WINDOW_RESIZABLE,
			&app->window,
			&app->renderer
			);

	if(!success) {
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		goto app_init_end;
	}

	SDL_SetRenderVSync(app->renderer, 1);

	if(!App_LoadFont()) {
		fprintf(stderr, "Failed to load font: %s\n", SDL_GetError());
		success = false;
		goto app_init_end;
	}

	app->view_texture = SDL_CreateTexture(
			app->renderer,
			SDL_PIXELFORMAT_RGBA32,
			SDL_TEXTUREACCESS_TARGET,
			view3d.w,
			view3d.h
			);

	if(app->view_texture == NULL) {
		fprintf(stderr, "Failed to create texture: %s", SDL_GetError());
		success = false;
		goto app_init_end;
	}

	SDL_SetTextureScaleMode(app->view_texture, SDL_SCALEMODE_NEAREST);

	SDL_StartTextInput(app->window);

	SDL_SetRenderLogicalPresentation(
			app->renderer,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			SDL_LOGICAL_PRESENTATION_LETTERBOX
			);

	MeshData_Init(&app->mesh_data);

	app->quit = false;
	app->input_state = INPUT_IDLE;
	app->update_view_texture = true;

	MeshData_ComputeVertices(&app->mesh_data, "0");

app_init_end:
	return success;
}

void App_Loop(void) {
	SDL_Event event;

	while(SDL_PollEvent(&event)) {
		SDL_ConvertEventToRenderCoordinates(app->renderer, &event);

		if(event.type == SDL_EVENT_QUIT) {
			app->quit = true;
		}

		App_HandleInput(&event);
	}

	App_Render();

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
