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

