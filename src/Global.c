#include "Global.h"

const SDL_FRect view3d_rect = {
	SCROLLBAR_WIDTH,
	SPACE_FONT_HEIGHT,
	SCREEN_WIDTH - SCROLLBAR_WIDTH,
	SCREEN_HEIGHT - SPACE_FONT_HEIGHT * 2
};

const SDL_FRect scrollbar_rect = {
	0,
	SPACE_FONT_HEIGHT + 1,
	SCROLLBAR_WIDTH,
	view3d_rect.h - 2
};
