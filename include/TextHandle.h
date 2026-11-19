#ifndef TEXT_HANDLE_H
#define TEXT_HANDLE_H

#include "Global.h"

struct App;

typedef struct TextInput {
	CharVector str;
	int cursor;
} TextInput;

void TextHandle_RenderStr(struct App *app, float x, float y, const char *str, int len);

void TextHandle_RenderInput(struct App *app, float x, float y, const TextInput *input);

bool TextHandle_HandleInput(const SDL_Event *event, TextInput *input);

#endif
