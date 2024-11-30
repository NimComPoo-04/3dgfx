#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "model.h"

Model Model_Load(const char *obj_file)
{
	FILE *f = fopen(obj_file, "rb+");
	char garbage[1024] = {0};

	Model mod = {0};

	int norm_count = 0;

	while(!feof(f))
	{
		char opt[5] = {0};

		float x = 0;
		float y = 0;
		float z = 0;

		fscanf(f, "%s", opt);

		if(opt[0] == 'v' && opt[1] == 0)
		{
			fscanf(f, "%f %f %f", &x, &y, &z);

			// Vertex
			Vertex v;
			v.pos.x = x; v.pos.y = y; v.pos.z = z;

			v.col.x = (sinf(v.pos.x) + 1)/2;
			v.col.y = (cosf(v.pos.y) + 1)/2;
			v.col.z = (sinf(v.pos.z) + 1)/2;
			v.col.w = 1.0;

			if(mod.count_vertices % 16 == 0)
			{
				mod.vertices = realloc(mod.vertices, sizeof(Vertex) * (mod.count_vertices + 16));
			}
			mod.vertices[mod.count_vertices++] = v;
		}

		if(opt[0] == 'v' && opt[1] == 'n' && opt[2] == 0)
		{
			fscanf(f, "%f %f %f", &x, &y, &z);

			mod.vertices[norm_count].norm.x = x;
			mod.vertices[norm_count].norm.y = y;
			mod.vertices[norm_count].norm.z = z;
			norm_count++;
		}

		else if(opt[0] == 'f' && opt[1] == 0)
		{
			fscanf(f, "%f %f %f", &x, &y, &z);

			// Face
			if(mod.count_faces % (3 * 16) == 0)
			{
				mod.faces = realloc(mod.faces, sizeof(int) * (mod.count_faces + 3 * 16));
			}

			mod.faces[mod.count_faces++] = (int)x;
			mod.faces[mod.count_faces++] = (int)y;
			mod.faces[mod.count_faces++] = (int)z;
		}

		fscanf(f, "%[^\n]", garbage);
	}

	fclose(f);

	return mod;
}

void  Model_Dump(Model *m)
{
	printf("Verteces:\n");
	for(int i = 0; i < m->count_vertices; i++)
		printf("%f %f %f\n", m->vertices[i].pos.x, m->vertices[i].pos.y, m->vertices[i].pos.z);

	printf("Normal:\n");
	for(int i = 0; i < m->count_vertices; i++)
		printf("%f %f %f\n", m->vertices[i].norm.x, m->vertices[i].norm.y, m->vertices[i].norm.z);

	printf("Faces:\n");
	for(int i = 0; i < m->count_faces; i += 3)
		printf("%d %d %d\n", m->faces[i], m->faces[i+1], m->faces[i+2]);
	puts("");
}

void  Model_Draw(painter_t *p, Model *m, Mat4x4 mod)
{
	for(int i = 0; i < m->count_faces; i += 3)
	{
		Vertex a = m->vertices[m->faces[i + 0] - 1];
		Vertex b = m->vertices[m->faces[i + 1] - 1];
		Vertex c = m->vertices[m->faces[i + 2] - 1];

		a.pos = Vec3_Transform(mod, a.pos);
		b.pos = Vec3_Transform(mod, b.pos);
		c.pos = Vec3_Transform(mod, c.pos);

		painter_triangle(p, a, b, c);
	}
}
