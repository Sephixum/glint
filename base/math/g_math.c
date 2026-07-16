#include "g_math.h"
#include <float.h>

////////////////////////
// f32 variant

internal vec2_f32 vec2_f32_create(f32 x, f32 y)
{
	return (vec2_f32){x, y};
}

internal vec3_f32 vec3_f32_create(f32 x, f32 y, f32 z)
{
	return (vec3_f32){x, y, z};
}

internal vec4_f32 vec4_f32_create(f32 x, f32 y, f32 z, f32 w)
{
	return (vec4_f32){x, y, z, w};
}

internal vec2_f32 vec2_f32_add(vec2_f32 a, vec2_f32 b)
{
	vec2_f32 result = {0};
	result.x	   = a.x + b.x;
	result.y	   = a.y + b.y;
	return result;
}

internal vec3_f32 vec3_f32_add(vec3_f32 a, vec3_f32 b)
{
	vec3_f32 result = {0};
	result.x	   = a.x + b.x;
	result.y	   = a.y + b.y;
	result.z	   = a.z + b.z;
	return result;
}

internal vec4_f32 vec4_f32_add(vec4_f32 a, vec4_f32 b)
{
	vec4_f32 result = {0};
	result.x	   = a.x + b.x;
	result.y	   = a.y + b.y;
	result.z	   = a.z + b.z;
	result.w	   = a.w + b.w;
	return result;
}

internal vec2_f32 vec2_f32_sub(vec2_f32 a, vec2_f32 b)
{
	vec2_f32 result = {0};
	result.x	   = a.x - b.x;
	result.y	   = a.y - b.y;
	return result;
}

internal vec3_f32 vec3_f32_sub(vec3_f32 a, vec3_f32 b)
{
	vec3_f32 result = {0};
	result.x	   = a.x - b.x;
	result.y	   = a.y - b.y;
	result.z	   = a.z - b.z;
	return result;
}

internal vec4_f32 vec4_f32_sub(vec4_f32 a, vec4_f32 b)
{
	vec4_f32 result = {0};
	result.x	   = a.x - b.x;
	result.y	   = a.y - b.y;
	result.z	   = a.z - b.z;
	result.w	   = a.w - b.w;
	return result;
}

internal vec2_f32 vec2_f32_mul(vec2_f32 a, vec2_f32 b)
{
	vec2_f32 result = {0};
	result.x	   = a.x * b.x;
	result.y	   = a.y * b.y;
	return result;
}

internal vec3_f32 vec3_f32_mul(vec3_f32 a, vec3_f32 b)
{
	vec3_f32 result = {0};
	result.x	   = a.x * b.x;
	result.y	   = a.y * b.y;
	result.z	   = a.z * b.z;
	return result;
}

internal vec4_f32 vec4_f32_mul(vec4_f32 a, vec4_f32 b)
{
	vec4_f32 result = {0};
	result.x	   = a.x * b.x;
	result.y	   = a.y * b.y;
	result.z	   = a.z * b.z;
	result.w	   = a.w * b.w;
	return result;
}

internal f32 vec2_f32_dot(vec2_f32 a, vec2_f32 b)
{
	return (a.x * b.x) + (a.y * b.y);
}

