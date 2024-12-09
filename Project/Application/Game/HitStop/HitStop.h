/**
 * @file HitStop.h
 * @brief ヒットストップの開始と更新処理を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

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

