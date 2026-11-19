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

#ifndef MESHFUNC_H
#define MESHFUNC_H

#include "Global.h"
#include <TinyCG.h>
#include <MParse.h>

typedef struct MeshData {
	TCG_Data cg;
	MP_Expr expr;

	VertexVector raw_vertices;
	SdlVertexVector draw_vertices;

	vec3s light_dir;
	vec3s rotations;
	float camera_distance;
	float actual_camera_distance;
	float graph_range;

	TCG_Mat4 projection;

	SDL_Texture *texture;
} MeshData;

void MeshData_Init(MeshData *data, SDL_Renderer *renderer);

void MeshData_ComputeVertices(MeshData *data, const char *str);

void MeshData_ConvertToDraw(MeshData *data);

void MeshData_SetNewCameraDistance(MeshData *data, float camera_distance);

void MeshData_Destroy(MeshData *data);

#endif
