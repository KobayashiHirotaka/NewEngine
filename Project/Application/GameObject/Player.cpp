#include "Player.h"

void Player::Initialize()
{
	input_ = Input::GetInstance();

	light_ = std::make_unique<Light>();
	light_->Initialize();

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->Initialize();

	IGame3dObject::SetTag("Player");

	worldTransform_.Initialize();
	worldTransform_.translation = { 0.065f,-2.5f,0.0f };
	worldTransform_.rotation = { 7.85f,1.72f,0.0f };
	worldTransform_.scale = { 0.007f,0.007f,0.007f };

	particleModel_.reset(ParticleModel::CreateFromOBJ("resource/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();

	worldTransform_.UpdateMatrixEuler();

	animationIndex = 0;
}

void Player::Update(Model* model)
{
	if (input_->PressKey(DIK_0))
	{
		animationIndex = 0;
		float animationTime;
		animationTime = model->GetAnimationTime();

		animationTime += 1.0f / 60.0f;
		animationTime = std::fmod(animationTime, model->GetAnimation()[0].duration);

		model->SetAnimationTime(animationTime);

		model->ApplyAnimation(0);

		model->Update();
	}

	if (input_->PressKey(DIK_1))
	{
		animationIndex = 1;
		float animationTime;
		animationTime = model->GetAnimationTime();

		animationTime += 1.0f / 60.0f;
		animationTime = std::fmod(animationTime, model->GetAnimation()[1].duration);

		model->SetAnimationTime(animationTime);

		model->ApplyAnimation(1);

		model->Update();
	}

	if (input_->PressKey(DIK_2))
	{
		animationIndex = 2;
		float animationTime;
		animationTime = model->GetAnimationTime();

		animationTime += 1.0f / 60.0f;
		animationTime = std::fmod(animationTime, model->GetAnimation()[2].duration);

		model->SetAnimationTime(animationTime);

		model->ApplyAnimation(2);

		model->Update();
	}

	model->ApplyAnimation(animationIndex);

	model->Update();

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

	model->GetLight()->ImGui("DirectionalLight");
	model->GetPointLight()->ImGui("PointLight");
	model->GetSpotLight()->ImGui("SpotLight");

	ImGui::Begin("Player");
	ImGui::SliderFloat3("WTFT", &worldTransform_.translation.x, -10.0f, 16.0f);
	ImGui::SliderFloat3("WTFR", &worldTransform_.rotation.x, 0.0f, 16.0f);
	ImGui::End();
}

void Player::Draw(Model* model, const Camera& camera)
{
	model->Draw(worldTransform_, camera, animationIndex);
}

void Player::BoneDraw(Model* model, const Camera& camera)
{
	model->BoneDraw(worldTransform_, camera, animationIndex);
}

void Player::DrawParticle(const Camera& camera)
{
	particleModel_->Draw(particleSystem_.get(), camera);
}
