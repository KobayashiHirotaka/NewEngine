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
	/// <summary>開始</summary>
	void Start(const float time);

	/// <summary>更新</summary>
	void Update();

private:
	//時間
	float time_ = 0.0f;;

	//ヒットストップが有効かどうか
	bool isActiive_ = false;
};

