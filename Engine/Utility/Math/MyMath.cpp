#include "Mymath.h"

//X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian)
{
	Matrix4x4 result;
	result.m[0][0] = 1.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}

//Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian)
{
	Matrix4x4 result;
	result.m[0][0] = std::cos(radian);
	result.m[0][1] = 0.0f;
	result.m[0][2] = -std::sin(radian);
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = 1.0f;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = std::sin(radian);
	result.m[2][1] = 0.0f;
	result.m[2][2] = std::cos(radian);
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}

//Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian)
{
	Matrix4x4 result;
	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = 1.0f;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}

//平行移動
Matrix4x4 MakeTranslateMatrix(Vector3 translate)
{
	Matrix4x4 result;
	result.m[0][0] = 1.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = 1.0f;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = 1.0f;
	result.m[2][3] = 0.0f;

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;

	return result;
};

//回転
Matrix4x4 MakeRotateMatrix(Vector3 rotation)
{
	Matrix4x4 result;
	result = MakeIdentity4x4();
	result = Multiply(MakeRotateXMatrix(rotation.x), Multiply(MakeRotateYMatrix(rotation.y), MakeRotateZMatrix(rotation.z)));
	return result;
}

//拡大縮小
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result;

	result.m[0][0] = scale.x;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = scale.y;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = scale.z;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

//アフィン変換
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 result;
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);

	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	result = Multiply(scaleMatrix, Multiply(rotateXYZMatrix, translateMatrix));
	return result;
}

//行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return result;
}

//行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
	return result;
}

