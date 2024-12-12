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
	/// <summary>更新</summary>
	static void Update();

	//DeltaTimeのGetter
	static float GetDeltaTime() { return sDeltaTime_; };

	//TimeScaleのGetter
	static void SetTimeScale(float timeScale) { sTimeScale_ = timeScale; };

private:
	//経過時間
	static float sDeltaTime_;

	//時間スケール
	static float sTimeScale_;
};

