#include <stdio.h>
#include <stdlib.h>

#include "vector.h"
#include "bitmap.h"
#include "painter.h"

#include "mesh.h"

uint32_t buffer[WIN_WIDTH * WIN_HEIGHT] = {0};
float    depth [WIN_WIDTH * WIN_HEIGHT] = {0};

uint32_t display_depth [WIN_WIDTH * WIN_HEIGHT] = {0};

int main(void)
{
	float ang = PI/2;
	float scl = 2;

	Mesh m = Mesh_Load("object_files/sussane.obj");

	painter_t pnt = {0};

	pnt.buffer = buffer;
	pnt.depth = depth;
	pnt.width = WIN_WIDTH;
	pnt.height = WIN_HEIGHT;

	pnt.far  = 100;
	pnt.near = 1;
	pnt.fov  = PI/2;

	pnt.projection = Mat4x4_PerspectiveProject(PI/2, 1, 100);
	pnt.projection.it[0][0] *= 1. * WIN_HEIGHT/WIN_WIDTH;
	pnt.fill_mode = FILL;

	pnt.ambientIntensity = 0.3;
	pnt.ambientColor = VEC4(1, 1, 1, 1);

	pnt.lightPositions[0] = VEC3(0, 1, 0);
	pnt.lightColors[0] = VEC4(1, 0, 0, 1);

	pnt.lightPositions[1] = VEC3(1, 0, 0);
	pnt.lightColors[1] = VEC4(0, 1, 0, 1);

	pnt.lightPositions[2] = VEC3(1/1.414, 0, 1/1.414);
	pnt.lightColors[2] = VEC4(0, 0, 1, 1);

	pnt.lightsCount = 2;
	
#if VIDEO

	float ligtng = 0;
	float height = scl;

	Vec3 X = {{1, 0, 0}};
	Vec3 Z = {{0, 0, -1}};

	for(int i = 0; i < WIN_FRAMES; i++)
	{
		pnt.camPos = VEC3(scl * cosf(ang), height, scl * sinf(ang));
		pnt.lookat = VEC3(0, 0, 0);
		pnt.camera = Mat4x4_CameraTransform(pnt.camPos, Vec3_Sub(pnt.lookat, pnt.camPos));

		painter_clear(&pnt);
		Mesh_Draw(&pnt, m, Mat4x4_Mul(Mat4x4_Rotate(X, ligtng/5), Mat4x4_Rotate(Z, ligtng/7)));

		pnt.lightPositions[0] = VEC3(2 * cosf(ligtng), 0, 2 * sinf(ligtng));
		pnt.lightPositions[1] = VEC3(0, 2 * cosf(ligtng), 2 * sinf(ligtng));
		pnt.lightPositions[2] = VEC3(2 * cosf(ligtng), 2 * sinf(ligtng), 0);

		fwrite(buffer, sizeof buffer, 1, stdout);

		ligtng += 0.03;
		ang += 0.01;

		if(i < WIN_FRAMES >> 2)
			height -= 5 * scl/WIN_FRAMES;
		else
			height += 1.5 * scl/WIN_FRAMES;
	}

#else
	painter_clear(&pnt);
	Mesh_Draw(&pnt, m, Mat4x4_Identity());

	bitmap_header_t bmp = BITMAP_HEADER_DEFAULT;
	bitmap_write(&bmp, buffer, WIN_WIDTH, WIN_HEIGHT, "test.bmp");
#endif

	return 0;
}
