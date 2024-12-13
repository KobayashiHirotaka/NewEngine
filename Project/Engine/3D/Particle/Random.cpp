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
	//std::random_deviceを使用してシードを生成
	std::random_device seedGenerator;

	//乱数エンジンを初期化
	sRandomEngine_ = std::mt19937(seedGenerator());
}

int Random::GetRandomInt(int min, int max)
{
	//指定された範囲でランダムな整数を生成する
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(sRandomEngine_);
}

float Random::GetRandomFloat(float min, float max)
{
	//指定された範囲でランダムな浮動小数点数を生成する
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(sRandomEngine_);
}