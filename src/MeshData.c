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

#include "MeshData.h"
#include "Global.h"

#include <math.h>
#include <wchar.h>

static bool MeshData_Compile(MeshData *data, const char *str);
static float MeshData_EvalHeight(const MeshData *data, float x, float z);
static vec3s MeshData_EvalPoint(const MeshData *data, float x, float z);
static vec2s MeshData_EvalUV(const MeshData *data, float x, float z);
static vec3s MeshData_EvalNormal(const MeshData *data, float x, float z);
static float MeshData_EvalColor(const MeshData *data, float x, float z);
static void MeshData_BuildQuad(MeshData *data, float x, float z, float step);
static void MeshData_BuildFullMesh(MeshData *data);
static void MeshData_ApplyTransformations(MeshData *data);
static void MeshData_ComputeTexture(MeshData *data);

static bool MeshData_Compile(MeshData *data, const char *str) {
	int error;

	if(MP_IsValid(&data->expr)) {
		MP_Free(&data->expr);
	}

	data->expr = MP_Compile(str, (const char *[]){"x", "y"}, 2, &error);

	return error == 0;
}

static float MeshData_EvalHeight(const MeshData *data, float x, float z) {
	return MP_Eval(&data->expr, (const float[]){x, z});
}

static vec3s MeshData_EvalPoint(const MeshData *data, float x, float z) {
	return (vec3s) {
		x,
		MeshData_EvalHeight(data, x, z),
		z,
	};
}

static vec2s MeshData_EvalUV(const MeshData *data, float x, float z) {
	vec2s p =  (vec2s) {
		((x - data->graph_range) / data->graph_range) * 0.5f + 1.0f,
		((z - data->graph_range) / data->graph_range) * 0.5f + 1.0f,
	};

	return p;
}

static vec3s MeshData_EvalNormal(const MeshData *data, float x, float z) {
	float step = 0.00001f;
	float og = MeshData_EvalHeight(data, x, z);

	float dx = MeshData_EvalHeight(data, x + step, z) - og;
	dx /= step;

	float dz = MeshData_EvalHeight(data, x, z + step) - og;
	dz /= step;

	return glms_normalize((vec3s){
			-dx,
			1.0f,
			-dz
			});
}

static float MeshData_EvalColor(const MeshData *data, float x, float z) {
	vec3s normal = MeshData_EvalNormal(data, x, z);

	return glms_dot(data->light_dir, normal) * 0.5f + 0.5f;
}

static void MeshData_BuildQuad(MeshData *data, float x, float z, float step) {
	Vertex v[4];

	for(int i = 0; i < 4; i++) {
		v[i] = (Vertex) {
			.pos = MeshData_EvalPoint(data, x + step * (i % 2), z + step * (i / 2)),
			.uv = MeshData_EvalUV(data, x + step * (i % 2), z + step * (i / 2)),
			.color = MeshData_EvalColor(data, x + step * (i % 2), z + step * (i / 2)),
		};

		if(!isfinite(v[i].pos.raw[1])) {
			return;
		}
	}

	arrpush(data->raw_vertices, v[0]);
	arrpush(data->raw_vertices, v[1]);
	arrpush(data->raw_vertices, v[2]);

	arrpush(data->raw_vertices, v[1]);
	arrpush(data->raw_vertices, v[2]);
	arrpush(data->raw_vertices, v[3]);
}

static void MeshData_BuildFullMesh(MeshData *data) {
	arrclear(data->raw_vertices);

	float graph_range = floorf(data->camera_distance / 2.0f);
	float step = (graph_range * 2.0f) / 40.0f;

	data->graph_range = graph_range;

	for(float z = -graph_range; z < graph_range - step * 0.1f; z += step) {
		for(float x = -graph_range; x < graph_range - step * 0.1f; x += step) {
			MeshData_BuildQuad(data, x, z, step);
		}
	}

	MeshData_ComputeTexture(data);
}

