#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <math.h>
#include <stdint.h>

#define PI 3.14159

#define SWAP(x, y, type) do { type __tmp = x; x = y; y = __tmp; } while(0)
#define CLAMP(a, b, c) ((a) < (b) ? (b) : ((a) > (c) ? (c) : (a)))

#define VEC2(x, y) (Vec2){{x, y}}
#define VEC3(x, y, z) (Vec3){{x, y, z}}
#define VEC4(x, y, z, w) (Vec4){{x, y, z, w}}

typedef union Vec2 { struct { float x; float y; };                   float it[2]; } Vec2;
typedef union Vec3 { struct { float x; float y; float z; };          float it[3]; } Vec3;
typedef union Vec4 { struct { float x; float y; float z; float w; }; float it[4]; } Vec4;

static inline Vec2 Vec2_Add(Vec2 a, Vec2 b) { a.x += b.x; a.y += b.y; return a; }
static inline Vec3 Vec3_Add(Vec3 a, Vec3 b) { a.x += b.x; a.y += b.y; a.z += b.z; return a; }
static inline Vec4 Vec4_Add(Vec4 a, Vec4 b) { a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w; return a; }

static inline Vec2 Vec2_Sub(Vec2 a, Vec2 b) { a.x -= b.x; a.y -= b.y; return a; }
static inline Vec3 Vec3_Sub(Vec3 a, Vec3 b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; return a; }
static inline Vec4 Vec4_Sub(Vec4 a, Vec4 b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w; return a; }

static inline Vec2 Vec2_Mul(Vec2 a, Vec2 b) { a.x *= b.x; a.y *= b.y; return a; }
static inline Vec3 Vec3_Mul(Vec3 a, Vec3 b) { a.x *= b.x; a.y *= b.y; a.z *= b.z; return a; }
static inline Vec4 Vec4_Mul(Vec4 a, Vec4 b) { a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w; return a; }

static inline Vec2 Vec2_Div(Vec2 a, Vec2 b) { a.x /= b.x; a.y /= b.y; return a; }
static inline Vec3 Vec3_Div(Vec3 a, Vec3 b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; return a; }
static inline Vec4 Vec4_Div(Vec4 a, Vec4 b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; a.w /= b.w; return a; }

static inline Vec2 Vec2_Scale(Vec2 a, float b) { a.x *= b; a.y *= b; return a; }
static inline Vec3 Vec3_Scale(Vec3 a, float b) { a.x *= b; a.y *= b; a.z *= b; return a; }
static inline Vec4 Vec4_Scale(Vec4 a, float b) { a.x *= b; a.y *= b; a.z *= b; a.w *= b; return a; }

static inline float Vec2_Dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
static inline float Vec3_Dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static inline float Vec4_Dot(Vec4 a, Vec4 b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

static inline float Vec3_Len2(Vec3 a) { return Vec3_Dot(a, a); }

static inline Vec3 Vec3_Cross(Vec3 a, Vec3 b)
{
	Vec3 c = {0};

	c.x = a.y * b.z - a.z * b.y;
	c.y = a.z * b.x - a.x * b.z;
	c.z = a.x * b.y - a.y * b.x;

	return c;
}

static inline float Vec2_Cross(Vec2 a, Vec2 b)
{
	return a.y * b.y - a.y * b.y;
}

typedef struct Mat4x4 { float it[4][4]; } Mat4x4;

static inline Mat4x4 Mat4x4_Identity() {
	Mat4x4 m = {0};
	m.it[0][0] = 1;
	m.it[1][1] = 1;
	m.it[2][2] = 1;
	m.it[3][3] = 1;
	return m;
}

static inline Vec4 Vec4_Transform(Mat4x4 m, Vec4 v)
{
	Vec4 a = {0};

	a.x = m.it[0][0] * v.it[0] + m.it[0][1] * v.it[1] +  m.it[0][2] * v.it[2] + m.it[0][3] * v.it[3];
	a.y = m.it[1][0] * v.it[0] + m.it[1][1] * v.it[1] +  m.it[1][2] * v.it[2] + m.it[1][3] * v.it[3];
	a.z = m.it[2][0] * v.it[0] + m.it[2][1] * v.it[1] +  m.it[2][2] * v.it[2] + m.it[2][3] * v.it[3];
	a.w = m.it[3][0] * v.it[0] + m.it[3][1] * v.it[1] +  m.it[3][2] * v.it[2] + m.it[3][3] * v.it[3];

	return a;
}

static inline Vec3 Vec3_Transform(Mat4x4 m, Vec3 v)
{
	Vec4 v4 = {{ v.x, v.y, v.z, 1 }};
	Vec4 tv = Vec4_Transform(m, v4);
	return VEC3(tv.x, tv.y, tv.z);
}

static inline Vec3 Vec3_PerspectiveProject(Mat4x4 prj, Vec3 v)
{
	Vec4 d = {{v.x, v.y, v.z, 1}};
	d = Vec4_Transform(prj, d);
	d = Vec4_Scale(d, 1/d.w);
	return VEC3(d.x, d.y, d.z);
}

static inline uint32_t Color_Convert(Vec4 col)
{
	uint32_t color = 0;

	color |= (int)(CLAMP(col.x, 0, 1) * 255) << 16;
	color |= (int)(CLAMP(col.y, 0, 1) * 255) << 8;
	color |= (int)(CLAMP(col.z, 0, 1) * 255);
	color |= (int)(CLAMP(col.w, 0, 1) * 255) << 24;

	return color;
}

static inline Vec3 Vec3_Interpolate(Vec3 start, Vec3 end, float t)
{
	Vec3 out;

	out.x = start.x * (1 - t) + end.x * t;
	out.y = start.y * (1 - t) + end.y * t;
	out.z = start.z * (1 - t) + end.z * t;

	return out;
}

static inline Vec4 Vec4_Interpolate(Vec4 start, Vec4 end, float t)
{
	Vec4 out;

	out.x = start.x * (1 - t) + end.x * t;
	out.y = start.y * (1 - t) + end.y * t;
	out.z = start.z * (1 - t) + end.z * t;
	out.w = start.w * (1 - t) + end.w * t;

	return out;
}

static inline Mat4x4 Mat4x4_Scale(Vec3 scl)
{
	Mat4x4 m = {0};
	m.it[0][0] = scl.x;
	m.it[1][1] = scl.y;
	m.it[2][2] = scl.z;
	m.it[3][3] = 1;
	return m;
}

static inline Mat4x4 Mat4x4_Rotate(Vec3 u, float t)
{
	Mat4x4 r = {0};

	float c = cosf(t);
	float s = sinf(t);

	r.it[0][0] = u.x * u.x * (1 - c) + c;
	r.it[0][1] = u.x * u.y * (1 - c) - u.z * s;
	r.it[0][2] = u.x * u.z * (1 - c) + u.y * s;

	r.it[1][0] = u.x * u.y * (1 - c) + u.z * s;
	r.it[1][1] = u.y * u.y * (1 - c) + c;
	r.it[1][2] = u.y * u.z * (1 - c) - u.x * s;

	r.it[2][0] = u.x * u.z * (1 - c) - u.y * s;
	r.it[2][1] = u.y * u.z * (1 - c) + u.x * s;
	r.it[2][2] = u.z * u.z * (1 - c) + c;

	return r;
}

static inline Mat4x4 Mat4x4_Translate(Vec3 u)
{
	Mat4x4 r = {0};

	r.it[0][0] = 1;
	r.it[1][1] = 1;
	r.it[2][2] = 1;
	r.it[3][3] = 1;

	r.it[0][3] = u.x;
	r.it[1][3] = u.y;
	r.it[2][3] = u.z;

	return r;
}

static inline Mat4x4 Mat4x4_Mul(Mat4x4 a, Mat4x4 b)
{
	Mat4x4 m = {0};

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			for(int k = 0; k < 4; k++)
			{
				m.it[i][j] += a.it[i][k] * b.it[k][j];
			}
		}
	}

	return m;
}

