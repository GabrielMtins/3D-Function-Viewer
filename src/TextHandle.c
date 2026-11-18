#include "TextHandle.h"

#include "App.h"

#include <string.h>

#define SPACE_FONT_WIDTH 18
#define SPACE_FONT_HEIGHT 36

#define FONT_WIDTH 6
#define FONT_HEIGHT 12

static void TextHandle_RenderCursor(App *app, float x, float y) {
	SDL_FRect dst = {
		x,
		y,
		SPACE_FONT_WIDTH,
		SPACE_FONT_HEIGHT
	};

	SDL_SetRenderDrawColor(app->renderer, 0x88, 0x88, 0x88, 0xff);
	SDL_RenderFillRect(app->renderer, &dst);
}

static void TextHandle_RenderChar(App *app, float x, float y, char c) {
	if(c < 32 || c > 126) return;

	SDL_FRect src = {
		(((int) c - 32) % 16) * FONT_WIDTH,
		(((int) c - 32) / 16) * FONT_HEIGHT,
		FONT_WIDTH,
		FONT_HEIGHT,
	};

	SDL_FRect dst = {
		x,
		y,
		SPACE_FONT_WIDTH,
		SPACE_FONT_HEIGHT
	};

	SDL_RenderTexture(
			app->renderer,
			app->font,
			&src,
			&dst
			);
}

void TextHandle_RenderStr(App *app, float x, float y, const TextInput *input) {
	int len = arrlen(input->str);

	for(int i = 0; i < len; i++) {
		if(i == input->cursor) {
			TextHandle_RenderCursor(
					app,
					x + input->cursor * SPACE_FONT_WIDTH,
					y
					);
		}
		
		TextHandle_RenderChar(
				app,
				x + i * SPACE_FONT_WIDTH,
				y,
				input->str[i]
				);
	}

	if(input->cursor == len) {
		TextHandle_RenderCursor(
				app,
				x + input->cursor * SPACE_FONT_WIDTH,
				y
				);
	}
}

void TextHandle_HandleInput(const SDL_Event *event, TextInput *input) {
	int len = arrlen(input->str);

	if(input->cursor > len) input->cursor = len;

	switch(event->type) {
		case SDL_EVENT_TEXT_INPUT:
			arrins(
					input->str,
					input->cursor,
					event->text.text[0]
					);

			input->cursor++;
			break;

		case SDL_EVENT_KEY_DOWN:
			if(event->key.key == SDLK_LEFT && input->cursor > 0) {
				input->cursor--;
			}

			if(event->key.key == SDLK_RIGHT && input->cursor < arrlen(input->str)) {
				input->cursor++;
			}

			if(event->key.key == SDLK_BACKSPACE && input->cursor > 0) {
				arrdel(
						input->str,
						input->cursor - 1
					  );

				input->cursor--;
			}

			break;
	}
}
