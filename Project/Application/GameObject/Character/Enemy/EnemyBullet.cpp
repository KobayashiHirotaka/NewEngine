#include "EnemyBullet.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include <cassert>

void EnemyBullet::Initialize(Model* model, const Vector3& positon, const Vector3& velocity) 
{

	assert(model);

	model_ = model;

	worldTransform_.Initialize();

	worldTransform_.translation = positon;

	velocity_ = velocity;

	//パーティクルの初期化
	particleModel_.reset(ParticleModel::CreateFromOBJ("resource/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();
}

void EnemyBullet::Update() 
{
	worldTransform_.translation = Add(worldTransform_.translation, velocity_);

	if (--deathTimer_ <= 0)
	{
		isDead_ = true;
	}

	//パーティクルの更新
	particleSystem_->Update();
	
	ParticleEmitter* newParticleEmitter = EmitterBuilder()
		.SetParticleType(ParticleEmitter::ParticleType::kNormal)
		.SetTranslation({ worldTransform_.translation })
		.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
		.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
		.SetScale({ 0.1f, 0.1f, 0.1f }, { 0.4f ,0.4f ,0.4f })
		.SetAzimuth(0.0f, 0.8f)
		.SetElevation(0.0f, 8.0f)
		.SetVelocity({ 0.08f ,0.08f ,0.08f }, { 0.1f ,0.1f ,0.1f })
		.SetColor({ 0.0f ,0.0f ,1.0f ,1.0f }, { 0.0f ,0.5f ,1.0f ,1.0f })
		.SetLifeTime(0.1f, 0.6f)
		.SetCount(10)
		.SetFrequency(4.0f)
		.SetDeleteTime(2.0f)
		.Build();
	particleSystem_->AddParticleEmitter(newParticleEmitter);

	worldTransform_.UpdateMatrixEuler();

	ImGui::Begin("EBullet");
	ImGui::Text("isDead = %d", isDead_);
	ImGui::SliderFloat3("WTFT", &worldTransform_.translation.x, -100.0f, 100.0f);
	ImGui::End();
}

void EnemyBullet::Draw(const Camera& camera)
{
	if (isDead_ == false)
	{
		model_->Draw(worldTransform_, camera, 0);
	}
}

void EnemyBullet::ParticleDraw(const Camera& camera)
{
	particleModel_->Draw(particleSystem_.get(), camera);
}


void EnemyBullet::OnCollision()
{
	isDead_ = true;
}

Vector3 EnemyBullet::GetWorldPosition()
{
	Vector3 worldPos;

	worldPos.x = worldTransform_.translation.x;
	worldPos.y = worldTransform_.translation.y;
	worldPos.z = worldTransform_.translation.z;

	return worldPos;
}
