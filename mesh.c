#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mesh.h"

static void insert_data(array_t *m, void *data)
{
	if(m->count % 16 == 0)
		m->data = realloc(m->data, (m->count + 16) * m->type);

	memcpy((char *)m->data + m->count * m->type, data, m->type);
	m->count++;
}

Mesh Mesh_Load(const char *fname)
{
	Mesh m = {0};

	m.vposes.type = sizeof(Vec3);
	m.norms.type = sizeof(Vec3);
	m.faces.type = sizeof(face_t);

	FILE *f = fopen(fname, "rb+");

	char buffer[1024] = {0};
	char *token = NULL;

	while(!feof(f))
	{
		fgets(buffer, sizeof buffer, f);
		if(feof(f)) break;

		token = strtok(buffer, " \n\r");

		while(token)
		{
			if(strcmp(token, "v") == 0)
			{
				Vec3 v = {0};

				for(int j = 0; j < 3; j++)
				{
					token = strtok(NULL, " ");
					v.it[j] = strtof(token, NULL);
				}

				insert_data(&m.vposes, &v);

			}
			else if(strcmp(token, "vn") == 0)
			{
				Vec3 v = {0};

				for(int j = 0; j < 3; j++)
				{
					token = strtok(NULL, " ");
					v.it[j] = strtof(token, NULL);
				}

				insert_data(&m.norms, &v);
			}
			else if(strcmp(token, "f") == 0)
			{
				face_t v = {0};

				for(int k = 0; k < 3; k++)
				{
					token = strtok(NULL, " /\n\r");
					if(token)
						v.vpos[k] = (short)strtol(token, NULL, 10) - 1;

					token = strtok(NULL, " /\n\r");
					if(token)
						v.norm[k] = (short)strtol(token, NULL, 10) - 1;
				}

				insert_data(&m.faces, &v);
			}
			else
			{
				break;
			}
		}
	}

	fclose(f);

	return m;
}

void Mesh_Draw(painter_t *p, Mesh m, Mat4x4 mod)
{
	for(int i = 0; i < m.faces.count; i++)
	{
		Vertex a = {0}, b = {0}, c = {0};

		face_t f = ((face_t *)(m.faces.data))[i];

		Vec3 *vpos = ((Vec3 *)m.vposes.data);
		Vec3 *norm = ((Vec3 *)m.norms.data);

		a.pos = Vec3_Transform(mod, vpos[f.vpos[0]]);
		b.pos = Vec3_Transform(mod, vpos[f.vpos[1]]);
		c.pos = Vec3_Transform(mod, vpos[f.vpos[2]]);

		if(norm)
		{
			a.norm = norm[f.norm[0]];
			b.norm = norm[f.norm[1]];
			c.norm = norm[f.norm[2]];
		}
		else
		{
			Vec3 dv12 = Vec3_Sub(a.pos, b.pos);
			Vec3 dv32 = Vec3_Sub(c.pos, b.pos);
			Vec3 norm = Vec3_Normalize(Vec3_Cross(VEC3(dv12.x, dv12.y, dv12.z), VEC3(dv32.x, dv32.y, dv32.z)));
			a.norm = b.norm = c.norm = norm;
		}

		a.col = VEC4(1, 1, 1, 1); //VEC4(0.29, 0.3, 0.56, 1);
		b.col = VEC4(1, 1, 1, 1); //VEC4(0.29, 0.3, 0.56, 1);
		c.col = VEC4(1, 1, 1, 1); //VEC4(0.29, 0.3, 0.56, 1);

		painter_triangle(p, a, b, c);
	}
}

