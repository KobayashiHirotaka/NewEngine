/**
 * @file HitStop.cpp
 * @brief ヒットストップの開始と更新処理を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "HitStop.h"

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
