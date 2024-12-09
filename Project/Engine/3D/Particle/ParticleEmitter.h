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
	enum class ParticleType
	{
		kBase,
		kNormal,
	};

	struct FloatRange
	{
		float min;
		float max;
	};

	struct Vector3Range
	{
		Vector3 min;
		Vector3 max;
	};

	struct Vector4Range
	{
		Vector4 min;
		Vector4 max;
	};

	void Initialize();

	void Update();

	std::list<std::unique_ptr<BaseParticle>>& GetParticles() { return particles_; };

	bool GetIsDead() { return isDead_; };

	void SetIsDead() { isDead_ = true; };

	const std::string& GetEmitterName() { return emitterName_; };

	void SetTranslation(const Vector3& translation) { translation_ = translation; };

	void SetParticleCount(uint32_t count) { particleCount_ = count; };

	void SetArea(const Vector3& min, const Vector3& max) { area_ = { min,max }; };

	void SetAzimuth(float min, float max) { azimuth_ = { min,max }; }

	void SetVelocity(const Vector3& min, const Vector3& max) { area_ = { min,max }; };

private:
	void EmitParticle();

private:
	std::list<std::unique_ptr<BaseParticle>> particles_;

	std::string emitterName_ = " ";

	//Emitterの設定項目
	ParticleType particleType_ = ParticleType::kBase;

	Vector3 translation_ = { 0.0f,0.0f,0.0f };

	Vector3Range rotation_ = { {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Vector3Range scale_ = { {1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f} };

	Vector3Range velocity_ = { {1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f} };

	Vector3Range area_ = { {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Vector4Range color_ = { {1.0f,1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f,1.0f} };

	FloatRange azimuth_ = { 0.0f,360.0f };

	FloatRange elevation_ = { 0.0f,180.0f };

	FloatRange lifeTime_ = { 0.5f,1.0f };

	uint32_t particleCount_ = 1;

	float frequency_ = 0.1f;

	float frequencyTime_ = frequency_;

	float deleteTime_ = 10.0f;

	float deleteTimer_ = 0.0f;

	bool isDead_ = false;

	friend class EmitterBuilder;
};