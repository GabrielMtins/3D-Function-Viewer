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
