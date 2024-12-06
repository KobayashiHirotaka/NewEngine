#pragma once
#include <chrono>

/**
 * @file GameTimer.h
 * @brief ゲーム中の時間の管理(更新など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

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

