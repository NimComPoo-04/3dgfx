#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "painter.h"

void painter_clear(painter_t *p)
{
	memset(p->buffer, 0, p->width * p->height * sizeof(uint32_t));
	for(int i = 0; i < p->width * p->height; i++)
		p->depth[i] = 1.f;
}

static inline Vertex vertex_interpolate(Vertex start, Vertex end, float t)
{
	Vertex out = {0};
	out.pos = Vec3_Interpolate(start.pos, end.pos, t);
	out.col = Vec4_Interpolate(start.col, end.col, t);
	return out;
}

static inline void put_pixel(painter_t *p, int x, int y, float z, uint32_t color)
{
	if(y < 0 || y >= p->height || x < 0 || x >= p->width)
		return;

	if(p->depth[y * p->width + x] <= z)
		return;

	p->buffer[y * p->width + x] = color;
	p->depth[y * p->width + x] = z;
}

void painter_point(painter_t *p, Vertex k)
{
	Vec3 position = Vec3_PerspectiveProject(p->projection, k.pos);
	uint32_t color = Color_Convert(k.col);

	int x = (position.x + 1) / 2 * p->width;
	int y = (position.y + 1) / 2 * p->height;

	put_pixel(p, x, y, position.z, color);
}

void draw_line(painter_t *p, float fx1, float fy1, float fz1,
		             float fx2, float fy2, float fz2,
			     Vec4 col1, Vec4 col2)
{
	int x1 = (fx1 + 1) / 2 * p->width;
	int y1 = (fy1 + 1) / 2 * p->height;

	int x2 = (fx2 + 1) / 2 * p->width;
	int y2 = (fy2 + 1) / 2 * p->height;

	if(abs(y2 - y1) < abs(x2 - x1))
	{
		if(x1 > x2)
		{
			SWAP(y1, y2, int);
			SWAP(x1, x2, int);
			SWAP(fz1, fz2, float);

			SWAP(col1, col2, Vec4);
		}

		int dx = x2 - x1;
		int dy = y2 - y1;

		if(dx == 0) return;

		int x = x1;

		while(x <= x2)
		{
			int y = (x - x1) * dy/dx + y1;

			float t = 1. * (x - x1) / (x2 - x1);
			uint32_t color = Color_Convert(Vec4_Interpolate(col1, col2, t));

			float z = fz1 * (1 - t) + fz2 * t;

			put_pixel(p, x, y, z, color);
			x += 1;
		}
	}
	else
	{
		if(y1 > y2)
		{
			SWAP(y1, y2, int);
			SWAP(x1, x2, int);
			SWAP(fz1, fz2, float);

			SWAP(col1, col2, Vec4);
		}

		int dx = x2 - x1;
		int dy = y2 - y1;

		int y = y1;

		if(dy == 0) return;

		while(y <= y2)
		{
			int x = (y - y1) * dx/dy + x1;

			float t = 1. * (y - y1) / (y2 - y1);
			uint32_t color = Color_Convert(Vec4_Interpolate(col1, col2, t));

			float z = fz1 * (1 - t) + fz2 * t;

			put_pixel(p, x, y, z, color);
			y += 1;
		}
	}
}

void painter_line(painter_t *p, Vertex a, Vertex b)
{
	Vec3 v1 = Vec3_PerspectiveProject(p->projection, a.pos);
	Vec3 v2 = Vec3_PerspectiveProject(p->projection, b.pos);

	draw_line(p, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, a.col, b.col);
}

// either have 2 which means actual clipping
// or 1 which means no clipping
// or 0 which means the whole triangle is thrown out

// FIXME: clipping does not subdevide triangle rn, need to do that.