//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			float Term = 0;
			for (int k = 0; k < 4; k++) {
				Term = Term + m1.m[i][k] * m2.m[k][j];
				result.m[i][j] = Term;
			}
		}
	}
	return result;
}

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m1) {
	Matrix4x4 result;
	float deterninant = m1.m[0][0] * m1.m[1][1] * m1.m[2][2] * m1.m[3][3] +
		m1.m[0][0] * m1.m[1][2] * m1.m[2][3] * m1.m[3][1] +
		m1.m[0][0] * m1.m[1][3] * m1.m[2][1] * m1.m[3][2] -

		m1.m[0][0] * m1.m[1][3] * m1.m[2][2] * m1.m[3][1] -
		m1.m[0][0] * m1.m[1][2] * m1.m[2][1] * m1.m[3][3] -
		m1.m[0][0] * m1.m[1][1] * m1.m[2][3] * m1.m[3][2] -

		m1.m[0][1] * m1.m[1][0] * m1.m[2][2] * m1.m[3][3] -
		m1.m[0][2] * m1.m[1][0] * m1.m[2][3] * m1.m[3][1] -
		m1.m[0][3] * m1.m[1][0] * m1.m[2][1] * m1.m[3][2] +

		m1.m[0][3] * m1.m[1][0] * m1.m[2][2] * m1.m[3][1] +
		m1.m[0][2] * m1.m[1][0] * m1.m[2][1] * m1.m[3][3] +
		m1.m[0][1] * m1.m[1][0] * m1.m[2][3] * m1.m[3][2] +

		m1.m[0][1] * m1.m[1][2] * m1.m[2][0] * m1.m[3][3] +
		m1.m[0][2] * m1.m[1][3] * m1.m[2][0] * m1.m[3][1] +
		m1.m[0][3] * m1.m[1][1] * m1.m[2][0] * m1.m[3][2] -

		m1.m[0][3] * m1.m[1][2] * m1.m[2][0] * m1.m[3][1] -
		m1.m[0][2] * m1.m[1][1] * m1.m[2][0] * m1.m[3][3] -
		m1.m[0][1] * m1.m[1][3] * m1.m[2][0] * m1.m[3][2] -

		m1.m[0][1] * m1.m[1][2] * m1.m[2][3] * m1.m[3][0] -
		m1.m[0][2] * m1.m[1][3] * m1.m[2][1] * m1.m[3][0] -
		m1.m[0][3] * m1.m[1][1] * m1.m[2][2] * m1.m[3][0] +

		m1.m[0][3] * m1.m[1][2] * m1.m[2][1] * m1.m[3][0] +
		m1.m[0][2] * m1.m[1][1] * m1.m[2][3] * m1.m[3][0] +
		m1.m[0][1] * m1.m[1][3] * m1.m[2][2] * m1.m[3][0];

	assert(deterninant != 0.0f);
	float deterninantRect = 1.0f / deterninant;

	result.m[0][0] =
		(m1.m[1][1] * m1.m[2][2] * m1.m[3][3] +
			m1.m[1][2] * m1.m[2][3] * m1.m[3][1] +
			m1.m[1][3] * m1.m[2][1] * m1.m[3][2] -
			m1.m[1][3] * m1.m[2][2] * m1.m[3][1] -
			m1.m[1][2] * m1.m[2][1] * m1.m[3][3] -
			m1.m[1][1] * m1.m[2][3] * m1.m[3][2]) * deterninantRect;
	result.m[0][1] =
		(-m1.m[0][1] * m1.m[2][2] * m1.m[3][3] -
			m1.m[0][2] * m1.m[2][3] * m1.m[3][1] -
			m1.m[0][3] * m1.m[2][1] * m1.m[3][2] +
			m1.m[0][3] * m1.m[2][2] * m1.m[3][1] +
			m1.m[0][2] * m1.m[2][1] * m1.m[3][3] +
			m1.m[0][1] * m1.m[2][3] * m1.m[3][2]) * deterninantRect;
	result.m[0][2] =
		(m1.m[0][1] * m1.m[1][2] * m1.m[3][3] +
			m1.m[0][2] * m1.m[1][3] * m1.m[3][1] +
			m1.m[0][3] * m1.m[1][1] * m1.m[3][2] -
			m1.m[0][3] * m1.m[1][2] * m1.m[3][1] -
			m1.m[0][2] * m1.m[1][1] * m1.m[3][3] -
			m1.m[0][1] * m1.m[1][3] * m1.m[3][2]) * deterninantRect;
	result.m[0][3] =
		(-m1.m[0][1] * m1.m[1][2] * m1.m[2][3] -
			m1.m[0][2] * m1.m[1][3] * m1.m[2][1] -
			m1.m[0][3] * m1.m[1][1] * m1.m[2][2] +
			m1.m[0][3] * m1.m[1][2] * m1.m[2][1] +
			m1.m[0][2] * m1.m[1][1] * m1.m[2][3] +
			m1.m[0][1] * m1.m[1][3] * m1.m[2][2]) * deterninantRect;

	result.m[1][0] =
		(-m1.m[1][0] * m1.m[2][2] * m1.m[3][3] -
			m1.m[1][2] * m1.m[2][3] * m1.m[3][0] -
			m1.m[1][3] * m1.m[2][0] * m1.m[3][2] +
			m1.m[1][3] * m1.m[2][2] * m1.m[3][0] +
			m1.m[1][2] * m1.m[2][0] * m1.m[3][3] +
			m1.m[1][0] * m1.m[2][3] * m1.m[3][2]) * deterninantRect;
	result.m[1][1] =
		(m1.m[0][0] * m1.m[2][2] * m1.m[3][3] +
			m1.m[0][2] * m1.m[2][3] * m1.m[3][0] +
			m1.m[0][3] * m1.m[2][0] * m1.m[3][2] -
			m1.m[0][3] * m1.m[2][2] * m1.m[3][0] -
			m1.m[0][2] * m1.m[2][0] * m1.m[3][3] -
			m1.m[0][0] * m1.m[2][3] * m1.m[3][2]) * deterninantRect;
	result.m[1][2] =
		(-m1.m[0][0] * m1.m[1][2] * m1.m[3][3] -
			m1.m[0][2] * m1.m[1][3] * m1.m[3][0] -
			m1.m[0][3] * m1.m[1][0] * m1.m[3][2] +
			m1.m[0][3] * m1.m[1][2] * m1.m[3][0] +
			m1.m[0][2] * m1.m[1][0] * m1.m[3][3] +
			m1.m[0][0] * m1.m[1][3] * m1.m[3][2]) * deterninantRect;
	result.m[1][3] =
		(m1.m[0][0] * m1.m[1][2] * m1.m[2][3] +
			m1.m[0][2] * m1.m[1][3] * m1.m[2][0] +
			m1.m[0][3] * m1.m[1][0] * m1.m[2][2] -
			m1.m[0][3] * m1.m[1][2] * m1.m[2][0] -
			m1.m[0][2] * m1.m[1][0] * m1.m[2][3] -
			m1.m[0][0] * m1.m[1][3] * m1.m[2][2]) * deterninantRect;

	result.m[2][0] =
		(m1.m[1][0] * m1.m[2][1] * m1.m[3][3] +
			m1.m[1][1] * m1.m[2][3] * m1.m[3][0] +
			m1.m[1][3] * m1.m[2][0] * m1.m[3][1] -
			m1.m[1][3] * m1.m[2][1] * m1.m[3][0] -
			m1.m[1][1] * m1.m[2][0] * m1.m[3][3] -
			m1.m[1][0] * m1.m[2][3] * m1.m[3][1]) * deterninantRect;
	result.m[2][1] =
		(-m1.m[0][0] * m1.m[2][1] * m1.m[3][3] -
			m1.m[0][1] * m1.m[2][3] * m1.m[3][0] -
			m1.m[0][3] * m1.m[2][0] * m1.m[3][1] +
			m1.m[0][3] * m1.m[2][1] * m1.m[3][0] +
			m1.m[0][1] * m1.m[2][0] * m1.m[3][3] +
			m1.m[0][0] * m1.m[2][3] * m1.m[3][1]) * deterninantRect;
	result.m[2][2] =
		(m1.m[0][0] * m1.m[1][1] * m1.m[3][3] +
			m1.m[0][1] * m1.m[1][3] * m1.m[3][0] +
			m1.m[0][3] * m1.m[1][0] * m1.m[3][1] -
			m1.m[0][3] * m1.m[1][1] * m1.m[3][0] -
			m1.m[0][1] * m1.m[1][0] * m1.m[3][3] -
			m1.m[0][0] * m1.m[1][3] * m1.m[3][1]) * deterninantRect;
	result.m[2][3] =
		(-m1.m[0][0] * m1.m[1][1] * m1.m[2][3] -
			m1.m[0][1] * m1.m[1][3] * m1.m[2][0] -
			m1.m[0][3] * m1.m[1][0] * m1.m[2][1] +
			m1.m[0][3] * m1.m[1][1] * m1.m[2][0] +
			m1.m[0][1] * m1.m[1][0] * m1.m[2][3] +
			m1.m[0][0] * m1.m[1][3] * m1.m[2][1]) * deterninantRect;

	result.m[3][0] =
		(-m1.m[1][0] * m1.m[2][1] * m1.m[3][2] -
			m1.m[1][1] * m1.m[2][2] * m1.m[3][0] -
			m1.m[1][2] * m1.m[2][0] * m1.m[3][1] +
			m1.m[1][2] * m1.m[2][1] * m1.m[3][0] +
			m1.m[1][1] * m1.m[2][0] * m1.m[3][2] +
			m1.m[1][0] * m1.m[2][2] * m1.m[3][1]) * deterninantRect;
	result.m[3][1] =
		(m1.m[0][0] * m1.m[2][1] * m1.m[3][2] +
			m1.m[0][1] * m1.m[2][2] * m1.m[3][0] +
			m1.m[0][2] * m1.m[2][0] * m1.m[3][1] -
			m1.m[0][2] * m1.m[2][1] * m1.m[3][0] -
			m1.m[0][1] * m1.m[2][0] * m1.m[3][2] -
			m1.m[0][0] * m1.m[2][2] * m1.m[3][1]) * deterninantRect;
	result.m[3][2] =
		(-m1.m[0][0] * m1.m[1][1] * m1.m[3][2] -
			m1.m[0][1] * m1.m[1][2] * m1.m[3][0] -
			m1.m[0][2] * m1.m[1][0] * m1.m[3][1] +
			m1.m[0][2] * m1.m[1][1] * m1.m[3][0] +
			m1.m[0][1] * m1.m[1][0] * m1.m[3][2] +
			m1.m[0][0] * m1.m[1][2] * m1.m[3][1]) * deterninantRect;
	result.m[3][3] =
		(m1.m[0][0] * m1.m[1][1] * m1.m[2][2] +
			m1.m[0][1] * m1.m[1][2] * m1.m[2][0] +
			m1.m[0][2] * m1.m[1][0] * m1.m[2][1] -
			m1.m[0][2] * m1.m[1][1] * m1.m[2][0] -
			m1.m[0][1] * m1.m[1][0] * m1.m[2][2] -
			m1.m[0][0] * m1.m[1][2] * m1.m[2][1]) * deterninantRect;

	return result;
}

