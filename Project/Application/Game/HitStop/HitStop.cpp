/**
 * @file HitStop.cpp
 * @brief ヒットストップの開始と更新処理を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "HitStop.h"

void HitStop::Start(const float time)
{
	//HitStopの初期設定
	GameTimer::SetTimeScale(0.0f);
	time_ = time;
	isActiive_ = true;
}

void HitStop::Update()
{
	//ヒットストップ
	if (isActiive_)
	{
		//時間を減らす
		const float deltaTime = 1.0f / 60.0f;
		time_ -= deltaTime;

		if (time_ <= 0)
		{
			//HitStop前に戻す
			time_ = 0.0f;
			GameTimer::SetTimeScale(1.0f);
			isActiive_ = false;
		}
	}
}
