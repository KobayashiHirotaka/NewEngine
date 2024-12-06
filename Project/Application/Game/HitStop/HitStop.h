#pragma once
#include "Application/Game/GameTimer/GameTimer.h"

/**
 * @file HitStop.h
 * @brief ヒットストップの管理(開始、更新など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

class HitStop
{
public:
	void Start(const float time);

	void Update();

private:
	float time_ = 0.0f;;

	bool isActiive_ = false;
};

