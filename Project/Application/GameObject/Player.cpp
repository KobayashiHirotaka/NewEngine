#include "Player.h"

void Player::Initialize()
{
	input_ = Input::GetInstance();

	light_ = std::make_unique<Light>();
	light_->Initialize();

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->Initialize();

	modelFighterBody_.reset(Model::CreateFromOBJ("resource/newEnemy", "newEnemy.gltf"));

	worldTransform_.Initialize();
	worldTransform_.translation.y = 3.0f;
	worldTransform_.scale = { 0.007f,0.007f,0.007f };

	particleModel_.reset(ParticleModel::CreateFromOBJ("resource/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();

	worldTransform_.UpdateMatrixEuler();

	animationIndex = 0;
}

void Player::Update()
{
	if (input_->PressKey(DIK_0))
	{
		animationIndex = 0;
		float animationTime;
		animationTime = modelFighterBody_->GetAnimationTime();

		animationTime += 1.0f / 60.0f;
		animationTime = std::fmod(animationTime, modelFighterBody_->GetAnimation()[0].duration);

		modelFighterBody_->SetAnimationTime(animationTime);

		modelFighterBody_->ApplyAnimation(0);

		modelFighterBody_->Update();
	}

	if (input_->PressKey(DIK_1))
	{
		animationIndex = 1;
		float animationTime;
		animationTime = modelFighterBody_->GetAnimationTime();

		animationTime += 1.0f / 60.0f;
		animationTime = std::fmod(animationTime, modelFighterBody_->GetAnimation()[1].duration);

		modelFighterBody_->SetAnimationTime(animationTime);

		modelFighterBody_->ApplyAnimation(1);

		modelFighterBody_->Update();
	}

	if (input_->PressKey(DIK_2))
	{
		animationIndex = 2;
		float animationTime;
		animationTime = modelFighterBody_->GetAnimationTime();

		animationTime += 1.0f / 60.0f;
		animationTime = std::fmod(animationTime, modelFighterBody_->GetAnimation()[2].duration);

		modelFighterBody_->SetAnimationTime(animationTime);

		modelFighterBody_->ApplyAnimation(2);

		modelFighterBody_->Update();
	}

	modelFighterBody_->ApplyAnimation(animationIndex);

	modelFighterBody_->Update();

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

	worldTransform_.UpdateMatrixEuler();

	modelFighterBody_->GetLight()->ImGui("DirectionalLight");

	modelFighterBody_->GetPointLight()->ImGui("PointLight");

	modelFighterBody_->GetSpotLight()->ImGui("SpotLight");
}

void Player::Draw(const Camera& camera)
{
	modelFighterBody_->Draw(worldTransform_, camera, animationIndex);
}

void Player::BoneDraw(const Camera& camera)
{
	modelFighterBody_->BoneDraw(worldTransform_, camera, animationIndex);
}

void Player::DrawParticle(const Camera& camera)
{
	particleModel_->Draw(particleSystem_.get(), camera);
}
