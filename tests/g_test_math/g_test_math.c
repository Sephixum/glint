#include "tests/g_test_common.h"
#include "base/base.h"

#define TEST_EPSILON_F32 1e-5f
#define TEST_EPSILON_F64 1e-9

static void test_vec2_f32_basic(void)
{
	GLINT_TEST_SECTION("vec2_f32 basic");

	vec2_f32 v2a = vec2_f32_create(3.0f, 4.0f);
	vec2_f32 v2b = vec2_f32_create(1.0f, 2.0f);

	GLINT_TEST_CHECK_EQ_FLOAT(v2a.x, 3.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(v2a.y, 4.0f, TEST_EPSILON_F32);

	vec2_f32 v2sum = vec2_f32_add(v2a, v2b);
	GLINT_TEST_CHECK_EQ_FLOAT(v2sum.x, 4.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(v2sum.y, 6.0f, TEST_EPSILON_F32);

	vec2_f32 v2sub = vec2_f32_sub(v2a, v2b);
	GLINT_TEST_CHECK_EQ_FLOAT(v2sub.x, 2.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(v2sub.y, 2.0f, TEST_EPSILON_F32);

	f32 v2dot = vec2_f32_dot(v2a, v2b);
	GLINT_TEST_CHECK_EQ_FLOAT(v2dot, 11.0f, TEST_EPSILON_F32); // 3*1 + 4*2 = 11

	f32 v2len = vec2_f32_length(v2a);
	GLINT_TEST_CHECK_EQ_FLOAT(v2len, 5.0f, TEST_EPSILON_F32); // sqrt(3^2 + 4^2) = 5

	f32 v2len_sq = vec2_f32_length_squared(v2a);
	GLINT_TEST_CHECK_EQ_FLOAT(v2len_sq, 25.0f, TEST_EPSILON_F32);

	vec2_f32 v2norm = vec2_f32_normalize(v2a);
	GLINT_TEST_CHECK_EQ_FLOAT(v2norm.x, 0.6f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(v2norm.y, 0.8f, TEST_EPSILON_F32);
}

static void test_vec3_f32_basic(void)
{
	GLINT_TEST_SECTION("vec3_f32 basic");

	vec3_f32 v3a = vec3_f32_create(1.0f, 0.0f, 0.0f);
	vec3_f32 v3b = vec3_f32_create(0.0f, 1.0f, 0.0f);

	vec3_f32 v3sum = vec3_f32_add(v3a, v3b);
	GLINT_TEST_CHECK_EQ_FLOAT(v3sum.x, 1.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(v3sum.y, 1.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(v3sum.z, 0.0f, TEST_EPSILON_F32);

	f32 v3dot = vec3_f32_dot(v3a, v3b);
	GLINT_TEST_CHECK_EQ_FLOAT(v3dot, 0.0f, TEST_EPSILON_F32); // perpendicular vectors

	vec3_f32 v3scale = vec3_f32_scale(v3a, 5.0f);
	GLINT_TEST_CHECK_EQ_FLOAT(v3scale.x, 5.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(v3scale.y, 0.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(v3scale.z, 0.0f, TEST_EPSILON_F32);
}

static void test_vec4_f32_basic(void)
{
	GLINT_TEST_SECTION("vec4_f32 basic");

	vec4_f32 v4a = vec4_f32_create(1.0f, 2.0f, 3.0f, 4.0f);
	vec4_f32 v4b = vec4_f32_create(5.0f, 6.0f, 7.0f, 8.0f);

	vec4_f32 v4sum = vec4_f32_add(v4a, v4b);
	GLINT_TEST_CHECK_EQ_FLOAT(v4sum.x, 6.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(v4sum.y, 8.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(v4sum.z, 10.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(v4sum.w, 12.0f, TEST_EPSILON_F32);

	f32 v4dot = vec4_f32_dot(v4a, v4b);
	GLINT_TEST_CHECK_EQ_FLOAT(v4dot, 70.0f, TEST_EPSILON_F32); // 1*5 + 2*6 + 3*7 + 4*8 = 70
}

static void test_mat4x4_f32_identity(void)
{
	GLINT_TEST_SECTION("mat4x4_f32 identity");

	mat4x4_f32 identity = mat4x4_f32_create_identity();

	GLINT_TEST_CHECK_EQ_FLOAT(identity.e[0][0], 1.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(identity.e[1][1], 1.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(identity.e[2][2], 1.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(identity.e[3][3], 1.0f, TEST_EPSILON_F32);

	GLINT_TEST_CHECK_EQ_FLOAT(identity.e[0][1], 0.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(identity.e[1][0], 0.0f, TEST_EPSILON_F32);
}

static void test_mat4x4_f32_translate(void)
{
	GLINT_TEST_SECTION("mat4x4_f32 translation");

	vec3_f32   translation_vec = vec3_f32_create(5.0f, 10.0f, 15.0f);
	mat4x4_f32 translation	   = mat4x4_f32_translate(translation_vec);

	vec3_f32 extracted = mat4x4_f32_get_translation(translation);
	GLINT_TEST_CHECK_EQ_FLOAT(extracted.x, 5.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(extracted.y, 10.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(extracted.z, 15.0f, TEST_EPSILON_F32);
}

static void test_mat4x4_f32_mul(void)
{
	GLINT_TEST_SECTION("mat4x4_f32 multiply");

	mat4x4_f32 identity	   = mat4x4_f32_create_identity();
	mat4x4_f32 translation = mat4x4_f32_translate(vec3_f32_create(1.0f, 2.0f, 3.0f));

	mat4x4_f32 result = mat4x4_f32_mul(identity, translation);

	// Identity * Translation should equal Translation
	vec3_f32 extracted = mat4x4_f32_get_translation(result);
	GLINT_TEST_CHECK_EQ_FLOAT(extracted.x, 1.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(extracted.y, 2.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(extracted.z, 3.0f, TEST_EPSILON_F32);
}

static void test_quat4_f32_identity(void)
{
	GLINT_TEST_SECTION("quat4_f32 identity");

	quat4_f32 q_identity = quat4_f32_identity();

	GLINT_TEST_CHECK_EQ_FLOAT(q_identity.x, 0.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(q_identity.y, 0.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(q_identity.z, 0.0f, TEST_EPSILON_F32);
	GLINT_TEST_CHECK_EQ_FLOAT(q_identity.w, 1.0f, TEST_EPSILON_F32);
}

static void test_quat4_f32_axis_angle(void)
{
	GLINT_TEST_SECTION("quat4_f32 axis angle");

	vec3_f32 axis  = vec3_f32_create(0.0f, 0.0f, 1.0f); // Z-axis
	f32		 angle = GLINT_DEG2RAD_F32(90.0f);			// 90 degrees

	quat4_f32 q_rot = quat4_f32_from_axis_angle(axis, angle);

	// For 90 degrees around Z-axis: w = cos(45°) ≈ 0.707, z = sin(45°) ≈ 0.707
	GLINT_TEST_CHECK(fabsf(q_rot.w - 0.707f) < 0.01f);
	GLINT_TEST_CHECK(fabsf(q_rot.z - 0.707f) < 0.01f);
	GLINT_TEST_CHECK(fabsf(q_rot.x) < 0.01f);
	GLINT_TEST_CHECK(fabsf(q_rot.y) < 0.01f);
}

static void test_vec3_f64_basic(void)
{
	GLINT_TEST_SECTION("vec3_f64 basic");

	vec3_f64 v3a = vec3_f64_create(1.0, 2.0, 3.0);
	vec3_f64 v3b = vec3_f64_create(4.0, 5.0, 6.0);

	f64 dot = vec3_f64_dot(v3a, v3b);
	GLINT_TEST_CHECK_EQ_FLOAT(dot, 32.0, TEST_EPSILON_F64); // 1*4 + 2*5 + 3*6 = 32

	vec3_f64 sum = vec3_f64_add(v3a, v3b);
	GLINT_TEST_CHECK_EQ_FLOAT(sum.x, 5.0, TEST_EPSILON_F64);
	GLINT_TEST_CHECK_EQ_FLOAT(sum.y, 7.0, TEST_EPSILON_F64);
	GLINT_TEST_CHECK_EQ_FLOAT(sum.z, 9.0, TEST_EPSILON_F64);
}

static void test_constants(void)
{
	GLINT_TEST_SECTION("math constants");

	GLINT_TEST_CHECK(glint_pi_f32 > 3.14f && glint_pi_f32 < 3.15f);
	GLINT_TEST_CHECK(glint_tau_f32 > 6.28f && glint_tau_f32 < 6.29f);

	f32 ninety_deg_rad = GLINT_DEG2RAD_F32(90.0f);
	GLINT_TEST_CHECK(ninety_deg_rad > 1.57f && ninety_deg_rad < 1.58f);

	f32 pi_rad_deg = GLINT_RAD2DEG_F32(glint_pi_f32);
	GLINT_TEST_CHECK(pi_rad_deg > 179.9f && pi_rad_deg < 180.1f);
}

static void register_math_tests(void) __attribute__((constructor));
static void register_math_tests(void)
{
	GLINT_TEST_REGISTER(test_vec2_f32_basic);
	GLINT_TEST_REGISTER(test_vec3_f32_basic);
	GLINT_TEST_REGISTER(test_vec4_f32_basic);
	GLINT_TEST_REGISTER(test_mat4x4_f32_identity);
	GLINT_TEST_REGISTER(test_mat4x4_f32_translate);
	GLINT_TEST_REGISTER(test_mat4x4_f32_mul);
	GLINT_TEST_REGISTER(test_quat4_f32_identity);
	GLINT_TEST_REGISTER(test_quat4_f32_axis_angle);
	GLINT_TEST_REGISTER(test_vec3_f64_basic);
	GLINT_TEST_REGISTER(test_constants);
}

#include "base/base.c"
