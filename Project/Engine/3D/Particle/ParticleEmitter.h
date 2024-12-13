/**
 * @file ParticleEmitter.h
 * @brief エミッターに設定されたパラメータに基づいてパーティクルを生成、更新を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Particle.h"
#include "Engine/3D/Particle/Random.h"
#include <list>
#include <memory>
#include <numbers>
#include <string>

class ParticleEmitter
{
public:
	//パーティクルの種類
	enum class ParticleType
	{
		//基本
		kBase,

		//通常
		kNormal,
	};

	//Float型の範囲
	struct FloatRange
	{
		//範囲の最小値
		float min; 

		//範囲の最大値
		float max; 
	};

	//3次元ベクトルの範囲
	struct Vector3Range
	{
		//範囲の最小値
		Vector3 min; 

		//範囲の最大値
		Vector3 max; 
	};

	//4次元ベクトルの範囲
	struct Vector4Range
	{
		//範囲の最小値
		Vector4 min;

		//範囲の最大値
		Vector4 max;
	};

	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>更新</summary>
	void Update();


	//Getter
	//Particle
	std::list<std::unique_ptr<BaseParticle>>& GetParticles() { return particles_; };

	//IsDead
	bool GetIsDead() { return isDead_; };

	//EmitterName
	const std::string& GetEmitterName() { return emitterName_; };

	//Setter
	//IsDead
	void SetIsDead() { isDead_ = true; };

	//Translation
	void SetTranslation(const Vector3& translation) { translation_ = translation; };

	//articleCount
	void SetParticleCount(uint32_t count) { particleCount_ = count; };

	//Area
	void SetArea(const Vector3& min, const Vector3& max) { area_ = { min,max }; };

	//Azimuth
	void SetAzimuth(float min, float max) { azimuth_ = { min,max }; }

	//Velocity
	void SetVelocity(const Vector3& min, const Vector3& max) { area_ = { min,max }; };

private:
	void EmitParticle();

private:
	//パーティクルのリスト
	std::list<std::unique_ptr<BaseParticle>> particles_;

	//エミッターの設定項目
	//エミッター名
	std::string emitterName_ = " ";

	//パーティクルの種類
	ParticleType particleType_ = ParticleType::kBase;

	//パーティクルの位置
	Vector3 translation_ = { 0.0f, 0.0f, 0.0f };

	//回転の範囲
	Vector3Range rotation_ = { {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	//スケールの範囲
	Vector3Range scale_ = { {1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f} };

	//速度の範囲
	Vector3Range velocity_ = { {1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f} };

	//発生範囲の範囲
	Vector3Range area_ = { {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	//色の範囲
	Vector4Range color_ = { {1.0f,1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f,1.0f} };

	//方位角の範囲
	FloatRange azimuth_ = { 0.0f, 360.0f };

	//仰角の範囲
	FloatRange elevation_ = { 0.0f, 180.0f };

	//パーティクルの生存時間の範囲
	FloatRange lifeTime_ = { 0.5f, 1.0f };

	//パーティクルの個数
	uint32_t particleCount_ = 1;

	//パーティクル発生頻度
	float frequency_ = 0.1f;

	//現在の発生タイマー
	float frequencyTime_ = frequency_;

	//パーティクルの削除までの時間
	float deleteTime_ = 10.0f;

	//削除タイマー
	float deleteTimer_ = 0.0f;

	//エミッターが死んでいるかのフラグ
	bool isDead_ = false;

	//friendクラスとして設定
	friend class EmitterBuilder;
};