#ifndef _MODEL_H_
#define _MODEL_H_

#include "vector.h"
#include "painter.h"

typedef struct
{
	Vertex *vertices;
	int *faces;

	int count_vertices;
	int count_faces;	
} Model;

Model Model_Load(const char *obj_file);
void  Model_Dump(Model *m);
void  Model_Draw(painter_t *p, Model *m, Mat4x4 mod);

#endif
