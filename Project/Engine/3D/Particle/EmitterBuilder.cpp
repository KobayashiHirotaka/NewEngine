#include "EmitterBuilder.h"

EmitterBuilder::EmitterBuilder()
{
	particleEmitter_ = new ParticleEmitter();
}

EmitterBuilder::~EmitterBuilder()
{

}

EmitterBuilder& EmitterBuilder::SetEmitterName(const std::string& emitterName)
{
	particleEmitter_->emitterName_ = emitterName;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetParticleType(ParticleEmitter::ParticleType particleType)
{
	particleEmitter_->particleType_ = particleType;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetTranslation(const Vector3& translation)
{
	particleEmitter_->translation_ = translation;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetArea(const Vector3& min, const Vector3& max)
{
	particleEmitter_->area_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetRotation(const Vector3& min, const Vector3& max)
{
	particleEmitter_->rotation_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetScale(const Vector3& min, const Vector3& max)
{
	particleEmitter_->scale_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetAzimuth(float min, float max)
{
	particleEmitter_->azimuth_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetElevation(float min, float max)
{
	particleEmitter_->elevation_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetVelocity(const Vector3& min, const Vector3& max)
{
	particleEmitter_->velocity_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetColor(const Vector4& min, const Vector4& max)
{
	particleEmitter_->color_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetLifeTime(float min, float max)
{
	particleEmitter_->lifeTime_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetCount(uint32_t count)
{
	particleEmitter_->particleCount_ = count;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetFrequency(float frequency)
{
	particleEmitter_->frequency_ = frequency;
	particleEmitter_->frequencyTime_ = frequency;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetDeleteTime(float deleteTime)
{
	particleEmitter_->deleteTime_ = deleteTime;
	return *this;
}

ParticleEmitter* EmitterBuilder::Build()
{
	return particleEmitter_;
}