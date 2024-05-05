#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include <math.h>
#include <map>
#include <optional>

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

struct Quaternion 
{
	float x, y, z, w;
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

struct VertexWeightData
{
	float weight;
	uint32_t vertexIndex;
};

struct JointWeightData
{
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};

//Keyframe構造体
template <typename tValue>

struct Keyframe 
{
	float time;//キーフレームの時間(単位：秒)
	tValue value;//キーフレームの値
};

using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;


//KeyframeをNodeごとにまとめる
template <typename tValue>
struct AnimationCurve 
{
	std::vector<Keyframe<tValue>> keyframes;
};

struct NodeAnimation
{
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};

struct Animation 
{
	float duration;//アニメーションの全体の長さ(単位：秒)

	//NodeAnimationの集合、Node名で引けるようにする
	std::map<std::string, NodeAnimation> nodeAnimations;
};

struct Node
{
	Vector3 translate;
	Quaternion rotate;
	Vector3 scale;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

struct MaterialData
{
	std::string textureFilePath;
};

struct ModelData
{
	std::map<std::string, JointWeightData> skinClusterData;
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	MaterialData material;
	std::string name;
	Node rootNode;
};

struct ModelTransformationData
{
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct Joint
{
	Vector3 translate;
	Quaternion rotate;
	Vector3 scale;
	Matrix4x4 localMatrix;
	Matrix4x4 skeletonSpaceMatrix;
	std::string name;
	std::vector<int32_t> children;
	int32_t index;
	std::optional<int32_t> parent;
};

struct Skeleton
{
	int32_t root;
	std::map<std::string, int32_t> jointMap;
	std::vector<Joint> joints;
};