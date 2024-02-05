#include "Player.h"

void Player::Initialize()
{
	input_ = Input::GetInstance();

	IGame3dObject::Initialize();
	worldTransform_.translation.y = 3.0f;

	particleModel_.reset(ParticleModel::CreateFromOBJ("resource/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();
}

void Player::Update()
{
	IGame3dObject::Update();

	//パーティクルの更新
	particleSystem_->Update();

	if (input_->PushKey(DIK_P))
	{
		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetParticleType(ParticleEmitter::ParticleType::kNormal)
			.SetTranslation(worldTransform_.translation)
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale({ 0.2f, 0.2f,0.2f }, { 0.6f ,0.6f ,0.6f })
			.SetAzimuth(0.0f, 360.0f)
			.SetElevation(0.0f, 0.0f)
			.SetVelocity({ 0.06f ,0.06f ,0.06f }, { 0.1f ,0.1f ,0.1f })
			.SetColor({ 1.0f ,1.0f ,1.0f ,1.0f }, { 1.0f ,1.0f ,1.0f ,1.0f })
			.SetLifeTime(0.1f, 1.0f)
			.SetCount(100)
			.SetFrequency(4.0f)
			.SetDeleteTime(2.0f)
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
}

void Player::Draw(const Camera& camera)
{
	IGame3dObject::Draw(camera);
}

void Player::DrawParticle(const Camera& camera)
{
	particleModel_->Draw(particleSystem_.get(), camera);
}