//転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[j][i] = m.m[i][j];
		}
	}
	return result;
}

//単位行列
Matrix4x4 MakeIdentity4x4()
{
	Matrix4x4 result =
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
	return result;
}

float contangent(float b, float a) {
	return(b / tan(a));
}

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRadio, float nearClip, float farClip) {
	Matrix4x4 result;
	result.m[0][0] = contangent((1 / aspectRadio), (fovY / 2));
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = contangent(1, fovY / 2);
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = -nearClip * farClip / (farClip - nearClip);
	result.m[3][3] = 0.0f;

	return result;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth)
{
	Matrix4x4 result = {
	width / 2,0,0,0,
	0,-(height / 2),0,0,
	0,0,maxDepth - minDepth,0,
	left + (width / 2),top + (height / 2),minDepth,1
	};
	return result;
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farCcip)
{
	Matrix4x4 result;
	float dx = right - left;
	float dy = top - bottom;
	float dz = farCcip - nearClip;
	result = {
		2.0f / dx,0.0f,0.0f,0.0f,
		0.0f,2.0f / dy,0.0f,0.0f,
		0.0f,0.0f,-2.0f / dz,0.0f,
		-((right + left) / dx),-((top + bottom) / dy),-((farCcip + nearClip) / dz),1.0f
	};
	return result;
}

