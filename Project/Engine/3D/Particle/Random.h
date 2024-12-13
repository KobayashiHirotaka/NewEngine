/**
 * @file Random.h
 * @brief 乱数生成を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include <random>

class Random
{
public:
	/// <summary>初期化</summary>
	static void Initialize();

	/// <summary>指定された範囲でランダムな整数を取得</summary>
	static int GetRandomInt(int min, int max);

	/// <summary>指定された範囲でランダムな浮動小数点数を取得</summary>
	static float GetRandomFloat(float min, float max);

private:
	//ランダム数を生成するための乱数エンジン
	static std::mt19937 sRandomEngine_;
};

