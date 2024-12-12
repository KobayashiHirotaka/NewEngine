/**
 * @file GameTimer.cpp
 * @brief ゲーム中の時間(経過時間)の管理を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "GameTimer.h"

float GameTimer::sDeltaTime_ = 0.0f;
float GameTimer::sTimeScale_ = 1.0f;

void GameTimer::Update()
{
	//1フレームあたりの時間
	const float deltaTime = 1.0f / 60.0f;
	sDeltaTime_ = deltaTime;

	//時間スケールを反映
	sDeltaTime_ *= sTimeScale_;
}
