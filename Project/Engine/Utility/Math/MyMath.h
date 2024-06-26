#pragma once
#include "Engine/Utility/Structs.h"
#include <cmath>
#include <cassert>

//float
float Length(const Vector3& v);

float Lerp(const float& v1, const float& v2, float t);

float LerpShortAngle(const float& a, const float& b, float t);

float EaseInOutQuad(float t);

float EaseInOutCubic(float t);

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

Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& quaternion, const Vector3& translation);

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

Matrix4x4 Transpose(const Matrix4x4& m);

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

//演算子オーバーロード
Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator+(const Vector3& v1, float s);
Vector3 operator-(const Vector3& v1, float s);
Vector3 operator*(const Vector3& v, float s);
Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);

Vector3 operator-(const Vector3& v);
Vector3 operator+(const Vector3& v);