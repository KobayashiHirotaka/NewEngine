#pragma once

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


struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};