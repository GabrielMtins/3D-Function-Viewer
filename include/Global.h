#ifndef GLOBAL_H
#define GLOBAL_H

#include <SDL3/SDL.h>
#include <stb_ds.h>
#include <cglm/struct.h>

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 360

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720

#define SPACE_FONT_WIDTH 6
#define SPACE_FONT_HEIGHT 12

extern const SDL_FRect view3d;

typedef struct Vertex {
	vec3s pos;
	float color;
} Vertex;

typedef char * CharVector;
typedef Vertex * VertexVector;
typedef SDL_Vertex * SdlVertexVector;

#endif
