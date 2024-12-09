/**
 * @file Particle.h
 * @brief パーティクルの初期化、更新を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "BaseParticle.h"
#include "Engine/Utility/Math/MyMath.h"

class Particle : public BaseParticle
{
public:
	void Initialize(const Vector3& translation, const Vector3& rotation, const Vector3& scale, const Vector3& velocity, const Vector4& color, float lifeTime) override;

	void Update() override;
};