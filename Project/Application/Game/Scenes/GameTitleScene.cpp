#include "GameTitleScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameTitleScene::GameTitleScene() {};

GameTitleScene::~GameTitleScene() {};

void GameTitleScene::Initialize()
{
	textureManager_ = TextureManager::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	textureHandle_[0] = TextureManager::LoadTexture("resource/images/startTimer1.png");
	sprite_[0].reset(Sprite::Create(textureHandle_[0], {-500.0f,-80.0f}));

	textureHandle_[1] = TextureManager::LoadTexture("resource/images/startTimer2.png");
	sprite_[1].reset(Sprite::Create(textureHandle_[1], { 500.0f,-80.0f }));

	worldTransform_.Initialize();

	particleModel_.reset(ParticleModel::CreateFromOBJ("resource/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();

	camera_.UpdateMatrix();

	titleSoundHandle_ = audio_->SoundLoadWave("resource/Sounds/Title.wav");
	audio_->StopAudio(titleSoundHandle_);
	audio_->SoundPlayWave(titleSoundHandle_, true, 1.0f);
};

void GameTitleScene::Update()
{
	//シーン切り替え
	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GamePlayScene");
			return;
		}
	}

	if (input_->PushKey(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GamePlayScene");
		return;
	}

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

	//ポストプロセス
	if (input_->PressKey(DIK_1))
	{
		PostProcess::GetInstance()->SetIsPostProcessActive(true);
	}

	//Bloom
	if (input_->PressKey(DIK_2))
	{
		PostProcess::GetInstance()->SetIsBloomActive(true);
	}

	//Vignette
	if (input_->PressKey(DIK_3))
	{
		PostProcess::GetInstance()->SetIsVignetteActive(true);
	}

	if (input_->PressKey(DIK_4))
	{
		PostProcess::GetInstance()->SetIsPostProcessActive(false);
		PostProcess::GetInstance()->SetIsBloomActive(false);
		PostProcess::GetInstance()->SetIsVignetteActive(false);
	}

	camera_.UpdateMatrix();

	worldTransform_.UpdateMatrix();

	/*camera_.UpdateMatrix();*/

	ImGui::Begin("TitleScene");
	ImGui::Text("Abutton or SpaceKey : PlayScene");
	ImGui::End();
};

void GameTitleScene::Draw()
{
	Model::PreDraw();

	Model::PostDraw();

	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	Model::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	for (int i = 0; i < 2; i++)
	{
		sprite_[i]->Draw();
	}

	Sprite::PostDraw();

	ParticleModel::PreDraw();

	particleModel_->Draw(particleSystem_.get(), camera_);

	ParticleModel::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();
};

void GameTitleScene::Finalize()
{

}