#pragma once
#include "Engine/Utility/Math/MyMath.h"
#include <vector>
#include <map>
#include <string>

class KeyframeAnimation
{
	template<typename tValue>

	struct Keyframe
	{
		float time;
		tValue value;
	};

	using KeyframeVector3 = Keyframe<Vector3>;
	using KeyframeQuaternion = Keyframe<Quaternion>;

	template<typename tValue>

	struct AnimationCurve
	{
		std::vector<Keyframe<tValue>> keyframes;
	};

	struct  NodeAnimation
	{
		std::vector<Vector3> translate;
		std::vector<Quaternion> rotate;
		std::vector<Vector3> scale;
	};

	struct Animation
	{
		float duration;

		std::map<std::string, NodeAnimation> nodeAnimations;
	};
};