static void MeshData_ApplyTransformations(MeshData *data) {
	data->cg.len = 0;
	TCG_AppendData(&data->cg, data->raw_vertices, arrlen(data->raw_vertices));

	TCG_Mat4 res;

	TCG_Mat4Identity(res);

	TCG_Mat4ScaleMul(res, (TCG_Vec3){1.0f, 1.0f, -1.0f});
	TCG_Mat4RotateYMul(res, data->rotations.raw[1]);
	TCG_Mat4RotateXMul(res, data->rotations.raw[0]);
	TCG_Mat4TranslateMul(res, (TCG_Vec3){0.0f, 0.0f, -data->actual_camera_distance -2.0f});
	TCG_Mat4Mul(data->projection, res);

	TCG_Transform(&data->cg, res);
	TCG_SmartClipping(&data->cg);
	TCG_ApplyProjection(&data->cg);
	TCG_ScaleToScreenSize(&data->cg, view3d_rect.w, view3d_rect.h);
	TCG_SortByDepth(&data->cg);
}

static void MeshData_ComputeTexture(MeshData *data) {
	void *raw;
	int pitch;
	const SDL_PixelFormatDetails *details = 
		SDL_GetPixelFormatDetails(data->texture->format);

	SDL_LockTexture(data->texture, NULL, &raw, &pitch);
	uint32_t *pixels = raw;

	pitch /= 4;

	for(int j = 0; j < MESH_TEXTURE_SIZE; j++) {
		for(int i = 0; i < MESH_TEXTURE_SIZE; i++) {
			uint8_t color = (uint8_t) (MeshData_EvalColor(
					data,
					(((float) i / MESH_TEXTURE_SIZE) * 2.0f - 1.0f) * data->graph_range,
					(((float) j / MESH_TEXTURE_SIZE) * 2.0f - 1.0f) * data->graph_range
					) * 255.0f);

			pixels[i + j * pitch] = SDL_MapRGBA(
					details,
					NULL,
					color, color, color, 0xff
					);
		}
	}

	SDL_UnlockTexture(data->texture);
}

void MeshData_Init(MeshData *data, SDL_Renderer *renderer) {
	data->cg = TCG_BaseData(
			TCG_TRIANGLE,
			sizeof(Vertex) / sizeof(float),
			0
			);

	data->light_dir = glms_normalize((vec3s){1.0f, 1.0f, 1.0f});

	TCG_Mat4Perspective(
			data->projection,
			TCG_Radians(45.0f),
			view3d_rect.w / view3d_rect.h,
			0.01f,
			100.0f
			);

	data->actual_camera_distance = CAMERA_MIN;
	data->camera_distance = 6;
	data->rotations.raw[0] = TCG_Radians(35.0f);

	data->texture = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_RGBA32,
			SDL_TEXTUREACCESS_STREAMING,
			MESH_TEXTURE_SIZE,
			MESH_TEXTURE_SIZE
			);
}

void MeshData_ComputeVertices(MeshData *data, const char *str) {
	if(!MeshData_Compile(data, str)) {
		return;
	}

	MeshData_BuildFullMesh(data);
}

void MeshData_ConvertToDraw(MeshData *data) {
	MeshData_ApplyTransformations(data);

	arrclear(data->draw_vertices);

	for(unsigned int i = 0; i < data->cg.len; i++) {
		Vertex *vertex = ((Vertex *) data->cg.raw) + i;

		SDL_Vertex sdl_vertex = {
			.position = {vertex->pos.raw[0], vertex->pos.raw[1]},
			.color = {1.0f, 1.0f, 1.0f, 1.0f},
			.tex_coord = {vertex->uv.raw[0], vertex->uv.raw[1]},
		};

		arrpush(data->draw_vertices, sdl_vertex);
	}
}

void MeshData_SetNewCameraDistance(MeshData *data, float camera_distance) {
	float ratio = camera_distance / data->camera_distance;
	data->actual_camera_distance = camera_distance;

	if(ratio < 1.5f && ratio > 0.66f) {
		return;
	}

	if(ratio > 1.5f) {
		data->camera_distance = 1.5f * data->camera_distance;
	} else {
		data->camera_distance = data->camera_distance * 2 / 3;
	}

	MeshData_BuildFullMesh(data);
}

void MeshData_Destroy(MeshData *data) {
	SDL_DestroyTexture(data->texture);
	arrfree(data->raw_vertices);
	arrfree(data->draw_vertices);
	TCG_FreeData(&data->cg);
}
