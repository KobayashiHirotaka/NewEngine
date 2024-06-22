#include "Player.h"

void Player::Initialize()
{
	IGame3dObject::SetTag("Player");

	//modelManagerのinstance
	modelManager_ = ModelManager::GetInstance();

	//inputのinstance
	input_ = Input::GetInstance();

	//worldTransformの初期化
	worldTransform_.Initialize();

	//particleModelの初期化
	particleModel_.reset(ParticleModel::CreateFromOBJ("resource/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();
}

void Player::Update()
{
	if (input_->PressKey(DIK_0))
	{
		animationIndex = 0;
		float animationTime;
		animationTime = model_->GetAnimationTime();

		animationTime += 1.0f / 60.0f;
		animationTime = std::fmod(animationTime, model_->GetAnimation()[0].duration);

		model_->SetAnimationTime(animationTime);

		model_->ApplyAnimation(0);

		model_->Update();
	}

	if (input_->PressKey(DIK_1))
	{
		animationIndex = 1;
		float animationTime;
		animationTime = model_->GetAnimationTime();

		animationTime += 1.0f / 60.0f;
		animationTime = std::fmod(animationTime, model_->GetAnimation()[1].duration);

		model_->SetAnimationTime(animationTime);

		model_->ApplyAnimation(1);

		model_->Update();
	}

	if (input_->PressKey(DIK_2))
	{
		animationIndex = 2;
		float animationTime;
		animationTime = model_->GetAnimationTime();

		animationTime += 1.0f / 60.0f;
		animationTime = std::fmod(animationTime, model_->GetAnimation()[2].duration);

		model_->SetAnimationTime(animationTime);

		model_->ApplyAnimation(2);

		model_->Update();
	}

	model_->ApplyAnimation(animationIndex);

	model_->Update();

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

	model_->GetLight()->ImGui("DirectionalLight");
	model_->GetPointLight()->ImGui("PointLight");
	model_->GetSpotLight()->ImGui("SpotLight");

	ImGui::Begin("Player");
	ImGui::SliderFloat3("WTFT", &worldTransform_.translation.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("WTFR", &worldTransform_.rotation.x, 0.0f, 16.0f);
	ImGui::End();

	//worldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Player::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera, animationIndex);
}

void Player::BoneDraw(const Camera& camera)
{
	model_->BoneDraw(worldTransform_, camera, animationIndex);
}

void Player::DrawParticle(const Camera& camera)
{
	particleModel_->Draw(particleSystem_.get(), camera);
}
