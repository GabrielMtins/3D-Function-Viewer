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