Vector3 Add(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vector3 Subtract(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

Vector3 Multiply(const float& v1, const Vector3& v2)
{
	Vector3 result{};

	result.x = v1 * v2.x;
	result.y = v1 * v2.y;
	result.z = v1 * v2.z;

	return result;
}

Vector3 Normalize(const Vector3& v)
{
	Vector3 result{};

	float date = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	if (date != 0) {
		result.x = v.x / date;
		result.y = v.y / date;
		result.z = v.z / date;
	}
	return result;
}

Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix)
{
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2];
	return result;
}

float Length(const Vector3& v)
{
	float result{};
	result = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return result;
}

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle)
{
	Matrix4x4 result{};

	result.m[0][0] = axis.x * axis.x * (1 - std::cos(angle)) + std::cos(angle);
	result.m[0][1] = axis.x * axis.y * (1 - std::cos(angle)) + axis.z * std::sin(angle);
	result.m[0][2] = axis.x * axis.z * (1 - std::cos(angle)) - axis.y * std::sin(angle);
	result.m[0][3] = 0.0f;

	result.m[1][0] = axis.x * axis.y * (1 - std::cos(angle)) - axis.z * std::sin(angle);
	result.m[1][1] = axis.y * axis.y * (1 - std::cos(angle)) + std::cos(angle);
	result.m[1][2] = axis.y * axis.z * (1 - std::cos(angle)) + axis.x * std::sin(angle);
	result.m[1][3] = 0.0f;

	result.m[2][0] = axis.x * axis.z * (1 - std::cos(angle)) + axis.y * std::sin(angle);
	result.m[2][1] = axis.y * axis.z * (1 - std::cos(angle)) -  axis.x * std::sin(angle);
	result.m[2][2] = axis.z * axis.z * (1 - std::cos(angle)) + std::cos(angle);
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

float Dot(const Vector3& v1, const Vector3& v2)
{
	float result{};

	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

	return result;
}

Vector3 Cross(const Vector3& v1, const Vector3& v2)
{
	Vector3 result{};

	result.x = (v1.y * v2.z) - (v1.z * v2.y);
	result.y = (v1.z * v2.x) - (v1.x * v2.z);
	result.z = (v1.x * v2.y) - (v1.y * v2.x);

	return result;
}


Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to)
{
	Matrix4x4 result{};

	Vector3 n = Normalize(Cross(from, to));

	if (from.x == -to.x && from.y == -to.y && from.z == -to.z)
	{
		if (from.x != 0.0f || from.y != 0.0f)
		{
			n = { from.y,-from.x,0.0f };
		}
		else if (from.x != 0.0f || from.z != 0.0f)
		{
			n = { from.z,0.0f,-from.x };
		}
	}

	float cosTheta = Dot(from, to);
	float sinTheta = Length(Cross(from, to));

	result.m[0][0] = n.x * n.x * (1 - cosTheta) + cosTheta;
	result.m[0][1] = n.x * n.y * (1 - cosTheta) + n.z * sinTheta;
	result.m[0][2] = n.x * n.z * (1 - cosTheta) - n.y * sinTheta;
	result.m[0][3] = 0.0f;

	result.m[1][0] = n.x * n.y * (1 - cosTheta) - n.z * sinTheta;
	result.m[1][1] = n.y * n.y * (1 - cosTheta) + cosTheta;
	result.m[1][2] = n.y * n.z * (1 - cosTheta) + n.x * sinTheta;
	result.m[1][3] = 0.0f;

	result.m[2][0] = n.x * n.z * (1 - cosTheta) + n.y * sinTheta;
	result.m[2][1] = n.y * n.z * (1 - cosTheta) - n.x * sinTheta;
	result.m[2][2] = n.z * n.z * (1 - cosTheta) + cosTheta;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs)
{
	Quaternion result{};

	Vector3 cross = Cross({ lhs.x,lhs.y ,lhs.z }, { rhs.x,rhs.y,rhs.z });
	float dot = Dot({ lhs.x,lhs.y ,lhs.z }, { rhs.x,rhs.y,rhs.z });

	result.x = cross.x + rhs.w * lhs.x + lhs.w * rhs.x;
	result.y = cross.y + rhs.w * lhs.y + lhs.w * rhs.y;
	result.z = cross.z + rhs.w * lhs.z + lhs.w * rhs.z;
	result.w = lhs.w * rhs.w - dot;

	return result;
}

