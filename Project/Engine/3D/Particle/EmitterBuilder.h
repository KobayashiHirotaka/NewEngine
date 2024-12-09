/**
 * @file EmitterBuilder.h
 * @brief パーティクルエミッターの生成を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "ParticleEmitter.h"

class EmitterBuilder
{
public:
	EmitterBuilder();

	~EmitterBuilder();

	EmitterBuilder& SetEmitterName(const std::string& emitterName);

	EmitterBuilder& SetParticleType(ParticleEmitter::ParticleType particleType);

	EmitterBuilder& SetTranslation(const Vector3& translation);

	EmitterBuilder& SetArea(const Vector3& min, const Vector3& max);

	EmitterBuilder& SetRotation(const Vector3& min, const Vector3& max);

	EmitterBuilder& SetScale(const Vector3& min, const Vector3& max);

	EmitterBuilder& SetAzimuth(float min, float max);

	EmitterBuilder& SetElevation(float min, float max);

	EmitterBuilder& SetVelocity(const Vector3& min, const Vector3& max);

	EmitterBuilder& SetColor(const Vector4& min, const Vector4& max);

	EmitterBuilder& SetLifeTime(float min, float max);

	EmitterBuilder& SetCount(uint32_t count);

	EmitterBuilder& SetFrequency(float frequency);

	EmitterBuilder& SetDeleteTime(float deleteTime);

	ParticleEmitter* Build();

private:
	ParticleEmitter* particleEmitter_ = nullptr;
};

