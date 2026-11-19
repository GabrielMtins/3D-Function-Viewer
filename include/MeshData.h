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

	TCG_Mat4 projection;
} MeshData;

void MeshData_Init(MeshData *data);

void MeshData_ComputeVertices(MeshData *data, const char *str);

void MeshData_ConvertToDraw(MeshData *data);

#endif
