#pragma once
#include "Engine/Utility/Structs/Matrix4x4.h"
#include "Engine/Utility/Structs/Vector2.h"
#include "Engine/Utility/Structs/Vector3.h"
#include "Engine/Utility/Structs/Vector4.h"
#include <assert.h>
#include <cmath>
#include <numbers>
#include <limits>

struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct AABB
{
	Vector3 min;
	Vector3 max;
};

struct Quaternion
{
	float x;
	float y;
	float z;
	float w;
};


//X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);

//Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);

//Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

//平行移動
Matrix4x4 MakeTranslateMatrix(Vector3 translate);

//回転
Matrix4x4 MakeRotateMatrix(Vector3 rotation);

//拡大縮小
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// アフィン変換
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

//行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

//行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);

//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m1);

//転置行列
Matrix4x4 Transpose(const Matrix4x4& m);

//単位行列
Matrix4x4 MakeIdentity4x4();

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRadio, float nearClip, float farClip);

//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farCcip);

Vector3 Add(const Vector3& v1, const Vector3& v2);

Vector3 Subtract(const Vector3& v1, const Vector3& v2);

Vector3 Multiply(const float& v1, const Vector3& v2);

Vector3 Normalize(const Vector3& v);

Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix);

float Length(const Vector3& v);

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);

float Dot(const Vector3& v1, const Vector3& v2);

Vector3 Cross(const Vector3& v1, const Vector3& v2);

Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

Quaternion IdentityQuaternion();

Quaternion Conjugate(const Quaternion& quaternion);

float Norm(const Quaternion& quaternion);

Quaternion Normalize(const Quaternion& quaternion);

Quaternion Inverse(const Quaternion& quaternion);

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);

Vector3 TransformVectorByMatrix(const Vector3& vector, const Matrix4x4& matrix);

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

