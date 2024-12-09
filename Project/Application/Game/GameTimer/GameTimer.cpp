/**
 * @file GameTimer.cpp
 * @brief ゲーム中の時間の管理(更新など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "GameTimer.h"

float GameTimer::deltaTime_ = 0.0f;
float GameTimer::timeScale_ = 1.0f;

void GameTimer::Update()
{
	deltaTime_ = 1.0f / 60.0f;

	deltaTime_ *= timeScale_;
}
