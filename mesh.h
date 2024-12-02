#ifndef _MESH_H_
#define _MESH_H_

#include "vector.h"
#include "painter.h"

typedef struct
{
	short vpos[3];
	short norm[3];
} face_t;

typedef struct
{
	void *data;
	int count;
	size_t type;
} array_t;

typedef struct
{
	array_t vposes;
	array_t norms;
	array_t faces;
} Mesh;

Mesh Mesh_Load(const char *fname);
Mesh Mesh_TestCube();
void Mesh_Draw(painter_t *p, Mesh m, Mat4x4 mod);

#endif
