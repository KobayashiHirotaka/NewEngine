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

	struct RangeFloat
	{
		float min;
		float max;
	};

	struct RangeVector3
	{
		Vector3 min;
		Vector3 max;
	};

	struct RangeVector4
	{
		Vector4 min;
		Vector4 max;
	};

	void Initialize();

	void Update();

	std::list<std::unique_ptr<BaseParticle>>& GetParticles() { return particles_; };

	bool GetIsDead() { return isDead_; };

	void SetIsDead() { isDead_ = true; };

	const std::string& GetName() { return name_; };

	void SetTranslation(const Vector3& translation) { translation_ = translation; };

	void SetPopCount(uint32_t count) { particleCount_ = count; };

	void SetPopArea(const Vector3& min, const Vector3& max) { area_ = { min,max }; };

	void SetPopAzimuth(float min, float max) { azimuth_ = { min,max }; }

	void SetPopVelocity(const Vector3& min, const Vector3& max) { area_ = { min,max }; };

private:
	void EmitParticle();

private:
	std::list<std::unique_ptr<BaseParticle>> particles_;
	
	std::string name_ = "nameless";
	
	ParticleType particleType_ = ParticleType::kBase;
	
	Vector3 translation_ = { 0.0f,0.0f,0.0f };

	RangeVector3 rotation_ = { {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	RangeVector3 scale_ = { {1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f} };

	RangeVector3 velocity_ = { {1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f} };
	
	RangeVector3 area_ = { {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	RangeVector4 color_ = { {1.0f,1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f,1.0f} };

	RangeFloat azimuth_ = { 0.0f,360.0f };

	RangeFloat elevation_ = { 0.0f,180.0f };
	
	RangeFloat lifeTime_ = { 0.5f,1.0f };

	uint32_t particleCount_ = 1;

	float frequency_ = 0.1f;

	float frequencyTime_ = frequency_;
	
	float deleteTime_ = 10.0f;

	float deleteTimer_ = 0.0f;

	bool isDead_ = false;

	friend class EmitterBuilder;
};