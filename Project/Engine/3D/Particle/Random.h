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
	static void Initialize();

	static int GetRandomInt(int min, int max);

	static float GetRandomFloat(float min, float max);

private:
	static std::mt19937 sRandomEngine_;
};