static inline Mat4x4 Mat4x4_PerspectiveProject(float fov, float znear, float zfar)
{
	/*
	 * A + B/n = -1 => A + B/n + 1 = 0
	 * A + B/f =  1 => A + B/f - 1 = 0
	 * => B/n - B/f + 2 = 0
	 * => B(f - n)/fn = -2
	 * => B = -2 fn/(f - n)
	 * => B = 2fn/(n - f)
	 *
	 * A + 2f/(n - f) = -1
	 * => A = -1 - 2f/(n-f)
	 * => A = -(n+f)/(n-f)
	 */

	float rt = 1 / tanf(fov/2);

	Mat4x4 m = {0};

	m.it[0][0] = rt;
	m.it[1][1] = rt;
	m.it[2][2] = -(znear + zfar) / (znear - zfar);
	m.it[2][3] =  2 * znear * zfar / (znear - zfar);
	m.it[3][2] = 1;

	return m;
}

static inline Vec3 Vec3_DotProject(Vec3 u, Vec3 v)
{
	return Vec3_Scale(u, Vec3_Dot(u, v)/Vec3_Dot(u, u));
}

static inline Vec3 Vec3_Normalize(Vec3 u)
{
	return Vec3_Scale(u, 1.f/sqrtf(Vec3_Len2(u)));
}

static inline Mat4x4 Mat4x4_CameraTransform(Vec3 pos, Vec3 norm)
{
	Vec3 Y = {{0, 1, 0}};

	Vec3 u1 = Vec3_Normalize(norm);
	Vec3 u2 = Y;
	Vec3 u3 = Vec3_Normalize(Vec3_Cross(u1, u2));
	u2 = Vec3_Cross(u3, u1);

	Mat4x4 m = {0};
	m.it[0][0] = u3.x; m.it[0][1] = u3.y; m.it[0][2] = u3.z; m.it[0][3] = -Vec3_Dot(u3, pos);
	m.it[1][0] = u2.x; m.it[1][1] = u2.y; m.it[1][2] = u2.z; m.it[1][3] = -Vec3_Dot(u2, pos);
	m.it[2][0] = u1.x; m.it[2][1] = u1.y; m.it[3][2] = u1.z; m.it[2][3] = -Vec3_Dot(u1, pos);
	m.it[3][3] = 1;

	return m;
}

// Returns the interpolation value for clipping point
static inline float Vec3_ClipInterp(Vec3 norm, Vec3 pos, Vec3 start, Vec3 end)
{
	float D = -Vec3_Dot(norm, pos);
	float P = Vec3_Dot(norm, start);
	float K = Vec3_Dot(norm, Vec3_Sub(end, start));

	return (- D - P) / K;
}

static inline float Vec3_PlanePointDist(Vec3 norm, Vec3 pos, Vec3 pt)
{
	return Vec3_Dot(Vec3_Sub(pt, pos), norm);
}

#endif
