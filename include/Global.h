#ifndef GLOBAL_H
#define GLOBAL_H

#define CGLM_NO_ANONYMOUS_STRUCT

#include <SDL3/SDL.h>
#include <stb_ds.h>
#include <cglm/struct.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 360

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define SPACE_FONT_WIDTH 6
#define SPACE_FONT_HEIGHT 12

#define INPUT_TEXT_Y (SCREEN_HEIGHT - SPACE_FONT_HEIGHT)

#define SCROLLBAR_WIDTH 12

#define CAMERA_MIN 4.0f
#define CAMERA_MAX 21.0f

#define MAX_EXPR_SIZE (SCREEN_WIDTH / SPACE_FONT_WIDTH - 12 - 1)

#define SCROLLBAR_PICK_HEIGHT 24

#define VIEW_UPDATE_RATE 16

extern const SDL_FRect view3d_rect;
extern const SDL_FRect scrollbar_rect;

typedef struct Vertex {
	vec3s pos;
	float color;
} Vertex;

typedef char * CharVector;
typedef Vertex * VertexVector;
typedef SDL_Vertex * SdlVertexVector;

#endif