int clip_triangle(Vec3 norm, Vec3 pos, Vertex a, Vertex b, Vertex c, Vertex *clps)
{
	float da = Vec3_PlanePointDist(norm, pos, a.pos);
	float db = Vec3_PlanePointDist(norm, pos, b.pos);
	float dc = Vec3_PlanePointDist(norm, pos, c.pos);

	int i = 0;

	// completely outside
	if(da < 0 && db < 0 && dc < 0)
		return 0;

	// completely inside
	else if(da >= -0 && db >= 0 && dc >= 0)
	{
		clps[i++] = a;
		clps[i++] = b;
		clps[i++] = c;
	}

	// A line is outside
	else if(da < 0 && db < 0)
	{
		// Point and A and B are out
		Vertex na = vertex_interpolate(a, c, CLAMP(Vec3_ClipInterp(norm, pos, a.pos, c.pos), 0, 1));
		Vertex nb = vertex_interpolate(b, c, CLAMP(Vec3_ClipInterp(norm, pos, b.pos, c.pos), 0, 1));

		clps[i++] = na;
		clps[i++] = nb;
		clps[i++] = c;
	}

	else if(da < 0 && dc < 0)
	{
		// Point and A and C are out
		Vertex na = vertex_interpolate(a, b, CLAMP(Vec3_ClipInterp(norm, pos, a.pos, b.pos), 0, 1));
		Vertex nc = vertex_interpolate(c, b, CLAMP(Vec3_ClipInterp(norm, pos, c.pos, b.pos), 0, 1));

		clps[i++] = na;
		clps[i++] = b;
		clps[i++] = nc;
	}
	else if(db < 0 && dc < 0)
	{
		// Point and A and B are out
		Vertex nb = vertex_interpolate(b, a, CLAMP(Vec3_ClipInterp(norm, pos, b.pos, a.pos), 0, 1));
		Vertex nc = vertex_interpolate(c, a, CLAMP(Vec3_ClipInterp(norm, pos, c.pos, a.pos), 0, 1));

		clps[i++] = a;
		clps[i++] = nb;
		clps[i++] = nc;
	}

	// A single point is outside
	else if(da < 0)
	{
		Vertex nab = vertex_interpolate(a, b, CLAMP(Vec3_ClipInterp(norm, pos, a.pos, b.pos), 0, 1));
		Vertex nac = vertex_interpolate(a, c, CLAMP(Vec3_ClipInterp(norm, pos, a.pos, c.pos), 0, 1));

		clps[i++] = nab;
		clps[i++] = b;
		clps[i++] = c;

		clps[i++] = nab;
		clps[i++] = c;
		clps[i++] = nac;
	}

	else if(db < 0)
	{
		Vertex nba = vertex_interpolate(b, a, CLAMP(Vec3_ClipInterp(norm, pos, b.pos, a.pos), 0, 1));
		Vertex nbc = vertex_interpolate(b, c, CLAMP(Vec3_ClipInterp(norm, pos, b.pos, c.pos), 0, 1));

		clps[i++] = a;
		clps[i++] = nba;
		clps[i++] = c;

		clps[i++] = nba;
		clps[i++] = nbc;
		clps[i++] = c;
	}

	else if(dc < 0)
	{
		Vertex nca = vertex_interpolate(c, a, CLAMP(Vec3_ClipInterp(norm, pos, c.pos, a.pos), 0, 1));
		Vertex ncb = vertex_interpolate(c, b, CLAMP(Vec3_ClipInterp(norm, pos, c.pos, b.pos), 0, 1));

		clps[i++] = a;
		clps[i++] = b;
		clps[i++] = nca;

		clps[i++] = nca;
		clps[i++] = b;
		clps[i++] = ncb;
	}

	// Any one of the above case should happen
	return i;
}

