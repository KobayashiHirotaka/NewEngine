/**
 * @file GameTimer.h
 * @brief ゲーム中の時間(経過時間)の管理を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include <chrono>

class GameTimer
{
public:
	//void Initialize();

	static void Update();

	static float GetDeltaTime() { return deltaTime_; };

	static void SetTimeScale(float timeScale) { timeScale_ = timeScale; };

private:
	static float deltaTime_;

	static float timeScale_;
};

