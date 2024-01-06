#pragma once
#include "Particle.h"
#include "ChargeParticle.h"
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
		kNormal,
		kScale,
		kCharge,
	};

	struct minmaxStructVector4 
	{
		Vector4 min;
		Vector4 max;
	};

	struct minmaxStructVector3
	{
		Vector3 min;
		Vector3 max;
	};

	struct minmaxStructFloat 
	{
		float min;
		float max;
	};

	void Initialize();

	void Update();

	std::list<std::unique_ptr<BaseParticle>>& GetParticles() { return particles_; };

	bool GetIsDead() { return isDead_; };

	void SetIsDead() { isDead_ = true; };

	const std::string& GetName() { return name_; };

	void SetTranslation(const Vector3& translation) { popTranslation_ = translation; };

	void SetPopCount(uint32_t count) { popCount_ = count; };

	void SetPopArea(const Vector3& min, const Vector3& max) { popArea_ = { min,max }; };

	void SetPopAzimuth(float min, float max) { popAzimuth = { min,max }; }

	void SetPopVelocity(const Vector3& min, const Vector3& max) { popArea_ = { min,max }; };

private:
	void Pop();

private:
	std::list<std::unique_ptr<BaseParticle>> particles_{};
	
	std::string name_ = "nameless";
	
	ParticleType particleType_ = ParticleType::kNormal;
	
	Vector3 popTranslation_ = { 0.0f,0.0f,0.0f };
	
	minmaxStructVector3 popArea_ = { {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	
	minmaxStructVector3 popRotation_ = { {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	
	minmaxStructVector3 popScale_ = { {1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f} };

	minmaxStructFloat popAzimuth = { 0.0f,360.0f };

	minmaxStructFloat popElevation = { 0.0f,180.0f };
	
	minmaxStructVector3 popVelocity_ = { {1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f} };
	
	minmaxStructVector4 popColor_ = { {1.0f,1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f,1.0f} };
	
	minmaxStructFloat popLifeTime_ = { 0.5f,1.0f };

	uint32_t popCount_ = 1;

	float popFrequency_ = 0.1f;

	float frequencyTime_ = popFrequency_;
	
	float deleteTime_ = 10.0f;

	float deleteTimer_ = 0.0f;

	bool isDead_ = false;

	friend class EmitterBuilder;
};