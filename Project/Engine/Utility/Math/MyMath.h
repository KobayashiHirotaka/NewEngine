/**
 * @file MyMath.h
 * @brief 数学関数の定義
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Utility/Structs.h"
#include <cmath>
#include <cassert>

//float
/// <summary>ベクトルの長さを取得</summary>
float Length(const Vector3& v);

/// <summary>線形補間</summary>
float Lerp(const float& v1, const float& v2, float t);

/// <summary>短角度での線形補間</summary>
float LerpShortAngle(const float& a, const float& b, float t);

/// <summary>EaseInOutQuadの補間関数</summary>
float EaseInOutQuad(float t);

/// <summary>EaseInOutCubicの補間関数</summary>
float EaseInOutCubic(float t);

//Vector3
/// <summary>加算</summary>
Vector3 Add(const Vector3& v1, const Vector3& v2);

/// <summary>減算</summary>
Vector3 Subtract(const Vector3& v1, const Vector3& v2);

/// <summary>積</summary>
Vector3 Multiply(const Vector3& v1, const Vector3& v2);
Vector3 Multiply(const float& v1, const Vector3& v2);

/// <summary>正規化/summary>
Vector3 Normalize(const Vector3& v);

/// <summary>ベクトルを行列で変換</summary>
Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix);

/// <summary>線形補完</summary>
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

//Matrix4x4
/// <summary>減算</summary>
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>積</summary>
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>逆行列</summary>
Matrix4x4 Inverse(const Matrix4x4& m);

/// <summary>単位行列を作成</summary>
Matrix4x4 MakeIdentity4x4();

/// <summary>平行移動行列を作成</summary>
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

/// <summary>スケーリング行列を作成</summary>
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

/// <summary>X軸回転行列を作成</summary>
Matrix4x4 MakeRotateXMatrix(float radian);

/// <summary>Y軸回転行列を作成</summary>
Matrix4x4 MakeRotateYMatrix(float radian);

/// <summary>Z軸回転行列を作成</summary>
Matrix4x4 MakeRotateZMatrix(float radian);

/// <summary>クォータニオンから回転行列を作成</summary>
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);

/// <summary>アフィン行列を作成</summary>
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& quaternion, const Vector3& translation);

/// <summary>透視投影行列を作成</summary>
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

/// <summary>直交投影行列を作成</summary>
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

/// <summary>行列の転置</summary>
Matrix4x4 Transpose(const Matrix4x4& m);

/// <summary>球面線形補間</summary>
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