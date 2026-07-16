#ifndef GLINT_BASE_MATH_MATH_H
#define GLINT_BASE_MATH_MATH_H

#include "../core/g_types.h"
#include <math.h>
#include <float.h>

// Constants
global constexpr f32 glint_vec_normalize_epsilon_f32 = 1e-6f;
global constexpr f64 glint_vec_normalize_epsilon_f64 = 1e-12;

global constexpr f32 glint_pi_f32 = 3.14159265359f;
global constexpr f64 glint_pi_f64 = 3.14159265358979311600;

global constexpr f32 glint_tau_f32 = 6.28318530718f;
global constexpr f64 glint_tau_f64 = 6.28318530717958647693;

global constexpr f32 glint_deg2rad_f32 = (glint_pi_f32 / 180.0f);
global constexpr f32 glint_rad2deg_f32 = (180.0f / glint_pi_f32);

global constexpr f64 glint_deg2rad_f64 = (glint_pi_f64 / 180.0);
global constexpr f64 glint_rad2deg_f64 = (180.0 / glint_pi_f64);

// Macros
#define GLINT_DEG2RAD_F32(x) ((x) * glint_deg2rad_f32)
#define GLINT_RAD2DEG_F32(x) ((x) * glint_rad2deg_f32)

#define GLINT_DEG2RAD_F64(x) ((x) * glint_deg2rad_f64)
#define GLINT_RAD2DEG_F64(x) ((x) * glint_rad2deg_f64)

////////////////////////
// F32 variant

typedef union vec2_f32 vec2_f32;
union vec2_f32
{
	struct
	{
		f32 x, y;
	};

	struct
	{
		f32 u, v;
	};

	f32 e[2];
};

typedef union vec3_f32 vec3_f32;
union vec3_f32
{
	struct
	{
		f32 x, y, z;
	};

	struct
	{
		f32 u, v, s;
	};

	f32 e[3];
};

typedef union vec4_f32 vec4_f32;
union vec4_f32
{
	struct
	{
		f32 x, y, z, w;
	};

	struct
	{
		f32 u, v, s, t;
	};

	f32 e[4];
};

typedef union mat4x4_f32 mat4x4_f32;
union mat4x4_f32
{
	f32		 e[4][4];	   // e[col][row] -- column-major
	f32		 elements[16]; // flat, column-major order, directly uploadable
	vec4_f32 columns[4];
};

typedef struct quat4_f32 quat4_f32;
struct quat4_f32
{
	f32 x, y, z, w; // w is scalar part
};

internal vec2_f32 vec2_f32_create(f32 x, f32 y);
internal vec3_f32 vec3_f32_create(f32 x, f32 y, f32 z);
internal vec4_f32 vec4_f32_create(f32 x, f32 y, f32 z, f32 w);

internal vec2_f32 vec2_f32_add(vec2_f32 a, vec2_f32 b);
internal vec3_f32 vec3_f32_add(vec3_f32 a, vec3_f32 b);
internal vec4_f32 vec4_f32_add(vec4_f32 a, vec4_f32 b);

internal vec2_f32 vec2_f32_sub(vec2_f32 a, vec2_f32 b);
internal vec3_f32 vec3_f32_sub(vec3_f32 a, vec3_f32 b);
internal vec4_f32 vec4_f32_sub(vec4_f32 a, vec4_f32 b);

internal vec2_f32 vec2_f32_mul(vec2_f32 a, vec2_f32 b);
internal vec3_f32 vec3_f32_mul(vec3_f32 a, vec3_f32 b);
internal vec4_f32 vec4_f32_mul(vec4_f32 a, vec4_f32 b);

internal f32 vec2_f32_dot(vec2_f32 a, vec2_f32 b);
internal f32 vec3_f32_dot(vec3_f32 a, vec3_f32 b);
internal f32 vec4_f32_dot(vec4_f32 a, vec4_f32 b);

internal vec2_f32 vec2_f32_scale(vec2_f32 a, f32 b);
internal vec3_f32 vec3_f32_scale(vec3_f32 a, f32 b);
internal vec4_f32 vec4_f32_scale(vec4_f32 a, f32 b);

internal f32 vec2_f32_length(vec2_f32 v);
internal f32 vec3_f32_length(vec3_f32 v);
internal f32 vec4_f32_length(vec4_f32 v);

internal f32 vec2_f32_length_squared(vec2_f32 v);
internal f32 vec3_f32_length_squared(vec3_f32 v);
internal f32 vec4_f32_length_squared(vec4_f32 v);

internal vec2_f32 vec2_f32_normalize(vec2_f32 v);
internal vec3_f32 vec3_f32_normalize(vec3_f32 v);
internal vec4_f32 vec4_f32_normalize(vec4_f32 v);

