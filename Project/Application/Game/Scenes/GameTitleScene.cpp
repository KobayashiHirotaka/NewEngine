#include "GameTitleScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameTitleScene::GameTitleScene() {};

GameTitleScene::~GameTitleScene() {};

void GameTitleScene::Initialize()
{
	textureManager_ = TextureManager::GetInstance();

	modelManager_ = ModelManager::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	textureHandle_[0] = TextureManager::LoadTexture("resource/images/startTimer1.png");
	sprite_[0].reset(Sprite::Create(textureHandle_[0], {-500.0f,-80.0f}));

	textureHandle_[1] = TextureManager::LoadTexture("resource/images/startTimer2.png");
	sprite_[1].reset(Sprite::Create(textureHandle_[1], { 500.0f,-80.0f }));

	modelManager_->LoadModel("resource/hammer", "hammer.obj");
	modelManager_->LoadModel("resource/skydome", "skydome.obj");

	worldTransform_.Initialize();
	worldTransform_.translation = { 0.0f,-1.0f,0.0f };

	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->SetModel(modelManager_->FindModel("hammer.obj"));

	debugCamera_.Initialize();

	camera_.UpdateMatrix();

	titleSoundHandle_ = audio_->SoundLoadWave("resource/Sounds/Title.wav");
	audio_->StopAudio(titleSoundHandle_);
	audio_->SoundPlayWave(titleSoundHandle_, true, 1.0f);
};

void GameTitleScene::Update()
{
	player_->Update();

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

	//モデル切り替え
	if (input_->PushKey(DIK_RETURN))
	{
		player_->SetModel(modelManager_->FindModel("skydome.obj"));
	}

	////カメラ移動
	//if (input_->PressKey(DIK_A))
	//{
	//	camera_.translation_.x -= 0.1f;
	//}

	//if (input_->PressKey(DIK_D))
	//{
	//	camera_.translation_.x += 0.1f;
	//}

	//if (input_->PressKey(DIK_W))
	//{
	//	camera_.translation_.y += 0.1f;
	//}

	//if (input_->PressKey(DIK_S))
	//{
	//	camera_.translation_.y -= 0.1f;
	//}

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

	debugCamera_.Update();

	if (input_->PushKey(DIK_K))
	{
		isDebugCamera_ = true;
	}
	else if (input_->PushKey(DIK_L))
	{
		isDebugCamera_ = false;
	}

	if (isDebugCamera_)
	{
		camera_.matView_ = debugCamera_.GetCamera().matView_;
		camera_.matProjection_ = debugCamera_.GetCamera().matProjection_;
		camera_.TransferMatrix();
	}
	else
	{
		camera_.UpdateMatrix();
	}

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

	player_->Draw(camera_);

	Model::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();

	ParticleModel::PreDraw();

	player_->DrawParticle(camera_);

	ParticleModel::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	for (int i = 0; i < 2; i++)
	{
		sprite_[i]->Draw();
	}

	Sprite::PostDraw();
};

void GameTitleScene::Finalize()
{

}