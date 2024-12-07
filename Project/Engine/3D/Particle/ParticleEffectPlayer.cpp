#include "ParticleEffectPlayer.h"

void ParticleEffectPlayer::Initialize()
{
	//パーティクルの初期化
	particleModel_.reset(ParticleModel::CreateFromOBJ("resource/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();
}

void ParticleEffectPlayer::Update()
{
	//パーティクルの更新
	particleSystem_->Update();
}

void ParticleEffectPlayer::PlayParticle(const char* name, Vector3 position)
{
	if (std::string("PlayerRightNackle") == name)
	{
		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale({ 0.1f, 0.1f, 0.1f }, { 0.4f ,0.4f ,0.4f })
			.SetAzimuth(172.0f, 180.0f)
			.SetElevation(0.0f, 0.0f)
			.SetVelocity({ 0.08f ,0.08f ,0.08f }, { 0.1f ,0.1f ,0.1f })
			.SetColor({ 1.0f ,0.0f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
			.SetLifeTime(0.1f, 0.6f)
			.SetCount(100)
			.SetFrequency(4.0f)
			.SetDeleteTime(2.0f)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("PlayerLeftNackle") == name)
	{
		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale({ 0.1f, 0.1f, 0.1f }, { 0.4f ,0.4f ,0.4f })
			.SetAzimuth(0.0f, 8.0f)
			.SetElevation(0.0f, 0.0f)
			.SetVelocity({ 0.08f ,0.08f ,0.08f }, { 0.1f ,0.1f ,0.1f })
			.SetColor({ 1.0f ,0.0f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
			.SetLifeTime(0.1f, 0.6f)
			.SetCount(100)
			.SetFrequency(4.0f)
			.SetDeleteTime(2.0f)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("EnemyRightNackle") == name)
	{
		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale({ 0.1f, 0.1f, 0.1f }, { 0.4f ,0.4f ,0.4f })
			.SetAzimuth(172.0f, 180.0f)
			.SetElevation(0.0f, 0.0f)
			.SetVelocity({ 0.08f ,0.08f ,0.08f }, { 0.1f ,0.1f ,0.1f })
			.SetColor({ 0.0f ,0.0f ,1.0f ,1.0f }, { 0.0f ,0.5f ,1.0f ,1.0f })
			.SetLifeTime(0.1f, 0.6f)
			.SetCount(100)
			.SetFrequency(4.0f)
			.SetDeleteTime(2.0f)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("EnemyLeftNackle") == name)
	{
		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale({ 0.1f, 0.1f, 0.1f }, { 0.4f ,0.4f ,0.4f })
			.SetAzimuth(0.0f, 8.0f)
			.SetElevation(0.0f, 0.0f)
			.SetVelocity({ 0.08f ,0.08f ,0.08f }, { 0.1f ,0.1f ,0.1f })
			.SetColor({ 0.0f ,0.0f ,1.0f ,1.0f }, { 0.0f ,0.5f ,1.0f ,1.0f })
			.SetLifeTime(0.1f, 0.6f)
			.SetCount(100)
			.SetFrequency(4.0f)
			.SetDeleteTime(2.0f)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("RightBullet") == name)
	{
		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x + 0.1f, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale({ 0.4f, 0.4f, 0.4f }, { 0.5f ,0.5f ,0.5f })
			.SetAzimuth(0.0f, 0.8f)
			.SetElevation(-9.0f, 9.0f)
			.SetVelocity({ 0.0f ,0.0f ,0.0f }, { 0.01f ,0.01f ,0.01f })
			.SetColor({ 0.0f ,0.0f ,1.0f ,1.0f }, { 0.0f ,0.5f ,1.0f ,1.0f })
			.SetLifeTime(0.1f, 0.6f)
			.SetCount(10)
			.SetFrequency(4.0f)
			.SetDeleteTime(0.03f)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("LeftBullet") == name)
	{
		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x - 0.1f, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale({ 0.4f, 0.4f, 0.4f }, { 0.5f ,0.5f ,0.5f })
			.SetAzimuth(0.0f, 0.8f)
			.SetElevation(-9.0f, 9.0f)
			.SetVelocity({ 0.0f ,0.0f ,0.0f }, { 0.01f ,0.01f ,0.01f })
			.SetColor({ 0.0f ,0.0f ,1.0f ,1.0f }, { 0.0f ,0.5f ,1.0f ,1.0f })
			.SetLifeTime(0.1f, 0.6f)
			.SetCount(10)
			.SetFrequency(4.0f)
			.SetDeleteTime(0.03f)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("Hit") == name)
	{
		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
			.SetAzimuth(0.0f, 360.0f)
			.SetElevation(0.0f, 0.0f)
			.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
			.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
			.SetLifeTime(0.1f, 1.0f)
			.SetCount(50)
			.SetFrequency(4.0f)
			.SetDeleteTime(1.0f)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
	else if (std::string("Guard") == name)
	{
		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation({ position.x, position.y,  position.z })
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale({ 0.1f, 0.1f,0.1f }, { 0.2f ,0.2f ,0.2f })
			.SetAzimuth(0.0f, 360.0f)
			.SetElevation(0.0f, 0.0f)
			.SetVelocity({ 0.03f ,0.03f ,0.03f }, { 0.06f ,0.06f ,0.06f })
			.SetColor({ 1.0f ,1.0f ,1.0f ,1.0f }, { 1.0f ,1.0f ,1.0f ,1.0f })
			.SetLifeTime(0.1f, 1.0f)
			.SetCount(100)
			.SetFrequency(4.0f)
			.SetDeleteTime(2.0f)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
}

void ParticleEffectPlayer::Draw(const Camera& camera)
{
	particleModel_->Draw(particleSystem_.get(), camera);
}