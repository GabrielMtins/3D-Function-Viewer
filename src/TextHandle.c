#include "TextHandle.h"

#include "App.h"

#define FONT_WIDTH 6
#define FONT_HEIGHT 12

#define IS_VALID_CH(ch) ((ch) >= 32 && (ch) <= 126)

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
	if(!IS_VALID_CH(c)) return;

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

void TextHandle_RenderStr(struct App *app, float x, float y, const char *str, int len) {
	if(len < 0) len = strlen(str);

	for(int i = 0; i < len; i++) {
		TextHandle_RenderChar(
				app,
				x + i * SPACE_FONT_WIDTH,
				y,
				str[i]
				);
	}
}

void TextHandle_RenderInput(App *app, float x, float y, const TextInput *input) {
	TextHandle_RenderCursor(
			app,
			x + input->cursor * SPACE_FONT_WIDTH,
			y
			);
	
	TextHandle_RenderStr(app, x, y, input->str, arrlen(input->str));
}

bool TextHandle_HandleInput(const SDL_Event *event, TextInput *input) {
	bool modified = false;
	int len = arrlen(input->str);
	char c;

	if(input->cursor > len) input->cursor = len;

	switch(event->type) {
		case SDL_EVENT_TEXT_INPUT:
			c = event->text.text[0];

			if(!IS_VALID_CH(c)) {
				break;
			}

			if(len == MAX_EXPR_SIZE) {
				break;
			}

			arrins(
					input->str,
					input->cursor,
					c
					);

			input->cursor++;
			modified = true;
			break;

		case SDL_EVENT_KEY_DOWN:
			if(event->key.key == SDLK_LEFT && input->cursor > 0) {
				input->cursor--;
				modified = true;
			}

			if(event->key.key == SDLK_RIGHT && input->cursor < arrlen(input->str)) {
				input->cursor++;
				modified = true;
			}

			if(event->key.key == SDLK_BACKSPACE && input->cursor > 0) {
				arrdel(
						input->str,
						input->cursor - 1
					  );

				input->cursor--;
				modified = true;
			}

			break;
	}

	return modified;
}
