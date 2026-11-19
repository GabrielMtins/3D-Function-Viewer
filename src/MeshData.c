#include "MeshData.h"

static bool MeshData_Compile(MeshData *data, const char *str);
static float MeshData_EvalHeight(MeshData *data, float x, float z);
static vec3s MeshData_EvalPoint(MeshData *data, float x, float z);
static vec3s MeshData_EvalNormal(MeshData *data, float x, float z);
static float MeshData_EvalColor(MeshData *data, float x, float z);
static void MeshData_BuildQuad(MeshData *data, float x, float z, float step);
static void MeshData_BuildFullMesh(MeshData *data);
static void MeshData_ApplyTransformations(MeshData *data);

static bool MeshData_Compile(MeshData *data, const char *str) {
	int error;

	if(MP_IsValid(&data->expr)) {
		MP_Free(&data->expr);
	}

	data->expr = MP_Compile(str, (const char *[]){"x", "y"}, 2, &error);

	return error == 0;
}

static float MeshData_EvalHeight(MeshData *data, float x, float z) {
	return MP_Eval(&data->expr, (const float[]){x, z});
}

static vec3s MeshData_EvalPoint(MeshData *data, float x, float z) {
	return (vec3s) {
		x,
		MeshData_EvalHeight(data, x, z),
		z,
	};
}

static vec3s MeshData_EvalNormal(MeshData *data, float x, float z) {
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

static float MeshData_EvalColor(MeshData *data, float x, float z) {
	vec3s normal = MeshData_EvalNormal(data, x, z);

	return glms_dot(data->light_dir, normal) * 0.5f + 0.5f;
}

static void MeshData_BuildQuad(MeshData *data, float x, float z, float step) {
	Vertex v[4];

	for(int i = 0; i < 4; i++) {
		v[i] = (Vertex) {
			.pos = MeshData_EvalPoint(data, x + step * (i % 2), z + step * (i / 2)),
			.color = MeshData_EvalColor(data, x + step * (i % 2), z + step * (i / 2)),
		};
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

	for(float z = -graph_range; z < graph_range + step * 0.1f; z += step) {
		for(float x = -graph_range; x < graph_range + step * 0.1f; x += step) {
			MeshData_BuildQuad(data, x, z, step);
		}
	}
}

static void MeshData_ApplyTransformations(MeshData *data) {
	data->cg.len = 0;
	TCG_AppendData(&data->cg, data->raw_vertices, arrlen(data->raw_vertices));

	TCG_Mat4 res, op;

	TCG_Mat4Identity(res);
	TCG_Mat4Scale(op, (TCG_Vec3){1.0f, 1.0f, -1.0f});
	TCG_Mat4Mul(op, res);
	TCG_Mat4RotateY(op, data->rotations.raw[1]);
	TCG_Mat4Mul(op, res);
	TCG_Mat4RotateX(op, data->rotations.raw[0]);
	TCG_Mat4Mul(op, res);
	TCG_Mat4Translate(op, (TCG_Vec3){0.0f, 0.0f, -data->camera_distance});
	TCG_Mat4Mul(op, res);
	TCG_Mat4Mul(data->projection, res);

	TCG_Transform(&data->cg, res);
	TCG_SmartClipping(&data->cg);
	TCG_ApplyProjection(&data->cg);
	TCG_ScaleToScreenSize(&data->cg, view3d.w, view3d.h);
	TCG_SortByDepth(&data->cg);
}

void MeshData_Init(MeshData *data) {
	data->cg = TCG_BaseData(
			TCG_TRIANGLE,
			sizeof(Vertex) / sizeof(float),
			0
			);

	data->light_dir = glms_normalize((vec3s){1.0f, 1.0f, 1.0f});

	TCG_Mat4Perspective(
			data->projection,
			TCG_Radians(45.0f),
			view3d.w / view3d.h,
			0.01f,
			100.0f
			);

	data->camera_distance = 8.0f;
	data->rotations.raw[0] = TCG_Radians(35.0f);
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
			.color = {vertex->color, vertex->color, vertex->color, 1.0f},
		};

		arrpush(data->draw_vertices, sdl_vertex);
	}
}