internal mat4x4_f32 mat4x4_f32_create_identity(void);
internal mat4x4_f32 mat4x4_f32_create_from_vec3_columns(vec3_f32 c0, vec3_f32 c1, vec3_f32 c2, vec3_f32 c3);

internal mat4x4_f32 mat4x4_f32_mul(mat4x4_f32 a, mat4x4_f32 b);
internal vec4_f32   mat4x4_f32_mul_vec4_f32(mat4x4_f32 m, vec4_f32 v);
internal mat4x4_f32 mat4x4_f32_transpose(mat4x4_f32 m);
internal mat4x4_f32 mat4x4_f32_inverse(mat4x4_f32 m);

internal mat4x4_f32 mat4x4_f32_translate(vec3_f32 t);
internal mat4x4_f32 mat4x4_f32_scale(vec3_f32 s);
internal mat4x4_f32 mat4x4_f32_rotate_x(f32 radians);
internal mat4x4_f32 mat4x4_f32_rotate_y(f32 radians);
internal mat4x4_f32 mat4x4_f32_rotate_z(f32 radians);
internal mat4x4_f32 mat4x4_f32_rotate_axis_angle(vec3_f32 axis, f32 radians);

internal vec3_f32 mat4x4_f32_get_translation(mat4x4_f32 m);
internal vec3_f32 mat4x4_f32_get_rotation(mat4x4_f32 m);
internal vec3_f32 mat4x4_f32_get_scale(mat4x4_f32 m);

internal quat4_f32 quat4_f32_create(f32 x, f32 y, f32 z, f32 w);
internal quat4_f32 quat4_f32_identity(void);

internal quat4_f32 quat4_f32_add(quat4_f32 a, quat4_f32 b);
internal quat4_f32 quat4_f32_sub(quat4_f32 a, quat4_f32 b);
internal quat4_f32 quat4_f32_mul(quat4_f32 a, quat4_f32 b);
internal quat4_f32 quat4_f32_scale(quat4_f32 q, f32 s);
internal quat4_f32 quat4_f32_conjugate(quat4_f32 q);
internal quat4_f32 quat4_f32_inverse(quat4_f32 q);

internal f32	   quat4_f32_dot(quat4_f32 a, quat4_f32 b);
internal f32	   quat4_f32_length_squared(quat4_f32 q);
internal f32	   quat4_f32_length(quat4_f32 q);
internal quat4_f32 quat4_f32_normalize(quat4_f32 q);

internal quat4_f32 quat4_f32_from_axis_angle(vec3_f32 axis, f32 radians);
internal void	   quat4_f32_to_axis_angle(quat4_f32 q, vec3_f32* out_axis, f32* out_radians);

internal quat4_f32 quat4_f32_from_euler(vec3_f32 euler);
internal vec3_f32  quat4_f32_to_euler(quat4_f32 q);

internal quat4_f32	quat4_f32_from_mat4x4(mat4x4_f32 m);
internal mat4x4_f32 quat4_f32_to_mat4x4(quat4_f32 q);

internal vec3_f32 quat4_f32_rotate_vec3(quat4_f32 q, vec3_f32 v);

internal quat4_f32 quat4_f32_slerp(quat4_f32 a, quat4_f32 b, f32 t);

////////////////////////
// F64 variant

typedef union vec2_f64 vec2_f64;
union vec2_f64
{
	struct
	{
		f64 x, y;
	};

	struct
	{
		f64 u, v;
	};

	f64 e[2];
};

typedef union vec3_f64 vec3_f64;
union vec3_f64
{
	struct
	{
		f64 x, y, z;
	};

	struct
	{
		f64 u, v, s;
	};

	f64 e[3];
};

typedef union vec4_f64 vec4_f64;
union vec4_f64
{
	struct
	{
		f64 x, y, z, w;
	};

	struct
	{
		f64 u, v, s, t;
	};

	f64 e[4];
};

typedef union mat4x4_f64 mat4x4_f64;
union mat4x4_f64
{
	f64		 e[4][4];
	f64		 elements[16];
	vec4_f64 columns[4];
};

typedef struct quat4_f64 quat4_f64;
struct quat4_f64
{
	f64 x, y, z, w;
};

internal vec2_f64 vec2_f64_create(f64 x, f64 y);
internal vec3_f64 vec3_f64_create(f64 x, f64 y, f64 z);
internal vec4_f64 vec4_f64_create(f64 x, f64 y, f64 z, f64 w);

internal vec2_f64 vec2_f64_add(vec2_f64 a, vec2_f64 b);
internal vec3_f64 vec3_f64_add(vec3_f64 a, vec3_f64 b);
internal vec4_f64 vec4_f64_add(vec4_f64 a, vec4_f64 b);

internal vec2_f64 vec2_f64_sub(vec2_f64 a, vec2_f64 b);
internal vec3_f64 vec3_f64_sub(vec3_f64 a, vec3_f64 b);
internal vec4_f64 vec4_f64_sub(vec4_f64 a, vec4_f64 b);