internal f32 vec3_f32_dot(vec3_f32 a, vec3_f32 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

internal f32 vec4_f32_dot(vec4_f32 a, vec4_f32 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

internal vec2_f32 vec2_f32_scale(vec2_f32 a, f32 b)
{
	a.x *= b;
	a.y *= b;
	return a;
}

internal vec3_f32 vec3_f32_scale(vec3_f32 a, f32 b)
{
	a.x *= b;
	a.y *= b;
	a.z *= b;
	return a;
}

internal vec4_f32 vec4_f32_scale(vec4_f32 a, f32 b)
{
	a.x *= b;
	a.y *= b;
	a.z *= b;
	a.w *= b;
	return a;
}

internal f32 vec2_f32_length(vec2_f32 v)
{
	return sqrtf(vec2_f32_dot(v, v));
}

internal f32 vec3_f32_length(vec3_f32 v)
{
	return sqrtf(vec3_f32_dot(v, v));
}

internal f32 vec4_f32_length(vec4_f32 v)
{
	return sqrtf(vec4_f32_dot(v, v));
}

internal f32 vec2_f32_length_squared(vec2_f32 v)
{
	return vec2_f32_dot(v, v);
}
internal f32 vec3_f32_length_squared(vec3_f32 v)
{
	return vec3_f32_dot(v, v);
}
internal f32 vec4_f32_length_squared(vec4_f32 v)
{
	return vec4_f32_dot(v, v);
}

internal vec2_f32 vec2_f32_normalize(vec2_f32 v)
{
	f32 len = vec2_f32_length(v);
	if (len < FLT_EPSILON)
	{
		return vec2_f32_create(0, 0);
	}
	f32 inverse_len = 1 / len;
	return vec2_f32_scale(v, inverse_len);
}

internal vec3_f32 vec3_f32_normalize(vec3_f32 v)
{
	f32 len = vec3_f32_length(v);
	if (len < FLT_EPSILON)
	{
		return vec3_f32_create(0, 0, 0);
	}
	f32 inverse_len = 1 / len;
	return vec3_f32_scale(v, inverse_len);
}

internal vec4_f32 vec4_f32_normalize(vec4_f32 v)
{
	f32 len = vec4_f32_length(v);
	if (len < FLT_EPSILON)
	{
		return vec4_f32_create(0, 0, 0, 0);
	}
	f32 inverse_len = 1 / len;
	return vec4_f32_scale(v, inverse_len);
}

internal mat4x4_f32 mat4x4_f32_create_identity(void)
{
	mat4x4_f32 result = {0};
	result.e[0][0]	 = 1.0f;
	result.e[1][1]	 = 1.0f;
	result.e[2][2]	 = 1.0f;
	result.e[3][3]	 = 1.0f;
	return result;
}

internal mat4x4_f32 mat4x4_f32_create_from_vec3_columns(vec3_f32 c0, vec3_f32 c1, vec3_f32 c2, vec3_f32 c3)
{
	// c0, c1, c2 = rotation/scale basis vectors (X, Y, Z axes)
	// c3         = translation
	// 4th component (w) filled in implicitly: 0 for basis columns, 1 for translation
	mat4x4_f32 result;
	result.columns[0] = vec4_f32_create(c0.x, c0.y, c0.z, 0.0f);
	result.columns[1] = vec4_f32_create(c1.x, c1.y, c1.z, 0.0f);
	result.columns[2] = vec4_f32_create(c2.x, c2.y, c2.z, 0.0f);
	result.columns[3] = vec4_f32_create(c3.x, c3.y, c3.z, 1.0f);
	return result;
}

////////////////////////////////
// mat4x4_f32 core ops

internal mat4x4_f32 mat4x4_f32_mul(mat4x4_f32 a, mat4x4_f32 b)
{
	mat4x4_f32 result;
	for (i32 col = 0; col < 4; col += 1)
	{
		for (i32 row = 0; row < 4; row += 1)
		{
			f32 sum = 0;
			for (i32 k = 0; k < 4; k += 1)
			{
				sum += a.e[k][row] * b.e[col][k];
			}
			result.e[col][row] = sum;
		}
	}
	return result;
}

internal vec4_f32 mat4x4_f32_mul_vec4_f32(mat4x4_f32 m, vec4_f32 v)
{
	vec4_f32 result;
	for (i32 row = 0; row < 4; row += 1)
	{
		f32 sum = 0;
		for (i32 col = 0; col < 4; col += 1)
		{
			sum += m.e[col][row] * v.e[col];
		}
		result.e[row] = sum;
	}
	return result;
}

internal mat4x4_f32 mat4x4_f32_transpose(mat4x4_f32 m)
{
	mat4x4_f32 result;
	for (i32 col = 0; col < 4; col += 1)
	{
		for (i32 row = 0; row < 4; row += 1)
		{
			result.e[col][row] = m.e[row][col];
		}
	}
	return result;
}

internal mat4x4_f32 mat4x4_f32_inverse(mat4x4_f32 m)
{
	// classic cofactor/adjugate expansion, operating directly on the
	// flat column-major elements array (no row/col bookkeeping needed)
	f32		 *a		 = m.elements;
	mat4x4_f32 result = {0};
	f32		 *inv	 = result.elements;

	inv[0]	= a[5] * a[10] * a[15] - a[5] * a[11] * a[14] - a[9] * a[6] * a[15] + a[9] * a[7] * a[14] +
			  a[13] * a[6] * a[11] - a[13] * a[7] * a[10];
	inv[4]	= -a[4] * a[10] * a[15] + a[4] * a[11] * a[14] + a[8] * a[6] * a[15] - a[8] * a[7] * a[14] -
			  a[12] * a[6] * a[11] + a[12] * a[7] * a[10];
	inv[8]	= a[4] * a[9] * a[15] - a[4] * a[11] * a[13] - a[8] * a[5] * a[15] + a[8] * a[7] * a[13] +
			  a[12] * a[5] * a[11] - a[12] * a[7] * a[9];
	inv[12] = -a[4] * a[9] * a[14] + a[4] * a[10] * a[13] + a[8] * a[5] * a[14] - a[8] * a[6] * a[13] -
			  a[12] * a[5] * a[10] + a[12] * a[6] * a[9];

	inv[1]	= -a[1] * a[10] * a[15] + a[1] * a[11] * a[14] + a[9] * a[2] * a[15] - a[9] * a[3] * a[14] -
			  a[13] * a[2] * a[11] + a[13] * a[3] * a[10];
	inv[5]	= a[0] * a[10] * a[15] - a[0] * a[11] * a[14] - a[8] * a[2] * a[15] + a[8] * a[3] * a[14] +
			  a[12] * a[2] * a[11] - a[12] * a[3] * a[10];
	inv[9]	= -a[0] * a[9] * a[15] + a[0] * a[11] * a[13] + a[8] * a[1] * a[15] - a[8] * a[3] * a[13] -
			  a[12] * a[1] * a[11] + a[12] * a[3] * a[9];
	inv[13] = a[0] * a[9] * a[14] - a[0] * a[10] * a[13] - a[8] * a[1] * a[14] + a[8] * a[2] * a[13] +
			  a[12] * a[1] * a[10] - a[12] * a[2] * a[9];

	inv[2]	= a[1] * a[6] * a[15] - a[1] * a[7] * a[14] - a[5] * a[2] * a[15] + a[5] * a[3] * a[14] +
			  a[13] * a[2] * a[7] - a[13] * a[3] * a[6];
	inv[6]	= -a[0] * a[6] * a[15] + a[0] * a[7] * a[14] + a[4] * a[2] * a[15] - a[4] * a[3] * a[14] -
			  a[12] * a[2] * a[7] + a[12] * a[3] * a[6];
	inv[10] = a[0] * a[5] * a[15] - a[0] * a[7] * a[13] - a[4] * a[1] * a[15] + a[4] * a[3] * a[13] +
			  a[12] * a[1] * a[7] - a[12] * a[3] * a[5];
	inv[14] = -a[0] * a[5] * a[14] + a[0] * a[6] * a[13] + a[4] * a[1] * a[14] - a[4] * a[2] * a[13] -
			  a[12] * a[1] * a[6] + a[12] * a[2] * a[5];

	inv[3]	= -a[1] * a[6] * a[11] + a[1] * a[7] * a[10] + a[5] * a[2] * a[11] - a[5] * a[3] * a[10] -
			  a[9] * a[2] * a[7] + a[9] * a[3] * a[6];
	inv[7]	= a[0] * a[6] * a[11] - a[0] * a[7] * a[10] - a[4] * a[2] * a[11] + a[4] * a[3] * a[10] +
			  a[8] * a[2] * a[7] - a[8] * a[3] * a[6];
	inv[11] = -a[0] * a[5] * a[11] + a[0] * a[7] * a[9] + a[4] * a[1] * a[11] - a[4] * a[3] * a[9] -
			  a[8] * a[1] * a[7] + a[8] * a[3] * a[5];
	inv[15] = a[0] * a[5] * a[10] - a[0] * a[6] * a[9] - a[4] * a[1] * a[10] + a[4] * a[2] * a[9] + a[8] * a[1] * a[6] -
			  a[8] * a[2] * a[5];

	f32 det = a[0] * inv[0] + a[1] * inv[4] + a[2] * inv[8] + a[3] * inv[12];
	if (det != 0.f)
	{
		f32 inv_det = 1.f / det;
		for (i32 i = 0; i < 16; i += 1)
		{
			inv[i] *= inv_det;
		}
	}
	return result;
}

////////////////////////////////
// Construction

internal mat4x4_f32 mat4x4_f32_translate(vec3_f32 t)
{
	mat4x4_f32 result = {
		.e =
			{
				{1, 0, 0, 0},
				{0, 1, 0, 0},
				{0, 0, 1, 0},
				{t.x, t.y, t.z, 1},
			},
	};
	return result;
}

internal mat4x4_f32 mat4x4_f32_scale(vec3_f32 s)
{
	mat4x4_f32 result = {
		.e =
			{
				{s.x, 0, 0, 0},
				{0, s.y, 0, 0},
				{0, 0, s.z, 0},
				{0, 0, 0, 1},
			},
	};
	return result;
}

internal mat4x4_f32 mat4x4_f32_rotate_x(f32 radians)
{
	f32		  c		 = cosf(radians);
	f32		  s		 = sinf(radians);
	mat4x4_f32 result = {
		.e =
			{
				{1, 0, 0, 0},
				{0, c, s, 0},
				{0, -s, c, 0},
				{0, 0, 0, 1},
			},
	};
	return result;
}

internal mat4x4_f32 mat4x4_f32_rotate_y(f32 radians)
{
	f32		  c		 = cosf(radians);
	f32		  s		 = sinf(radians);
	mat4x4_f32 result = {
		.e =
			{
				{c, 0, -s, 0},
				{0, 1, 0, 0},
				{s, 0, c, 0},
				{0, 0, 0, 1},
			},
	};
	return result;
}

internal mat4x4_f32 mat4x4_f32_rotate_z(f32 radians)
{
	f32		  c		 = cosf(radians);
	f32		  s		 = sinf(radians);
	mat4x4_f32 result = {
		.e =
			{
				{c, s, 0, 0},
				{-s, c, 0, 0},
				{0, 0, 1, 0},
				{0, 0, 0, 1},
			},
	};
	return result;
}

internal mat4x4_f32 mat4x4_f32_rotate_axis_angle(vec3_f32 axis, f32 radians)
{
	// normalize axis defensively
	f32 len		= sqrtf(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
	f32 inv_len = (len != 0.f) ? (1.f / len) : 0.f;
	f32 x		= axis.x * inv_len;
	f32 y		= axis.y * inv_len;
	f32 z		= axis.z * inv_len;

	f32 c = cosf(radians);
	f32 s = sinf(radians);
	f32 t = 1.f - c;

	mat4x4_f32 result = {
		.e =
			{
				{t * x * x + c, t * x * y + s * z, t * x * z - s * y, 0},
				{t * x * y - s * z, t * y * y + c, t * y * z + s * x, 0},
				{t * x * z + s * y, t * y * z - s * x, t * z * z + c, 0},
				{0, 0, 0, 1},
			},
	};
	return result;
}

////////////////////////////////
// Decomposition

internal vec3_f32 mat4x4_f32_get_translation(mat4x4_f32 m)
{
	vec3_f32 result = {m.e[3][0], m.e[3][1], m.e[3][2]};
	return result;
}

internal vec3_f32 mat4x4_f32_get_scale(mat4x4_f32 m)
{
	vec3_f32 result = {
		sqrtf(m.e[0][0] * m.e[0][0] + m.e[0][1] * m.e[0][1] + m.e[0][2] * m.e[0][2]),
		sqrtf(m.e[1][0] * m.e[1][0] + m.e[1][1] * m.e[1][1] + m.e[1][2] * m.e[1][2]),
		sqrtf(m.e[2][0] * m.e[2][0] + m.e[2][1] * m.e[2][1] + m.e[2][2] * m.e[2][2]),
	};
	return result;
}

internal vec3_f32 mat4x4_f32_get_rotation(mat4x4_f32 m)
{
	// strip scale so we're decomposing a pure rotation matrix
	vec3_f32 scale = mat4x4_f32_get_scale(m);
	f32		sx	  = (scale.x != 0.f) ? (1.f / scale.x) : 0.f;
	f32		sy	  = (scale.y != 0.f) ? (1.f / scale.y) : 0.f;
	f32		sz	  = (scale.z != 0.f) ? (1.f / scale.z) : 0.f;

	f32 r00 = m.e[0][0] * sx, r01 = m.e[0][1] * sx, r02 = m.e[0][2] * sx;
	f32 r12 = m.e[1][2] * sy;
	f32 r22 = m.e[2][2] * sz;

	// Tait-Bryan ZYX extraction (matches R = RotateZ * RotateY * RotateX
	// composition order). Clamp guards against asin domain errors from
	// floating point drift at the gimbal-lock poles.
	f32 sin_y = -r02;
	sin_y	  = (sin_y < -1.f) ? -1.f : ((sin_y > 1.f) ? 1.f : sin_y);

	vec3_f32 result = {
		atan2f(r12, r22), // x (roll)
		asinf(sin_y),	  // y (pitch)
		atan2f(r01, r00), // z (yaw)
	};
	return result;
}

////////////////////////
// f64 variant
internal vec2_f64 vec2_f64_create(f64 x, f64 y)
{
	return (vec2_f64){x, y};
}
internal vec3_f64 vec3_f64_create(f64 x, f64 y, f64 z)
{
	return (vec3_f64){x, y, z};
}
internal vec4_f64 vec4_f64_create(f64 x, f64 y, f64 z, f64 w)
{
	return (vec4_f64){x, y, z, w};
}

internal vec2_f64 vec2_f64_add(vec2_f64 a, vec2_f64 b)
{
	vec2_f64 result = {0};
	result.x	   = a.x + b.x;
	result.y	   = a.y + b.y;
	return result;
}
internal vec3_f64 vec3_f64_add(vec3_f64 a, vec3_f64 b)
{
	vec3_f64 result = {0};
	result.x	   = a.x + b.x;
	result.y	   = a.y + b.y;
	result.z	   = a.z + b.z;
	return result;
}
internal vec4_f64 vec4_f64_add(vec4_f64 a, vec4_f64 b)
{
	vec4_f64 result = {0};
	result.x	   = a.x + b.x;
	result.y	   = a.y + b.y;
	result.z	   = a.z + b.z;
	result.w	   = a.w + b.w;
	return result;
}

internal vec2_f64 vec2_f64_sub(vec2_f64 a, vec2_f64 b)
{
	vec2_f64 result = {0};
	result.x	   = a.x - b.x;
	result.y	   = a.y - b.y;
	return result;
}
internal vec3_f64 vec3_f64_sub(vec3_f64 a, vec3_f64 b)
{
	vec3_f64 result = {0};
	result.x	   = a.x - b.x;
	result.y	   = a.y - b.y;
	result.z	   = a.z - b.z;
	return result;
}
internal vec4_f64 vec4_f64_sub(vec4_f64 a, vec4_f64 b)
{
	vec4_f64 result = {0};
	result.x	   = a.x - b.x;
	result.y	   = a.y - b.y;
	result.z	   = a.z - b.z;
	result.w	   = a.w - b.w;
	return result;
}

internal vec2_f64 vec2_f64_mul(vec2_f64 a, vec2_f64 b)
{
	vec2_f64 result = {0};
	result.x	   = a.x * b.x;
	result.y	   = a.y * b.y;
	return result;
}
internal vec3_f64 vec3_f64_mul(vec3_f64 a, vec3_f64 b)
{
	vec3_f64 result = {0};
	result.x	   = a.x * b.x;
	result.y	   = a.y * b.y;
	result.z	   = a.z * b.z;
	return result;
}
internal vec4_f64 vec4_f64_mul(vec4_f64 a, vec4_f64 b)
{
	vec4_f64 result = {0};
	result.x	   = a.x * b.x;
	result.y	   = a.y * b.y;
	result.z	   = a.z * b.z;
	result.w	   = a.w * b.w;
	return result;
}

internal f64 vec2_f64_dot(vec2_f64 a, vec2_f64 b)
{
	return (a.x * b.x) + (a.y * b.y);
}
internal f64 vec3_f64_dot(vec3_f64 a, vec3_f64 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}
internal f64 vec4_f64_dot(vec4_f64 a, vec4_f64 b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

internal vec2_f64 vec2_f64_scale(vec2_f64 a, f64 b)
{
	a.x *= b;
	a.y *= b;
	return a;
}
internal vec3_f64 vec3_f64_scale(vec3_f64 a, f64 b)
{
	a.x *= b;
	a.y *= b;
	a.z *= b;
	return a;
}
internal vec4_f64 vec4_f64_scale(vec4_f64 a, f64 b)
{
	a.x *= b;
	a.y *= b;
	a.z *= b;
	a.w *= b;
	return a;
}

internal f64 vec2_f64_length(vec2_f64 v)
{
	return sqrt(vec2_f64_dot(v, v));
}
internal f64 vec3_f64_length(vec3_f64 v)
{
	return sqrt(vec3_f64_dot(v, v));
}
internal f64 vec4_f64_length(vec4_f64 v)
{
	return sqrt(vec4_f64_dot(v, v));
}

internal f64 vec2_f64_length_squared(vec2_f64 v)
{
	return vec2_f64_dot(v, v);
}
internal f64 vec3_f64_length_squared(vec3_f64 v)
{
	return vec3_f64_dot(v, v);
}
internal f64 vec4_f64_length_squared(vec4_f64 v)
{
	return vec4_f64_dot(v, v);
}

internal vec2_f64 vec2_f64_normalize(vec2_f64 v)
{
	f64 len = vec2_f64_length(v);
	if (len < DBL_EPSILON)
	{
		return vec2_f64_create(0, 0);
	}
	f64 inverse_len = 1.0 / len;
	return vec2_f64_scale(v, inverse_len);
}

internal vec3_f64 vec3_f64_normalize(vec3_f64 v)
{
	f64 len = vec3_f64_length(v);
	if (len < DBL_EPSILON)
	{
		return vec3_f64_create(0, 0, 0);
	}
	f64 inverse_len = 1.0 / len;
	return vec3_f64_scale(v, inverse_len);
}

internal vec4_f64 vec4_f64_normalize(vec4_f64 v)
{
	f64 len = vec4_f64_length(v);
	if (len < DBL_EPSILON)
	{
		return vec4_f64_create(0, 0, 0, 0);
	}
	f64 inverse_len = 1.0 / len;
	return vec4_f64_scale(v, inverse_len);
}

////////////////////////////////
// Construction helpers

internal mat4x4_f64 mat4x4_f64_create_identity(void)
{
	mat4x4_f64 result = {
		.e =
			{
				{1, 0, 0, 0},
				{0, 1, 0, 0},
				{0, 0, 1, 0},
				{0, 0, 0, 1},
			},
	};
	return result;
}

internal mat4x4_f64 mat4x4_f64_create_from_vec3_columns(vec3_f64 c0, vec3_f64 c1, vec3_f64 c2, vec3_f64 c3)
{
	// c0/c1/c2 are basis vectors (w=0), c3 is translation (w=1)
	mat4x4_f64 result = {
		.e =
			{
				{c0.x, c0.y, c0.z, 0},
				{c1.x, c1.y, c1.z, 0},
				{c2.x, c2.y, c2.z, 0},
				{c3.x, c3.y, c3.z, 1},
			},
	};
	return result;
}

////////////////////////////////
// mat4x4_f64 core ops

internal mat4x4_f64 mat4x4_f64_mul(mat4x4_f64 a, mat4x4_f64 b)
{
	mat4x4_f64 result;
	for (i32 col = 0; col < 4; col += 1)
	{
		for (i32 row = 0; row < 4; row += 1)
		{
			f64 sum = 0;
			for (i32 k = 0; k < 4; k += 1)
			{
				sum += a.e[k][row] * b.e[col][k];
			}
			result.e[col][row] = sum;
		}
	}
	return result;
}

internal vec4_f64 mat4x4_f64_mul_vec4_f64(mat4x4_f64 m, vec4_f64 v)
{
	vec4_f64 result;
	for (i32 row = 0; row < 4; row += 1)
	{
		f64 sum = 0;
		for (i32 col = 0; col < 4; col += 1)
		{
			sum += m.e[col][row] * v.e[col];
		}
		result.e[row] = sum;
	}
	return result;
}

internal mat4x4_f64 mat4x4_f64_transpose(mat4x4_f64 m)
{
	mat4x4_f64 result;
	for (i32 col = 0; col < 4; col += 1)
	{
		for (i32 row = 0; row < 4; row += 1)
		{
			result.e[col][row] = m.e[row][col];
		}
	}
	return result;
}

internal mat4x4_f64 mat4x4_f64_inverse(mat4x4_f64 m)
{
	f64		 *a		 = m.elements;
	mat4x4_f64 result = {0};
	f64		 *inv	 = result.elements;

	inv[0]	= a[5] * a[10] * a[15] - a[5] * a[11] * a[14] - a[9] * a[6] * a[15] + a[9] * a[7] * a[14] +
			  a[13] * a[6] * a[11] - a[13] * a[7] * a[10];
	inv[4]	= -a[4] * a[10] * a[15] + a[4] * a[11] * a[14] + a[8] * a[6] * a[15] - a[8] * a[7] * a[14] -
			  a[12] * a[6] * a[11] + a[12] * a[7] * a[10];
	inv[8]	= a[4] * a[9] * a[15] - a[4] * a[11] * a[13] - a[8] * a[5] * a[15] + a[8] * a[7] * a[13] +
			  a[12] * a[5] * a[11] - a[12] * a[7] * a[9];
	inv[12] = -a[4] * a[9] * a[14] + a[4] * a[10] * a[13] + a[8] * a[5] * a[14] - a[8] * a[6] * a[13] -
			  a[12] * a[5] * a[10] + a[12] * a[6] * a[9];

	inv[1]	= -a[1] * a[10] * a[15] + a[1] * a[11] * a[14] + a[9] * a[2] * a[15] - a[9] * a[3] * a[14] -
			  a[13] * a[2] * a[11] + a[13] * a[3] * a[10];
	inv[5]	= a[0] * a[10] * a[15] - a[0] * a[11] * a[14] - a[8] * a[2] * a[15] + a[8] * a[3] * a[14] +
			  a[12] * a[2] * a[11] - a[12] * a[3] * a[10];
	inv[9]	= -a[0] * a[9] * a[15] + a[0] * a[11] * a[13] + a[8] * a[1] * a[15] - a[8] * a[3] * a[13] -
			  a[12] * a[1] * a[11] + a[12] * a[3] * a[9];
	inv[13] = a[0] * a[9] * a[14] - a[0] * a[10] * a[13] - a[8] * a[1] * a[14] + a[8] * a[2] * a[13] +
			  a[12] * a[1] * a[10] - a[12] * a[2] * a[9];

	inv[2]	= a[1] * a[6] * a[15] - a[1] * a[7] * a[14] - a[5] * a[2] * a[15] + a[5] * a[3] * a[14] +
			  a[13] * a[2] * a[7] - a[13] * a[3] * a[6];
	inv[6]	= -a[0] * a[6] * a[15] + a[0] * a[7] * a[14] + a[4] * a[2] * a[15] - a[4] * a[3] * a[14] -
			  a[12] * a[2] * a[7] + a[12] * a[3] * a[6];
	inv[10] = a[0] * a[5] * a[15] - a[0] * a[7] * a[13] - a[4] * a[1] * a[15] + a[4] * a[3] * a[13] +
			  a[12] * a[1] * a[7] - a[12] * a[3] * a[5];
	inv[14] = -a[0] * a[5] * a[14] + a[0] * a[6] * a[13] + a[4] * a[1] * a[14] - a[4] * a[2] * a[13] -
			  a[12] * a[1] * a[6] + a[12] * a[2] * a[5];

	inv[3]	= -a[1] * a[6] * a[11] + a[1] * a[7] * a[10] + a[5] * a[2] * a[11] - a[5] * a[3] * a[10] -
			  a[9] * a[2] * a[7] + a[9] * a[3] * a[6];
	inv[7]	= a[0] * a[6] * a[11] - a[0] * a[7] * a[10] - a[4] * a[2] * a[11] + a[4] * a[3] * a[10] +
			  a[8] * a[2] * a[7] - a[8] * a[3] * a[6];
	inv[11] = -a[0] * a[5] * a[11] + a[0] * a[7] * a[9] + a[4] * a[1] * a[11] - a[4] * a[3] * a[9] -
			  a[8] * a[1] * a[7] + a[8] * a[3] * a[5];
	inv[15] = a[0] * a[5] * a[10] - a[0] * a[6] * a[9] - a[4] * a[1] * a[10] + a[4] * a[2] * a[9] + a[8] * a[1] * a[6] -
			  a[8] * a[2] * a[5];

	f64 det = a[0] * inv[0] + a[1] * inv[4] + a[2] * inv[8] + a[3] * inv[12];
	if (det != 0.0)
	{
		f64 inv_det = 1.0 / det;
		for (i32 i = 0; i < 16; i += 1)
		{
			inv[i] *= inv_det;
		}
	}
	return result;
}

////////////////////////////////
// Transform construction

internal mat4x4_f64 mat4x4_f64_translate(vec3_f64 t)
{
	mat4x4_f64 result = {
		.e =
			{
				{1, 0, 0, 0},
				{0, 1, 0, 0},
				{0, 0, 1, 0},
				{t.x, t.y, t.z, 1},
			},
	};
	return result;
}

internal mat4x4_f64 mat4x4_f64_scale(vec3_f64 s)
{
	mat4x4_f64 result = {
		.e =
			{
				{s.x, 0, 0, 0},
				{0, s.y, 0, 0},
				{0, 0, s.z, 0},
				{0, 0, 0, 1},
			},
	};
	return result;
}

internal mat4x4_f64 mat4x4_f64_rotate_x(f64 radians)
{
	f64		  c		 = cos(radians);
	f64		  s		 = sin(radians);
	mat4x4_f64 result = {
		.e =
			{
				{1, 0, 0, 0},
				{0, c, s, 0},
				{0, -s, c, 0},
				{0, 0, 0, 1},
			},
	};
	return result;
}

internal mat4x4_f64 mat4x4_f64_rotate_y(f64 radians)
{
	f64		  c		 = cos(radians);
	f64		  s		 = sin(radians);
	mat4x4_f64 result = {
		.e =
			{
				{c, 0, -s, 0},
				{0, 1, 0, 0},
				{s, 0, c, 0},
				{0, 0, 0, 1},
			},
	};
	return result;
}

internal mat4x4_f64 mat4x4_f64_rotate_z(f64 radians)
{
	f64		  c		 = cos(radians);
	f64		  s		 = sin(radians);
	mat4x4_f64 result = {
		.e =
			{
				{c, s, 0, 0},
				{-s, c, 0, 0},
				{0, 0, 1, 0},
				{0, 0, 0, 1},
			},
	};
	return result;
}

internal mat4x4_f64 mat4x4_f64_rotate_axis_angle(vec3_f64 axis, f64 radians)
{
	f64 len		= sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
	f64 inv_len = (len != 0.0) ? (1.0 / len) : 0.0;
	f64 x		= axis.x * inv_len;
	f64 y		= axis.y * inv_len;
	f64 z		= axis.z * inv_len;

	f64 c = cos(radians);
	f64 s = sin(radians);
	f64 t = 1.0 - c;

	mat4x4_f64 result = {
		.e =
			{
				{t * x * x + c, t * x * y + s * z, t * x * z - s * y, 0},
				{t * x * y - s * z, t * y * y + c, t * y * z + s * x, 0},
				{t * x * z + s * y, t * y * z - s * x, t * z * z + c, 0},
				{0, 0, 0, 1},
			},
	};
	return result;
}

////////////////////////////////
// Decomposition

internal vec3_f64 mat4x4_f64_get_translation(mat4x4_f64 m)
{
	vec3_f64 result = {m.e[3][0], m.e[3][1], m.e[3][2]};
	return result;
}

internal vec3_f64 mat4x4_f64_get_scale(mat4x4_f64 m)
{
	vec3_f64 result = {
		sqrt(m.e[0][0] * m.e[0][0] + m.e[0][1] * m.e[0][1] + m.e[0][2] * m.e[0][2]),
		sqrt(m.e[1][0] * m.e[1][0] + m.e[1][1] * m.e[1][1] + m.e[1][2] * m.e[1][2]),
		sqrt(m.e[2][0] * m.e[2][0] + m.e[2][1] * m.e[2][1] + m.e[2][2] * m.e[2][2]),
	};
	return result;
}

internal vec3_f64 mat4x4_f64_get_rotation(mat4x4_f64 m)
{
	vec3_f64 scale = mat4x4_f64_get_scale(m);
	f64		sx	  = (scale.x != 0.0) ? (1.0 / scale.x) : 0.0;
	f64		sy	  = (scale.y != 0.0) ? (1.0 / scale.y) : 0.0;
	f64		sz	  = (scale.z != 0.0) ? (1.0 / scale.z) : 0.0;

	f64 r00 = m.e[0][0] * sx, r01 = m.e[0][1] * sx, r02 = m.e[0][2] * sx;
	f64 r12 = m.e[1][2] * sy;
	f64 r22 = m.e[2][2] * sz;

	f64 sin_y = -r02;
	sin_y	  = (sin_y < -1.0) ? -1.0 : ((sin_y > 1.0) ? 1.0 : sin_y);

	vec3_f64 result = {
		atan2(r12, r22), // x (roll)
		asin(sin_y),	 // y (pitch)
		atan2(r01, r00), // z (yaw)
	};
	return result;
}

////////////////////////////////
// Quaternion f32 variant

internal quat4_f32 quat4_f32_create(f32 x, f32 y, f32 z, f32 w)
{
	return (quat4_f32){x, y, z, w};
}

internal quat4_f32 quat4_f32_identity(void)
{
	return (quat4_f32){0, 0, 0, 1};
}

internal quat4_f32 quat4_f32_add(quat4_f32 a, quat4_f32 b)
{
	return (quat4_f32){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

internal quat4_f32 quat4_f32_sub(quat4_f32 a, quat4_f32 b)
{
	return (quat4_f32){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

internal quat4_f32 quat4_f32_mul(quat4_f32 a, quat4_f32 b)
{
	// quaternion product: (a0 + a1*i + a2*j + a3*k)(b0 + b1*i + b2*j + b3*k)
	// where a.w is scalar, a.xyz is vector part
	quat4_f32 result;
	result.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
	result.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
	result.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
	result.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;
	return result;
}

internal quat4_f32 quat4_f32_scale(quat4_f32 q, f32 s)
{
	return (quat4_f32){q.x * s, q.y * s, q.z * s, q.w * s};
}

internal quat4_f32 quat4_f32_conjugate(quat4_f32 q)
{
	return (quat4_f32){-q.x, -q.y, -q.z, q.w};
}

internal f32 quat4_f32_length_squared(quat4_f32 q)
{
	return q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
}

internal f32 quat4_f32_length(quat4_f32 q)
{
	return sqrtf(quat4_f32_length_squared(q));
}

internal f32 quat4_f32_dot(quat4_f32 a, quat4_f32 b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

internal quat4_f32 quat4_f32_normalize(quat4_f32 q)
{
	f32 len = quat4_f32_length(q);
	if(len < FLT_EPSILON)
	{
		return quat4_f32_identity();
	}
	f32 inv_len = 1.0f / len;
	return quat4_f32_scale(q, inv_len);
}

internal quat4_f32 quat4_f32_inverse(quat4_f32 q)
{
	f32 len_sq = quat4_f32_length_squared(q);
	if(len_sq < FLT_EPSILON)
	{
		return quat4_f32_identity();
	}
	quat4_f32 conj = quat4_f32_conjugate(q);
	return quat4_f32_scale(conj, 1.0f / len_sq);
}

internal quat4_f32 quat4_f32_from_axis_angle(vec3_f32 axis, f32 radians)
{
	f32 len = sqrtf(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
	f32 inv_len = (len != 0.f) ? (1.f / len) : 0.f;
	
	f32 half_angle = radians * 0.5f;
	f32 sin_half = sinf(half_angle);
	f32 cos_half = cosf(half_angle);
	
	quat4_f32 result;
	result.x = axis.x * inv_len * sin_half;
	result.y = axis.y * inv_len * sin_half;
	result.z = axis.z * inv_len * sin_half;
	result.w = cos_half;
	return result;
}

internal void quat4_f32_to_axis_angle(quat4_f32 q, vec3_f32 *out_axis, f32 *out_radians)
{
	q = quat4_f32_normalize(q);
	
	f32 sin_half_sq = q.x*q.x + q.y*q.y + q.z*q.z;
	f32 sin_half = sqrtf(sin_half_sq);
	
	if(sin_half < FLT_EPSILON)
	{
		// no rotation or 360-degree rotation
		out_axis->x = 1.f;
		out_axis->y = 0.f;
		out_axis->z = 0.f;
		*out_radians = 0.f;
	}
	else
	{
		f32 inv_sin = 1.f / sin_half;
		out_axis->x = q.x * inv_sin;
		out_axis->y = q.y * inv_sin;
		out_axis->z = q.z * inv_sin;
		*out_radians = 2.f * atan2f(sin_half, q.w);
	}
}

internal quat4_f32 quat4_f32_from_euler(vec3_f32 euler)
{
	// Tait-Bryan convention: ZYX (yaw-pitch-roll)
	// euler.x = roll (rotation around X), euler.y = pitch (Y), euler.z = yaw (Z)
	f32 roll = euler.x * 0.5f;
	f32 pitch = euler.y * 0.5f;
	f32 yaw = euler.z * 0.5f;
	
	f32 sr = sinf(roll), cr = cosf(roll);
	f32 sp = sinf(pitch), cp = cosf(pitch);
	f32 sy = sinf(yaw), cy = cosf(yaw);
	
	quat4_f32 result;
	result.x = sr*cp*cy - cr*sp*sy;
	result.y = cr*sp*cy + sr*cp*sy;
	result.z = cr*cp*sy - sr*sp*cy;
	result.w = cr*cp*cy + sr*sp*sy;
	return result;
}

internal vec3_f32 quat4_f32_to_euler(quat4_f32 q)
{
	// Tait-Bryan ZYX extraction
	q = quat4_f32_normalize(q);
	
	f32 x2 = q.x*q.x, y2 = q.y*q.y, z2 = q.z*q.z;
	f32 xy = q.x*q.y, xz = q.x*q.z, yz = q.y*q.z;
	f32 wx = q.w*q.x, wy = q.w*q.y, wz = q.w*q.z;
	
	// sin(pitch) clamped to avoid asin domain errors
	f32 sin_pitch = 2.0f * (wy - xz);
	sin_pitch = (sin_pitch < -1.f) ? -1.f : ((sin_pitch > 1.f) ? 1.f : sin_pitch);
	
	vec3_f32 result;
	result.x = atan2f(2.0f * (wz + xy), 1.0f - 2.0f * (x2 + y2));  // roll
	result.y = asinf(sin_pitch);                                     // pitch
	result.z = atan2f(2.0f * (wx + yz), 1.0f - 2.0f * (y2 + z2));  // yaw
	return result;
}

internal quat4_f32 quat4_f32_from_mat4x4(mat4x4_f32 m)
{
	// Shepperd's method for robust conversion
	quat4_f32 result;
	
	f32 trace = m.e[0][0] + m.e[1][1] + m.e[2][2];
	
	if(trace > 0.f)
	{
		f32 s = sqrtf(trace + 1.f) * 2.f;
		f32 inv_s = 1.f / s;
		result.w = 0.25f * s;
		result.x = (m.e[2][1] - m.e[1][2]) * inv_s;
		result.y = (m.e[0][2] - m.e[2][0]) * inv_s;
		result.z = (m.e[1][0] - m.e[0][1]) * inv_s;
	}
	else if(m.e[0][0] > m.e[1][1] && m.e[0][0] > m.e[2][2])
	{
		f32 s = sqrtf(1.f + m.e[0][0] - m.e[1][1] - m.e[2][2]) * 2.f;
		f32 inv_s = 1.f / s;
		result.w = (m.e[2][1] - m.e[1][2]) * inv_s;
		result.x = 0.25f * s;
		result.y = (m.e[0][1] + m.e[1][0]) * inv_s;
		result.z = (m.e[0][2] + m.e[2][0]) * inv_s;
	}
	else if(m.e[1][1] > m.e[2][2])
	{
		f32 s = sqrtf(1.f + m.e[1][1] - m.e[0][0] - m.e[2][2]) * 2.f;
		f32 inv_s = 1.f / s;
		result.w = (m.e[0][2] - m.e[2][0]) * inv_s;
		result.x = (m.e[0][1] + m.e[1][0]) * inv_s;
		result.y = 0.25f * s;
		result.z = (m.e[1][2] + m.e[2][1]) * inv_s;
	}
	else
	{
		f32 s = sqrtf(1.f + m.e[2][2] - m.e[0][0] - m.e[1][1]) * 2.f;
		f32 inv_s = 1.f / s;
		result.w = (m.e[1][0] - m.e[0][1]) * inv_s;
		result.x = (m.e[0][2] + m.e[2][0]) * inv_s;
		result.y = (m.e[1][2] + m.e[2][1]) * inv_s;
		result.z = 0.25f * s;
	}
	
	return result;
}

internal mat4x4_f32 quat4_f32_to_mat4x4(quat4_f32 q)
{
	q = quat4_f32_normalize(q);
	
	f32 x2 = q.x*q.x, y2 = q.y*q.y, z2 = q.z*q.z;
	f32 xy = q.x*q.y, xz = q.x*q.z, yz = q.y*q.z;
	f32 wx = q.w*q.x, wy = q.w*q.y, wz = q.w*q.z;
	
	mat4x4_f32 result = {
		.e = {
			{1.f - 2.f*(y2 + z2), 2.f*(xy + wz), 2.f*(xz - wy), 0.f},
			{2.f*(xy - wz), 1.f - 2.f*(x2 + z2), 2.f*(yz + wx), 0.f},
			{2.f*(xz + wy), 2.f*(yz - wx), 1.f - 2.f*(x2 + y2), 0.f},
			{0.f, 0.f, 0.f, 1.f},
		},
	};
	return result;
}

internal vec3_f32 quat4_f32_rotate_vec3(quat4_f32 q, vec3_f32 v)
{
	// v' = q * v * q^-1
	quat4_f32 v_quat = {v.x, v.y, v.z, 0.f};
	quat4_f32 q_inv = quat4_f32_inverse(q);
	quat4_f32 result_quat = quat4_f32_mul(quat4_f32_mul(q, v_quat), q_inv);
	return (vec3_f32){result_quat.x, result_quat.y, result_quat.z};
}

internal quat4_f32 quat4_f32_slerp(quat4_f32 a, quat4_f32 b, f32 t)
{
	a = quat4_f32_normalize(a);
	b = quat4_f32_normalize(b);
	
	f32 dot = quat4_f32_dot(a, b);
	
	// clamp dot to avoid numerical issues with acos
	dot = (dot < -1.f) ? -1.f : ((dot > 1.f) ? 1.f : dot);
	
	// if dot < 0, negate b to take the shorter path
	if(dot < 0.f)
	{
		b = quat4_f32_scale(b, -1.f);
		dot = -dot;
	}
	
	f32 theta = acosf(dot);
	f32 sin_theta = sinf(theta);
	
	if(sin_theta < FLT_EPSILON)
	{
		// quaternions are nearly parallel, use linear interpolation
		return quat4_f32_normalize(quat4_f32_add(
			quat4_f32_scale(a, 1.f - t),
			quat4_f32_scale(b, t)
		));
	}
	
	f32 inv_sin = 1.f / sin_theta;
	f32 w_a = sinf((1.f - t) * theta) * inv_sin;
	f32 w_b = sinf(t * theta) * inv_sin;
	
	return quat4_f32_normalize(quat4_f32_add(
		quat4_f32_scale(a, w_a),
		quat4_f32_scale(b, w_b)
	));
}

////////////////////////////////
// Quaternion f64 variant

internal quat4_f64 quat4_f64_create(f64 x, f64 y, f64 z, f64 w)
{
	return (quat4_f64){x, y, z, w};
}

internal quat4_f64 quat4_f64_identity(void)
{
	return (quat4_f64){0, 0, 0, 1};
}

internal quat4_f64 quat4_f64_add(quat4_f64 a, quat4_f64 b)
{
	return (quat4_f64){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

internal quat4_f64 quat4_f64_sub(quat4_f64 a, quat4_f64 b)
{
	return (quat4_f64){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

internal quat4_f64 quat4_f64_mul(quat4_f64 a, quat4_f64 b)
{
	quat4_f64 result;
	result.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
	result.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
	result.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
	result.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;
	return result;
}

internal quat4_f64 quat4_f64_scale(quat4_f64 q, f64 s)
{
	return (quat4_f64){q.x * s, q.y * s, q.z * s, q.w * s};
}

internal quat4_f64 quat4_f64_conjugate(quat4_f64 q)
{
	return (quat4_f64){-q.x, -q.y, -q.z, q.w};
}

internal f64 quat4_f64_length_squared(quat4_f64 q)
{
	return q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
}

internal f64 quat4_f64_length(quat4_f64 q)
{
	return sqrt(quat4_f64_length_squared(q));
}

internal f64 quat4_f64_dot(quat4_f64 a, quat4_f64 b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

internal quat4_f64 quat4_f64_normalize(quat4_f64 q)
{
	f64 len = quat4_f64_length(q);
	if(len < DBL_EPSILON)
	{
		return quat4_f64_identity();
	}
	f64 inv_len = 1.0 / len;
	return quat4_f64_scale(q, inv_len);
}

internal quat4_f64 quat4_f64_inverse(quat4_f64 q)
{
	f64 len_sq = quat4_f64_length_squared(q);
	if(len_sq < DBL_EPSILON)
	{
		return quat4_f64_identity();
	}
	quat4_f64 conj = quat4_f64_conjugate(q);
	return quat4_f64_scale(conj, 1.0 / len_sq);
}

internal quat4_f64 quat4_f64_from_axis_angle(vec3_f64 axis, f64 radians)
{
	f64 len = sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
	f64 inv_len = (len != 0.0) ? (1.0 / len) : 0.0;
	
	f64 half_angle = radians * 0.5;
	f64 sin_half = sin(half_angle);
	f64 cos_half = cos(half_angle);
	
	quat4_f64 result;
	result.x = axis.x * inv_len * sin_half;
	result.y = axis.y * inv_len * sin_half;
	result.z = axis.z * inv_len * sin_half;
	result.w = cos_half;
	return result;
}

internal void quat4_f64_to_axis_angle(quat4_f64 q, vec3_f64 *out_axis, f64 *out_radians)
{
	q = quat4_f64_normalize(q);
	
	f64 sin_half_sq = q.x*q.x + q.y*q.y + q.z*q.z;
	f64 sin_half = sqrt(sin_half_sq);
	
	if(sin_half < DBL_EPSILON)
	{
		out_axis->x = 1.0;
		out_axis->y = 0.0;
		out_axis->z = 0.0;
		*out_radians = 0.0;
	}
	else
	{
		f64 inv_sin = 1.0 / sin_half;
		out_axis->x = q.x * inv_sin;
		out_axis->y = q.y * inv_sin;
		out_axis->z = q.z * inv_sin;
		*out_radians = 2.0 * atan2(sin_half, q.w);
	}
}

internal quat4_f64 quat4_f64_from_euler(vec3_f64 euler)
{
	f64 roll = euler.x * 0.5;
	f64 pitch = euler.y * 0.5;
	f64 yaw = euler.z * 0.5;
	
	f64 sr = sin(roll), cr = cos(roll);
	f64 sp = sin(pitch), cp = cos(pitch);
	f64 sy = sin(yaw), cy = cos(yaw);
	
	quat4_f64 result;
	result.x = sr*cp*cy - cr*sp*sy;
	result.y = cr*sp*cy + sr*cp*sy;
	result.z = cr*cp*sy - sr*sp*cy;
	result.w = cr*cp*cy + sr*sp*sy;
	return result;
}

internal vec3_f64 quat4_f64_to_euler(quat4_f64 q)
{
	q = quat4_f64_normalize(q);
	
	f64 x2 = q.x*q.x, y2 = q.y*q.y, z2 = q.z*q.z;
	f64 xy = q.x*q.y, xz = q.x*q.z, yz = q.y*q.z;
	f64 wx = q.w*q.x, wy = q.w*q.y, wz = q.w*q.z;
	
	f64 sin_pitch = 2.0 * (wy - xz);
	sin_pitch = (sin_pitch < -1.0) ? -1.0 : ((sin_pitch > 1.0) ? 1.0 : sin_pitch);
	
	vec3_f64 result;
	result.x = atan2(2.0 * (wz + xy), 1.0 - 2.0 * (x2 + y2));
	result.y = asin(sin_pitch);
	result.z = atan2(2.0 * (wx + yz), 1.0 - 2.0 * (y2 + z2));
	return result;
}

internal quat4_f64 quat4_f64_from_mat4x4(mat4x4_f64 m)
{
	quat4_f64 result;
	
	f64 trace = m.e[0][0] + m.e[1][1] + m.e[2][2];
	
	if(trace > 0.0)
	{
		f64 s = sqrt(trace + 1.0) * 2.0;
		f64 inv_s = 1.0 / s;
		result.w = 0.25 * s;
		result.x = (m.e[2][1] - m.e[1][2]) * inv_s;
		result.y = (m.e[0][2] - m.e[2][0]) * inv_s;
		result.z = (m.e[1][0] - m.e[0][1]) * inv_s;
	}
	else if(m.e[0][0] > m.e[1][1] && m.e[0][0] > m.e[2][2])
	{
		f64 s = sqrt(1.0 + m.e[0][0] - m.e[1][1] - m.e[2][2]) * 2.0;
		f64 inv_s = 1.0 / s;
		result.w = (m.e[2][1] - m.e[1][2]) * inv_s;
		result.x = 0.25 * s;
		result.y = (m.e[0][1] + m.e[1][0]) * inv_s;
		result.z = (m.e[0][2] + m.e[2][0]) * inv_s;
	}
	else if(m.e[1][1] > m.e[2][2])
	{
		f64 s = sqrt(1.0 + m.e[1][1] - m.e[0][0] - m.e[2][2]) * 2.0;
		f64 inv_s = 1.0 / s;
		result.w = (m.e[0][2] - m.e[2][0]) * inv_s;
		result.x = (m.e[0][1] + m.e[1][0]) * inv_s;
		result.y = 0.25 * s;
		result.z = (m.e[1][2] + m.e[2][1]) * inv_s;
	}
	else
	{
		f64 s = sqrt(1.0 + m.e[2][2] - m.e[0][0] - m.e[1][1]) * 2.0;
		f64 inv_s = 1.0 / s;
		result.w = (m.e[1][0] - m.e[0][1]) * inv_s;
		result.x = (m.e[0][2] + m.e[2][0]) * inv_s;
		result.y = (m.e[1][2] + m.e[2][1]) * inv_s;
		result.z = 0.25 * s;
	}
	
	return result;
}

internal mat4x4_f64 quat4_f64_to_mat4x4(quat4_f64 q)
{
	q = quat4_f64_normalize(q);
	
	f64 x2 = q.x*q.x, y2 = q.y*q.y, z2 = q.z*q.z;
	f64 xy = q.x*q.y, xz = q.x*q.z, yz = q.y*q.z;
	f64 wx = q.w*q.x, wy = q.w*q.y, wz = q.w*q.z;
	
	mat4x4_f64 result = {
		.e = {
			{1.0 - 2.0*(y2 + z2), 2.0*(xy + wz), 2.0*(xz - wy), 0.0},
			{2.0*(xy - wz), 1.0 - 2.0*(x2 + z2), 2.0*(yz + wx), 0.0},
			{2.0*(xz + wy), 2.0*(yz - wx), 1.0 - 2.0*(x2 + y2), 0.0},
			{0.0, 0.0, 0.0, 1.0},
		},
	};
	return result;
}

internal vec3_f64 quat4_f64_rotate_vec3(quat4_f64 q, vec3_f64 v)
{
	quat4_f64 v_quat = {v.x, v.y, v.z, 0.0};
	quat4_f64 q_inv = quat4_f64_inverse(q);
	quat4_f64 result_quat = quat4_f64_mul(quat4_f64_mul(q, v_quat), q_inv);
	return (vec3_f64){result_quat.x, result_quat.y, result_quat.z};
}

internal quat4_f64 quat4_f64_slerp(quat4_f64 a, quat4_f64 b, f64 t)
{
	a = quat4_f64_normalize(a);
	b = quat4_f64_normalize(b);
	
	f64 dot = quat4_f64_dot(a, b);
	
	dot = (dot < -1.0) ? -1.0 : ((dot > 1.0) ? 1.0 : dot);
	
	if(dot < 0.0)
	{
		b = quat4_f64_scale(b, -1.0);
		dot = -dot;
	}
	
	f64 theta = acos(dot);
	f64 sin_theta = sin(theta);
	
	if(sin_theta < DBL_EPSILON)
	{
		return quat4_f64_normalize(quat4_f64_add(
			quat4_f64_scale(a, 1.0 - t),
			quat4_f64_scale(b, t)
		));
	}
	
	f64 inv_sin = 1.0 / sin_theta;
	f64 w_a = sin((1.0 - t) * theta) * inv_sin;
	f64 w_b = sin(t * theta) * inv_sin;
	
	return quat4_f64_normalize(quat4_f64_add(
		quat4_f64_scale(a, w_a),
		quat4_f64_scale(b, w_b)
	));
}
