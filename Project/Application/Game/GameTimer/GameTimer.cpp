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
	sDeltaTime_ = 1.0f / 60.0f;

	sDeltaTime_ *= sTimeScale_;
}
