#ifndef _PAINTER_H_
#define _PAINTER_H_

#include <stdint.h>
#include "vector.h"

/*
 * TODO:
 * 	1) camera transforms		[X]
 * 	2) model transforms		[X]
 *
 *  	1) depth buffering		[x]
 * 	2) normals interpolated for lighting
 * 	3) phong shaidn
 */

typedef struct
{
	uint32_t *buffer;
	float *depth;
	int width;
	int height;

	Mat4x4 camera;

	float far;
	float near;
	float fov;

	Mat4x4 projection;

	enum {
		WIREFRAME,
		FILL
	} fill_mode;
} painter_t;

typedef struct Vertex
{
	Vec3 pos;	// position data
	Vec4 col;	// color data
	Vec3 norm;	// normal data
} Vertex;

void painter_clear(painter_t *p);
void painter_point(painter_t *p, Vertex v);
void painter_line(painter_t *p, Vertex a, Vertex b);
void painter_triangle(painter_t *p, Vertex a, Vertex b, Vertex c);

void painter_triangles_indexed(painter_t *p, Vertex *list,
		int *indx, int indx_count);

#endif