internal vec2_f64 vec2_f64_mul(vec2_f64 a, vec2_f64 b);
internal vec3_f64 vec3_f64_mul(vec3_f64 a, vec3_f64 b);
internal vec4_f64 vec4_f64_mul(vec4_f64 a, vec4_f64 b);

internal f64 vec2_f64_dot(vec2_f64 a, vec2_f64 b);
internal f64 vec3_f64_dot(vec3_f64 a, vec3_f64 b);
internal f64 vec4_f64_dot(vec4_f64 a, vec4_f64 b);

internal vec2_f64 vec2_f64_scale(vec2_f64 a, f64 b);
internal vec3_f64 vec3_f64_scale(vec3_f64 a, f64 b);
internal vec4_f64 vec4_f64_scale(vec4_f64 a, f64 b);

internal f64 vec2_f64_length(vec2_f64 v);
internal f64 vec3_f64_length(vec3_f64 v);
internal f64 vec4_f64_length(vec4_f64 v);

internal f64 vec2_f64_length_squared(vec2_f64 v);
internal f64 vec3_f64_length_squared(vec3_f64 v);
internal f64 vec4_f64_length_squared(vec4_f64 v);

internal vec2_f64 vec2_f64_normalize(vec2_f64 v);
internal vec3_f64 vec3_f64_normalize(vec3_f64 v);
internal vec4_f64 vec4_f64_normalize(vec4_f64 v);

internal mat4x4_f64 mat4x4_f64_create_identity(void);
internal mat4x4_f64 mat4x4_f64_create_from_vec3_columns(vec3_f64 c0, vec3_f64 c1, vec3_f64 c2, vec3_f64 c3);

internal mat4x4_f64 mat4x4_f64_mul(mat4x4_f64 a, mat4x4_f64 b);
internal vec4_f64   mat4x4_f64_mul_vec4_f64(mat4x4_f64 m, vec4_f64 v);
internal mat4x4_f64 mat4x4_f64_transpose(mat4x4_f64 m);
internal mat4x4_f64 mat4x4_f64_inverse(mat4x4_f64 m);

internal mat4x4_f64 mat4x4_f64_translate(vec3_f64 t);
internal mat4x4_f64 mat4x4_f64_scale(vec3_f64 s);
internal mat4x4_f64 mat4x4_f64_rotate_x(f64 radians);
internal mat4x4_f64 mat4x4_f64_rotate_y(f64 radians);
internal mat4x4_f64 mat4x4_f64_rotate_z(f64 radians);
internal mat4x4_f64 mat4x4_f64_rotate_axis_angle(vec3_f64 axis, f64 radians);

internal vec3_f64 mat4x4_f64_get_translation(mat4x4_f64 m);
internal vec3_f64 mat4x4_f64_get_rotation(mat4x4_f64 m);
internal vec3_f64 mat4x4_f64_get_scale(mat4x4_f64 m);

internal quat4_f64 quat4_f64_create(f64 x, f64 y, f64 z, f64 w);
internal quat4_f64 quat4_f64_identity(void);

internal quat4_f64 quat4_f64_add(quat4_f64 a, quat4_f64 b);
internal quat4_f64 quat4_f64_sub(quat4_f64 a, quat4_f64 b);
internal quat4_f64 quat4_f64_mul(quat4_f64 a, quat4_f64 b);
internal quat4_f64 quat4_f64_scale(quat4_f64 q, f64 s);
internal quat4_f64 quat4_f64_conjugate(quat4_f64 q);
internal quat4_f64 quat4_f64_inverse(quat4_f64 q);

internal f64	   quat4_f64_dot(quat4_f64 a, quat4_f64 b);
internal f64	   quat4_f64_length_squared(quat4_f64 q);
internal f64	   quat4_f64_length(quat4_f64 q);
internal quat4_f64 quat4_f64_normalize(quat4_f64 q);

internal quat4_f64 quat4_f64_from_axis_angle(vec3_f64 axis, f64 radians);
internal void	   quat4_f64_to_axis_angle(quat4_f64 q, vec3_f64* out_axis, f64* out_radians);

internal quat4_f64 quat4_f64_from_euler(vec3_f64 euler);
internal vec3_f64  quat4_f64_to_euler(quat4_f64 q);

internal quat4_f64	quat4_f64_from_mat4x4(mat4x4_f64 m);
internal mat4x4_f64 quat4_f64_to_mat4x4(quat4_f64 q);

internal vec3_f64 quat4_f64_rotate_vec3(quat4_f64 q, vec3_f64 v);

internal quat4_f64 quat4_f64_slerp(quat4_f64 a, quat4_f64 b, f64 t);

#endif // GLINT_BASE_MATH_MATH_H
