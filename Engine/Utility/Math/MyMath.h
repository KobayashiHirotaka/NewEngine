#pragma once
#include <cmath>
#include <cassert>

//structs
struct Vector2
{
	float x;
	float y;
};

struct Vector3
{
	float x;
	float y;
	float z;
};

struct Vector4
{
	float x;
	float y;
	float z;
	float w;
};

struct Matrix4x4
{
	float m[4][4];
};

struct AABB
{
	Vector3 min;
	Vector3 max;
};

//float
float Length(const Vector3& v);

float Lerp(const float& v1, const float& v2, float t);

//Vector3
Vector3 Add(const Vector3& v1, const Vector3& v2);

Vector3 Subtract(const Vector3& v1, const Vector3& v2);

Vector3 Multiply(const Vector3& v1, const Vector3& v2);

Vector3 Multiply(const float& v1, const Vector3& v2);

Vector3 Normalize(const Vector3& v);

Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix);

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

//Matrix4x4
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

Matrix4x4 Inverse(const Matrix4x4& m);

Matrix4x4 MakeIdentity4x4();

Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

Matrix4x4 MakeScaleMatrix(const Vector3& scale);

Matrix4x4 MakeRotateXMatrix(float radian);

Matrix4x4 MakeRotateYMatrix(float radian);

Matrix4x4 MakeRotateZMatrix(float radian);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);