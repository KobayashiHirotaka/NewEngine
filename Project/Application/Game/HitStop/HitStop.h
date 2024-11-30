#pragma once
#include "Application/Game/GameTimer/GameTimer.h"

class HitStop
{
public:
	static void Initialize(const float time);

	static void Update();

private:
	static float time_;
};