void painter_triangle(painter_t *p, Vertex a, Vertex b, Vertex c)
{
	// fprintf(stderr, "%lf\n", a.pos.z);

	//FIXME: Do proper normal mapping stuff man
	Vec3 dv12 = Vec3_Sub(a.pos, b.pos);
	Vec3 dv32 = Vec3_Sub(c.pos, b.pos);

	Vec3 norm = Vec3_Cross(VEC3(dv12.x, dv12.y, dv12.z), VEC3(dv32.x, dv32.y, dv32.z));
	Vec3 light = Vec3_Sub(VEC3(0, 0, 0), VEC3(20, -20, 20));

	// Camera transform
	a.pos = Vec3_Transform(p->camera, a.pos);
	b.pos = Vec3_Transform(p->camera, b.pos);
	c.pos = Vec3_Transform(p->camera, c.pos);

	// Back-face culling bois
	dv12 = Vec3_Sub(a.pos, b.pos);
	dv32 = Vec3_Sub(c.pos, b.pos);
	float shade = Vec3_Dot(Vec3_Cross(VEC3(dv12.x, dv12.y, dv12.z), VEC3(dv32.x, dv32.y, dv32.z)), VEC3(0, 0, -1));

	if(shade <= -0.0006) return; // FIXME: idk why it does not work with zero but whatever man

	// Clip triangles against 4 planes
	
	static Vertex clipped[3 * 64] = {0}; int clipped_count = 0;
	static Vertex scratch[3 * 64] = {0}; int scratch_count = 0;

	float angle = p->fov/2;
	float cs = cos(angle);
	float ss = sin(angle);

	struct {Vec3 norm; Vec3 pos;}
	clipping_planes[] = {
		{ {{0, 0, -1}}, {{0, 0, p->far}} },
		{ {{0, 0, 1}}, {{0, 0, p->near}} },

		{ {{-cs, 0, ss}}, {{-0.01, 0, 0}} },
		{ {{cs, 0, ss}}, {{0.01, 0, 0}} },

		{ {{0, -cs, ss}}, {{0, -0.01, 0}} },
		{ {{0, cs, ss}}, {{0, 0.01, 0}} },
	};

	clipped[clipped_count++] = a;
	clipped[clipped_count++] = b;
	clipped[clipped_count++] = c;

	for(int i = 0; i < sizeof(clipping_planes)/sizeof(clipping_planes[0]); i++)
	{
		scratch_count = 0;
		for(int j = 0; j < clipped_count; j+=3)
		{
			scratch_count += clip_triangle(clipping_planes[i].norm, clipping_planes[i].pos,
					clipped[j + 0], clipped[j + 1], clipped[j + 2], scratch + scratch_count);
		}

		if(scratch_count == 0)
			return;

		clipped_count = scratch_count;
		memcpy(clipped, scratch, clipped_count * sizeof(Vertex));
	}

	if(clipped_count == 0) return;

	// Interpolate like this lol
	for(int i = 0; i < clipped_count; i += 3)
	{
		a = clipped[i + 0];
		b = clipped[i + 1];
		c = clipped[i + 2];

		a.pos = Vec3_PerspectiveProject(p->projection, a.pos);
		b.pos = Vec3_PerspectiveProject(p->projection, b.pos);
		c.pos = Vec3_PerspectiveProject(p->projection, c.pos);

		if(p->fill_mode == FILL)
		{
			int x1 = (a.pos.x + 1)/2 * p->width; int y1 = (a.pos.y + 1)/2 * p->height;
			int x2 = (b.pos.x + 1)/2 * p->width; int y2 = (b.pos.y + 1)/2 * p->height;
			int x3 = (c.pos.x + 1)/2 * p->width; int y3 = (c.pos.y + 1)/2 * p->height;

			if(y1 > y2) { SWAP(y1, y2, int); SWAP(x1, x2, int); SWAP(a, b, Vertex); }
			if(y1 > y3) { SWAP(y1, y3, int); SWAP(x1, x3, int); SWAP(a, c, Vertex); }
			if(y2 > y3) { SWAP(y2, y3, int); SWAP(x2, x3, int); SWAP(b, c, Vertex); }

			if(y1 == y3) continue;

			int x4 = ((y2 - y1) * (x3 - x1) / (y3 - y1)) + x1;
			int y4 = y2;

			int start_x = x1;
			int end_x = x1;

			float d = (y2 - y3) * (x1 - x3) - (x2 - x3) * (y1 - y3);

			// barycentric cordinates used for interpolation
			for(int y = y1; y < y2; y++)
			{
				start_x = (y - y1) * (x3 - x1) / (y3 - y1) + x1;
				end_x   = (y - y1) * (x2 - x1) / (y2 - y1) + x1;

				if(start_x > end_x)
					SWAP(start_x, end_x, int);

				for(int x = start_x; x <= end_x; x++)
				{
					float t1 = CLAMP(((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / d, 0, 1);
					float t2 = CLAMP(((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / d, 0, 1);
					float t3 = CLAMP(1. - t1 - t2, 0, 1);

					float z = t1 * a.pos.z + t2 * b.pos.z + t3 * c.pos.z;

					Vec4 col = Vec4_Add(Vec4_Add(Vec4_Scale(a.col, t1), Vec4_Scale(b.col, t2)), Vec4_Scale(c.col, t3));
					//Vec3 norm = Vec3_Add(Vec3_Add(Vec3_Scale(a.norm, t1), Vec3_Scale(b.norm, t2)), Vec3_Scale(c.norm, t3));

					float shaden = 0;

					shaden += CLAMP(Vec3_Dot(norm, VEC3(10, 20, 0)), 0, 1);
					shaden += CLAMP(Vec3_Dot(norm, VEC3(0, -20, 0)), 0, 1);

					shaden += CLAMP(Vec3_Dot(norm, VEC3(10, 0, 0)), 0, 1);
					shaden += CLAMP(Vec3_Dot(norm, VEC3(-10, 0, 0)), 0, 1);

					shaden += CLAMP(Vec3_Dot(norm, VEC3(0, 0, 10)), 0, 1);
					shaden += CLAMP(Vec3_Dot(norm, VEC3(10, 0, -10)), 0, 1);

					uint32_t color = Color_Convert(Vec4_Add(VEC4(0.275, 0.035, 0.549, 1), Vec4_Scale(col, fabsf(shaden))));

					put_pixel(p, x, y, z, color);
				}
			}

			for(int y = y2; y < y3; y++)
			{
				start_x = (y - y2) * (x4 - x3) / (y4 - y3) + x4;
				end_x   = (y - y2) * (x2 - x3) / (y2 - y3) + x2;

				if(start_x > end_x)
					SWAP(start_x, end_x, int);


				for(int x = start_x; x <= end_x; x++)
				{
					float t1 = CLAMP(((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / d, 0, 1);
					float t2 = CLAMP(((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / d, 0, 1);
					float t3 = CLAMP(1. - t1 - t2, 0, 1);

					float z = t1 * a.pos.z + t2 * b.pos.z + t3 * c.pos.z;

					Vec4 col = Vec4_Add(Vec4_Add(Vec4_Scale(a.col, t1), Vec4_Scale(b.col, t2)), Vec4_Scale(c.col, t3));
					//Vec3 norm = Vec3_Add(Vec3_Add(Vec3_Scale(a.norm, t1), Vec3_Scale(b.norm, t2)), Vec3_Scale(c.norm, t3));

					float shaden = 0;

					shaden += CLAMP(Vec3_Dot(norm, VEC3(10, 20, 0)), 0, 1);
					shaden += CLAMP(Vec3_Dot(norm, VEC3(0, -20, 0)), 0, 1);

					shaden += CLAMP(Vec3_Dot(norm, VEC3(10, 0, 0)), 0, 1);
					shaden += CLAMP(Vec3_Dot(norm, VEC3(-10, 0, 0)), 0, 1);

					shaden += CLAMP(Vec3_Dot(norm, VEC3(0, 0, 10)), 0, 1);
					shaden += CLAMP(Vec3_Dot(norm, VEC3(10, 0, -10)), 0, 1);

					uint32_t color = Color_Convert(Vec4_Add(VEC4(0.275, 0.035, 0.549, 1), Vec4_Scale(col, fabsf(shaden))));

					put_pixel(p, x, y, z, color);
				}
			}
		}
		else if(p->fill_mode == WIREFRAME)
		{
			Vec4 WHITE = {{ 1., 1., 1., 1. }};

			(a.pos.z + 1) / 2;

			// We set the z value to -3 so that they are drawn over absolutely
			draw_line(p, a.pos.x, a.pos.y, -3, b.pos.x, b.pos.y, -3, WHITE, WHITE);
			draw_line(p, a.pos.x, a.pos.y, -3, c.pos.x, c.pos.y, -3, WHITE, WHITE);
			draw_line(p, b.pos.x, b.pos.y, -3, c.pos.x, c.pos.y, -3, WHITE, WHITE);
		}

	}
}

void painter_triangles_indexed(painter_t *p, Vertex *list,
		int *indx, int indx_count)
{
	for(int i = 0; i < indx_count; i += 3)
	{
		Vertex a = list[indx[i + 0]];
		Vertex b = list[indx[i + 1]];
		Vertex c = list[indx[i + 2]];

		painter_triangle(p, a, b, c);
	}
}
