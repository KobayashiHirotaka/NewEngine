#pragma once
#include "Application/Game/GameTimer/GameTimer.h"

class HitStop
{
public:
	void Start(const float time);

	void Update();

private:
	float time_ = 0.0f;;

	bool isActiive_ = false;
};

