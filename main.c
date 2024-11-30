#include <stdio.h>
#include <stdlib.h>

#include "vector.h"
#include "bitmap.h"
#include "painter.h"

#include "model.h"

uint32_t buffer[WIN_WIDTH * WIN_HEIGHT] = {0};
float    depth [WIN_WIDTH * WIN_HEIGHT] = {0};

int main(void)
{
	float ang = -PI/2.3;
	float scl = 4;

	Vec3 origin = {{scl * cosf(ang), scl/4, scl * sinf(ang)}};
	Vec3 lookat = Vec3_Scale(origin, -1);

	painter_t pnt = {
		buffer, depth,
		WIN_WIDTH, WIN_HEIGHT,
		Mat4x4_CameraTransform(origin, lookat),
		10, 1, PI/2,
		Mat4x4_PerspectiveProject(PI/2, 1, 10),
		FILL
	};

	painter_clear(&pnt);

	Model m = Model_Load("mesh.obj");
	Model_Draw(&pnt, &m, Mat4x4_Identity());

	bitmap_header_t bmp = BITMAP_HEADER_DEFAULT;
	bitmap_write(&bmp, buffer, WIN_WIDTH, WIN_HEIGHT, "test.bmp");

	return 0;
}

