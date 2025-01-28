/**
 * @file ParticleEmitter.cpp
 * @brief パーティクルエフェクトの生成、再生などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "ParticleEffectPlayer.h"

void ParticleEffectPlayer::Initialize()
{
	//モデルの生成
	baseParticleModel_.reset(ParticleModel::CreateFromOBJ("Resource/Particle/BaseParticle", "BaseParticle.obj"));
	//shockWaveParticleModel_.reset(ParticleModel::CreateFromOBJ("Resource/Particle/ShockWaveParticle", "ShockWaveParticle.obj"));

	//パーティクルシステムの初期化
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();
}

void ParticleEffectPlayer::Update()
{
	//パーティクルの更新
	particleSystem_->Update();
}

void ParticleEffectPlayer::PlayParticle(const char* name, const Vector3& position)
{
	//名前に応じたパーティクル
	if (std::string("PlayerRightNackle") == name)
	{
		//各パラメータの設定
		const Vector3 scaleMin = { 0.1f,0.1f,0.1f };
		const Vector3 scaleMax = { 0.4f,0.4f,0.4f };

		const float azimuthMin = 172.0f;
		const float azimuthMax = 180.0f;

		const Vector3 velocityMin = { 0.08f ,0.08f ,0.08f };
		const Vector3 velocityMax = { 0.1f ,0.1f ,0.1f };

		const Vector4 colorMin = { 1.0f ,0.0f ,0.0f ,1.0f };
		const Vector4 colorMax = { 1.0f ,0.5f ,0.0f ,1.0f };

		const float lifeTimeMin = 0.1f;
		const float lifeTimeMax = 0.6f;

		const int count = 100;

		const float frequency = 4.0f;

		const float deleteTime = 2.0f;

		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale(scaleMin, scaleMax)
			.SetAzimuth(azimuthMin, azimuthMax)
			.SetElevation(0.0f, 0.0f)
			.SetVelocity(velocityMin, velocityMax)
			.SetColor(colorMin, colorMax)
			.SetLifeTime(lifeTimeMin, lifeTimeMax)
			.SetCount(count)
			.SetFrequency(frequency)
			.SetDeleteTime(deleteTime)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("PlayerLeftNackle") == name)
	{
		//各パラメータの設定
		const Vector3 scaleMin = { 0.1f,0.1f,0.1f };
		const Vector3 scaleMax = { 0.4f,0.4f,0.4f };

		const float azimuthMin = 0.0f;
		const float azimuthMax = 8.0f;

		const Vector3 velocityMin = { 0.08f ,0.08f ,0.08f };
		const Vector3 velocityMax = { 0.1f ,0.1f ,0.1f };

		const Vector4 colorMin = { 1.0f ,0.0f ,0.0f ,1.0f };
		const Vector4 colorMax = { 1.0f ,0.5f ,0.0f ,1.0f };

		const float lifeTimeMin = 0.1f;
		const float lifeTimeMax = 0.6f;

		const int count = 100;

		const float frequency = 4.0f;

		const float deleteTime = 2.0f;

		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale(scaleMin, scaleMax)
			.SetAzimuth(azimuthMin, azimuthMax)
			.SetElevation(0.0f, 0.0f)
			.SetVelocity(velocityMin, velocityMax)
			.SetColor(colorMin, colorMax)
			.SetLifeTime(lifeTimeMin, lifeTimeMax)
			.SetCount(count)
			.SetFrequency(frequency)
			.SetDeleteTime(deleteTime)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("EnemyRightNackle") == name)
	{
		//各パラメータの設定
		const Vector3 scaleMin = { 0.1f,0.1f,0.1f };
		const Vector3 scaleMax = { 0.4f,0.4f,0.4f };

		const float azimuthMin = 172.0f;
		const float azimuthMax = 180.0f;

		const Vector3 velocityMin = { 0.08f ,0.08f ,0.08f };
		const Vector3 velocityMax = { 0.1f ,0.1f ,0.1f };

		const Vector4 colorMin = { 0.0f ,0.0f ,1.0f ,1.0f };
		const Vector4 colorMax = { 0.0f ,0.5f ,1.0f ,1.0f };

		const float lifeTimeMin = 0.1f;
		const float lifeTimeMax = 0.6f;

		const int count = 100;

		const float frequency = 4.0f;

		const float deleteTime = 2.0f;

		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale(scaleMin, scaleMax)
			.SetAzimuth(azimuthMin, azimuthMax)
			.SetElevation(0.0f, 0.0f)
			.SetVelocity(velocityMin, velocityMax)
			.SetColor(colorMin, colorMax)
			.SetLifeTime(lifeTimeMin, lifeTimeMax)
			.SetCount(count)
			.SetFrequency(frequency)
			.SetDeleteTime(deleteTime)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("EnemyLeftNackle") == name)
	{
		//各パラメータの設定
		const Vector3 scaleMin = { 0.1f,0.1f,0.1f };
		const Vector3 scaleMax = { 0.4f,0.4f,0.4f };

		const float azimuthMin = 0.0f;
		const float azimuthMax = 8.0f;

		const Vector3 velocityMin = { 0.08f ,0.08f ,0.08f };
		const Vector3 velocityMax = { 0.1f ,0.1f ,0.1f };

		const Vector4 colorMin = { 0.0f ,0.0f ,1.0f ,1.0f };
		const Vector4 colorMax = { 0.0f ,0.5f ,1.0f ,1.0f };

		const float lifeTimeMin = 0.1f;
		const float lifeTimeMax = 0.6f;

		const int count = 100;

		const float frequency = 4.0f;

		const float deleteTime = 2.0f;

		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale(scaleMin, scaleMax)
			.SetAzimuth(azimuthMin, azimuthMax)
			.SetElevation(0.0f, 0.0f)
			.SetVelocity(velocityMin, velocityMax)
			.SetColor(colorMin, colorMax)
			.SetLifeTime(lifeTimeMin, lifeTimeMax)
			.SetCount(count)
			.SetFrequency(frequency)
			.SetDeleteTime(deleteTime)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("PlayerRightBullet") == name)
	{
		//各パラメータの設定
		const float adjustPositionX = 0.1f;

		const Vector3 scaleMin = { 0.4f,0.4f,0.4f };
		const Vector3 scaleMax = { 0.5f,0.5f,0.5f };

		const float azimuthMin = 0.0f;
		const float azimuthMax = 0.8f;

		const float elevationMin = -9.0f;
		const float elevationMax = 9.0f;

		const Vector3 velocityMin = { 0.0f ,0.0f ,0.0f };
		const Vector3 velocityMax = { 0.01f ,0.01f ,0.01f };

		const Vector4 colorMin = { 1.0f ,0.0f ,0.0f ,1.0f };
		const Vector4 colorMax = { 1.0f ,0.5f ,0.0f ,1.0f };

		const float lifeTimeMin = 0.1f;
		const float lifeTimeMax = 0.6f;

		const int count = 10;

		const float frequency = 4.0f;

		const float deleteTime = 0.03f;

		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x + adjustPositionX, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale(scaleMin, scaleMax)
			.SetAzimuth(azimuthMin, azimuthMax)
			.SetElevation(elevationMin, elevationMax)
			.SetVelocity(velocityMin, velocityMax)
			.SetColor(colorMin, colorMax)
			.SetLifeTime(lifeTimeMin, lifeTimeMax)
			.SetCount(count)
			.SetFrequency(frequency)
			.SetDeleteTime(deleteTime)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("PlayerLeftBullet") == name)
	{
		//各パラメータの設定
		const float adjustPositionX = 0.1f;

		const Vector3 scaleMin = { 0.4f,0.4f,0.4f };
		const Vector3 scaleMax = { 0.5f,0.5f,0.5f };

		const float azimuthMin = 0.0f;
		const float azimuthMax = 0.8f;

		const float elevationMin = -9.0f;
		const float elevationMax = 9.0f;

		const Vector3 velocityMin = { 0.0f ,0.0f ,0.0f };
		const Vector3 velocityMax = { 0.01f ,0.01f ,0.01f };

		const Vector4 colorMin = { 1.0f ,0.0f ,0.0f ,1.0f };
		const Vector4 colorMax = { 1.0f ,0.5f ,0.0f ,1.0f };

		const float lifeTimeMin = 0.1f;
		const float lifeTimeMax = 0.6f;

		const int count = 10;

		const float frequency = 4.0f;

		const float deleteTime = 0.03f;

		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x - adjustPositionX, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale(scaleMin, scaleMax)
			.SetAzimuth(azimuthMin, azimuthMax)
			.SetElevation(elevationMin, elevationMax)
			.SetVelocity(velocityMin, velocityMax)
			.SetColor(colorMin, colorMax)
			.SetLifeTime(lifeTimeMin, lifeTimeMax)
			.SetCount(count)
			.SetFrequency(frequency)
			.SetDeleteTime(deleteTime)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("EnemyRightBullet") == name)
	{
		//各パラメータの設定
		const float adjustPositionX = 0.1f;

		const Vector3 scaleMin = { 0.4f,0.4f,0.4f };
		const Vector3 scaleMax = { 0.5f,0.5f,0.5f };

		const float azimuthMin = 0.0f;
		const float azimuthMax = 0.8f;

		const float elevationMin = -9.0f;
		const float elevationMax = 9.0f;

		const Vector3 velocityMin = { 0.0f ,0.0f ,0.0f };
		const Vector3 velocityMax = { 0.01f ,0.01f ,0.01f };

		const Vector4 colorMin = { 0.0f ,0.0f ,1.0f ,1.0f };
		const Vector4 colorMax = { 0.0f ,0.5f ,1.0f ,1.0f };

		const float lifeTimeMin = 0.1f;
		const float lifeTimeMax = 0.6f;

		const int count = 10;

		const float frequency = 4.0f;

		const float deleteTime = 0.03f;

		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x + adjustPositionX, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale(scaleMin, scaleMax)
			.SetAzimuth(azimuthMin, azimuthMax)
			.SetElevation(elevationMin, elevationMax)
			.SetVelocity(velocityMin, velocityMax)
			.SetColor(colorMin, colorMax)
			.SetLifeTime(lifeTimeMin, lifeTimeMax)
			.SetCount(count)
			.SetFrequency(frequency)
			.SetDeleteTime(deleteTime)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
		}
	else if (std::string("EnemyLeftBullet") == name)
	{
		//各パラメータの設定
		const float adjustPositionX = 0.1f;

		const Vector3 scaleMin = { 0.4f,0.4f,0.4f };
		const Vector3 scaleMax = { 0.5f,0.5f,0.5f };

		const float azimuthMin = 0.0f;
		const float azimuthMax = 0.8f;

		const float elevationMin = -9.0f;
		const float elevationMax = 9.0f;

		const Vector3 velocityMin = { 0.0f ,0.0f ,0.0f };
		const Vector3 velocityMax = { 0.01f ,0.01f ,0.01f };

		const Vector4 colorMin = { 0.0f ,0.0f ,1.0f ,1.0f };
		const Vector4 colorMax = { 0.0f ,0.5f ,1.0f ,1.0f };

		const float lifeTimeMin = 0.1f;
		const float lifeTimeMax = 0.6f;

		const int count = 10;

		const float frequency = 4.0f;

		const float deleteTime = 0.03f;

		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x - adjustPositionX, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale(scaleMin, scaleMax)
			.SetAzimuth(azimuthMin, azimuthMax)
			.SetElevation(elevationMin, elevationMax)
			.SetVelocity(velocityMin, velocityMax)
			.SetColor(colorMin, colorMax)
			.SetLifeTime(lifeTimeMin, lifeTimeMax)
			.SetCount(count)
			.SetFrequency(frequency)
			.SetDeleteTime(deleteTime)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
		}
	else if (std::string("Hit") == name)
	{
		//各パラメータの設定
		const Vector3 scaleMin = { 0.1f,0.1f,0.1f };
		const Vector3 scaleMax = { 0.2f,0.2f,0.2f };

		const float azimuthMin = 0.0f;
		const float azimuthMax = 360.0f;

		const Vector3 velocityMin = { 0.03f ,0.03f ,0.03f };
		const Vector3 velocityMax = { 0.06f ,0.06f ,0.06f };

		const Vector4 colorMin = { 1.0f ,0.5f ,0.0f ,1.0f };
		const Vector4 colorMax = { 1.0f ,0.5f ,0.0f ,1.0f };

		const float lifeTimeMin = 0.1f;
		const float lifeTimeMax = 1.0f;

		const int count = 50;

		const float frequency = 4.0f;

		const float deleteTime = 1.0f;

		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale(scaleMin, scaleMax)
			.SetAzimuth(azimuthMin, azimuthMax)
			.SetElevation(0.0f, 0.0f)
			.SetVelocity(velocityMin, velocityMax)
			.SetColor(colorMin, colorMax)
			.SetLifeTime(lifeTimeMin, lifeTimeMax)
			.SetCount(count)
			.SetFrequency(frequency)
			.SetDeleteTime(deleteTime)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("Guard") == name)
	{
		//各パラメータの設定
		const Vector3 scaleMin = { 0.1f,0.1f,0.1f };
		const Vector3 scaleMax = { 0.2f,0.2f,0.2f };

		const float azimuthMin = 0.0f;
		const float azimuthMax = 360.0f;

		const Vector3 velocityMin = { 0.03f ,0.03f ,0.03f };
		const Vector3 velocityMax = { 0.06f ,0.06f ,0.06f };

		const Vector4 colorMin = { 1.0f ,1.0f ,1.0f ,1.0f };
		const Vector4 colorMax = { 1.0f ,1.0f ,1.0f ,1.0f };

		const float lifeTimeMin = 0.1f;
		const float lifeTimeMax = 1.0f;

		const int count = 80;

		const float frequency = 4.0f;

		const float deleteTime = 1.0f;

		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale(scaleMin, scaleMax)
			.SetAzimuth(azimuthMin, azimuthMax)
			.SetElevation(0.0f, 0.0f)
			.SetVelocity(velocityMin, velocityMax)
			.SetColor(colorMin, colorMax)
			.SetLifeTime(lifeTimeMin, lifeTimeMax)
			.SetCount(count)
			.SetFrequency(frequency)
			.SetDeleteTime(deleteTime)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("ShockWave") == name)
	{
		
	}
}

void ParticleEffectPlayer::Draw(const Camera& camera)
{
	//パーティクルの描画
	baseParticleModel_->Draw(particleSystem_.get(), camera);
	//shockWaveParticleModel_->Draw(particleSystem_.get(), camera);
}