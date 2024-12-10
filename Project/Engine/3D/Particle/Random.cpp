/**
 * @file Random.cpp
 * @brief 乱数生成を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Random.h"

std::mt19937 Random::sRandomEngine_;

void Random::Initialize()
{
	std::random_device seedGenerator;
	sRandomEngine_ = std::mt19937(seedGenerator());
}

int Random::GetRandomInt(int min, int max)
{
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(sRandomEngine_);
}

float Random::GetRandomFloat(float min, float max)
{
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(sRandomEngine_);
}