#include "GameTimer.h"

float GameTimer::deltaTime_ = 0.0f;
float GameTimer::timeScale_ = 1.0f;

void GameTimer::Update()
{
	deltaTime_ = 1.0f / 60.0f;

	deltaTime_ *= timeScale_;
}
