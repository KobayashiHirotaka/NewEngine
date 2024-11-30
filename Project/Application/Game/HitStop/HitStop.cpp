#include "HitStop.h"

float HitStop::time_ = 0.0f;

void HitStop::Initialize(const float time)
{
	GameTimer::SetTimeScale(0.0f);
	time_ = time;
}

void HitStop::Update()
{
	const float deltaTime = 1.0f / 60.0f;
	time_ -= deltaTime;

	if (time_ <= 0)
	{
		time_ = 0.0f;
		GameTimer::SetTimeScale(1.0f);
	}
}
