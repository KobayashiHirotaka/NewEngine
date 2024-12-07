#include "HitStop.h"

/**
 * @file HitStop.cpp
 * @brief ヒットストップの管理(開始、更新など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

void HitStop::Start(const float time)
{
	GameTimer::SetTimeScale(0.0f);
	time_ = time;
	isActiive_ = true;
}

void HitStop::Update()
{
	if (isActiive_)
	{
		const float deltaTime = 1.0f / 60.0f;
		time_ -= deltaTime;

		if (time_ <= 0)
		{
			time_ = 0.0f;
			GameTimer::SetTimeScale(1.0f);
			isActiive_ = false;
		}
	}
}