Quaternion IdentityQuaternion()
{
	Quaternion result{};

	result.x = 0.0f;
	result.y = 0.0f;
	result.z = 0.0f;
	result.w = 1.0f;

	return result;
}

Quaternion Conjugate(const Quaternion& quaternion)
{
	Quaternion result{};

	result.x = quaternion.x * -1.0f;
	result.y = quaternion.y * -1.0f;
	result.z = quaternion.z * -1.0f;
	result.w = quaternion.w;

	return result;
}

float Norm(const Quaternion& quaternion)
{
	float result{};

	result = sqrtf(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);

	return result;
}

Quaternion Normalize(const Quaternion& quaternion)
{
	Quaternion result{};

	float norm = Norm(quaternion);

	if (norm != 0.0f)
	{
		result.x = quaternion.x / norm;
		result.y = quaternion.y / norm;
		result.z = quaternion.z / norm;
		result.w = quaternion.w / norm;
	}

	return result;
}

Quaternion Inverse(const Quaternion& quaternion)
{
	Quaternion result{};

	float norm = Norm(quaternion);
	Quaternion conj = Conjugate(quaternion);

	if (norm != 0.0f)
	{
		float normSquared = norm * norm;

		result.x = conj.x / normSquared;
		result.y = conj.y / normSquared;
		result.z = conj.z / normSquared;
		result.w = conj.w / normSquared;
	}

	return result;
}

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle)
{
	Quaternion result{};

	float halfAngle = angle / 2.0f;
	float sin = std::sin(halfAngle);

	result.x = axis.x * sin;
	result.y = axis.y * sin;
	result.z = axis.z * sin;
	result.w = std::cos(halfAngle);

	return result;
}

Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion)
{
	Vector3 result{};

	Quaternion fromVector = { vector.x,vector.y,vector.z,0.0f };
	Quaternion conj = Conjugate(quaternion);
	Quaternion rotate = Multiply(quaternion, Multiply(fromVector, conj));

	result.x = rotate.x;
	result.y = rotate.y;
	result.z = rotate.z;

	return result;
}

Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion)
{
	Matrix4x4 result{};

	result.m[0][0] = (quaternion.w * quaternion.w) + (quaternion.x * quaternion.x) - (quaternion.y * quaternion.y) - (quaternion.z * quaternion.z);
	result.m[0][1] = 2.0f * ((quaternion.x * quaternion.y) + (quaternion.w * quaternion.z));
	result.m[0][2] = 2.0f * ((quaternion.x * quaternion.z) - (quaternion.w * quaternion.y));
	result.m[0][3] = 0.0f;

	result.m[1][0] = 2.0f * ((quaternion.x * quaternion.y) - (quaternion.w * quaternion.z));
	result.m[1][1] = (quaternion.w * quaternion.w) - (quaternion.x * quaternion.x) + (quaternion.y * quaternion.y) - (quaternion.z * quaternion.z);
	result.m[1][2] = 2.0f * ((quaternion.y * quaternion.z) + (quaternion.w * quaternion.x));
	result.m[1][3] = 0.0f;

	result.m[2][0] = 2.0f * ((quaternion.x * quaternion.z) + (quaternion.w * quaternion.y));
	result.m[2][1] = 2.0f * ((quaternion.y * quaternion.z) - (quaternion.w * quaternion.x));
	result.m[2][2] = (quaternion.w * quaternion.w) - (quaternion.x * quaternion.x) - (quaternion.y * quaternion.y) + (quaternion.z * quaternion.z);
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

Vector3 TransformVectorByMatrix(const Vector3& vector, const Matrix4x4& matrix)
{
	Vector3 result{};

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];

	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

	assert(w != 0.0f);

	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{
	Quaternion result{};

	Quaternion localQ0 = q0;
	Quaternion localQ1 = q1;

	float dot = (localQ0.x * localQ1.x) + (localQ0.y * localQ1.y) + (localQ0.z * localQ1.z) + (localQ0.w * localQ1.w);

	if (dot < 0.0f)
	{
		localQ0 = { -localQ0.x,-localQ0.y ,-localQ0.z ,-localQ0.w };
		dot = -dot;
	}

	if (dot >= 1.0f - std::numeric_limits<float>::epsilon())
	{
		result.x = (1.0f - t) * localQ0.x + t * localQ1.x;
		result.y = (1.0f - t) * localQ0.y + t * localQ1.y;
		result.z = (1.0f - t) * localQ0.z + t * localQ1.z;
		result.w = (1.0f - t) * localQ0.w + t * localQ1.w;
		return result;
	}

	float theta = std::acos(dot);
	
	float scale0 = std::sin((1 - t) * theta) / std::sin(theta);
	float scale1 = std::sin(t * theta) / std::sin(theta);

	result.x = scale0 * localQ0.x + scale1 * localQ1.x;
	result.y = scale0 * localQ0.y + scale1 * localQ1.y;
	result.z = scale0 * localQ0.z + scale1 * localQ1.z;
	result.w = scale0 * localQ0.w + scale1 * localQ1.w;

	return result;
}
