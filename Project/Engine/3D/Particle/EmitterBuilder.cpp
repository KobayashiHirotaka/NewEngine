/**
 * @file EmitterBuilder.cpp
 * @brief パーティクルエミッターの生成を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "EmitterBuilder.h"

EmitterBuilder::EmitterBuilder()
{
	//新しいParticleEmitterを作成
	particleEmitter_ = new ParticleEmitter();
}

EmitterBuilder::~EmitterBuilder()
{
	
}

EmitterBuilder& EmitterBuilder::SetEmitterName(const std::string& emitterName)
{
	//エミッター名を設定
	particleEmitter_->emitterName_ = emitterName;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetParticleType(ParticleEmitter::ParticleType particleType)
{
	//パーティクルのタイプを設定
	particleEmitter_->particleType_ = particleType;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetTranslation(const Vector3& translation)
{
	//パーティクルエミッターの位置を設定
	particleEmitter_->translation_ = translation;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetArea(const Vector3& min, const Vector3& max)
{
	//発生エリアを設定
	particleEmitter_->area_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetRotation(const Vector3& min, const Vector3& max)
{
	//回転範囲を設定
	particleEmitter_->rotation_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetScale(const Vector3& min, const Vector3& max)
{
	//スケール範囲を設定
	particleEmitter_->scale_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetAzimuth(float min, float max)
{
	//水平角度範囲を設定
	particleEmitter_->azimuth_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetElevation(float min, float max)
{
	//垂直角度範囲を設定
	particleEmitter_->elevation_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetVelocity(const Vector3& min, const Vector3& max)
{
	//パーティクルの速度範囲を設定
	particleEmitter_->velocity_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetColor(const Vector4& min, const Vector4& max)
{
	//色範囲を設定
	particleEmitter_->color_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetLifeTime(float min, float max)
{
	//ライフタイム範囲を設定
	particleEmitter_->lifeTime_ = { min,max };
	return *this;
}

EmitterBuilder& EmitterBuilder::SetCount(uint32_t count)
{
	//パーティクルの個数を設定
	particleEmitter_->particleCount_ = count;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetFrequency(float frequency)
{
	//パーティクルの発生頻度を設定
	particleEmitter_->frequency_ = frequency;
	particleEmitter_->frequencyTime_ = frequency;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetDeleteTime(float deleteTime)
{
	//パーティクルエミッターの削除時間を設定
	particleEmitter_->deleteTime_ = deleteTime;
	return *this;
}

ParticleEmitter* EmitterBuilder::Build()
{
	//設定したパラメータに基づいてParticleEmitterオブジェクトを返す
	return particleEmitter_;
}