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

	light_ = std::make_unique<Light>();
	light_->Initialize();

	pointLight_ = std::make_unique<PointLight>();
	pointLight_->Initialize();

	PostProcess::GetInstance()->SetIsPostProcessActive(true);
	PostProcess::GetInstance()->SetIsBloomActive(true);
	/*PostProcess::GetInstance()->SetIsVignetteActive(true);*/

	model_.reset(Model::CreateFromOBJ("resource/models", "monsterBall.obj"));

	groundModel_.reset(Model::CreateFromOBJ("resource/models", "terrain.obj"));

	textureHandle_[0] = TextureManager::LoadTexture("resource/images/startTimer1.png");
	sprite_[0].reset(Sprite::Create(textureHandle_[0], {-500.0f,-80.0f}));

	textureHandle_[1] = TextureManager::LoadTexture("resource/images/startTimer2.png");
	sprite_[1].reset(Sprite::Create(textureHandle_[1], { 500.0f,-80.0f }));

	worldTransform_.Initialize();
	groundWorldTransform_.Initialize();

	camera_.translation_.y = 3.0f;

	debugCamera_.Initialize();

	camera_.UpdateMatrix();
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

	model_->GetLight()->ImGui("DirectionalLight");
	
	model_->GetPointLight()->ImGui("PointLight");

	model_->GetSpotLight()->ImGui("SpotLight");

	groundModel_->GetLight()->ImGui("DirectionalLight");

	groundModel_->GetPointLight()->ImGui("PointLight");

	groundModel_->GetSpotLight()->ImGui("SpotLight");

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
	groundWorldTransform_.UpdateMatrix();

	/*camera_.UpdateMatrix();*/

	ImGui::Begin("Model");
	ImGui::DragFloat3("scale", &worldTransform_.scale.x, 0.1f, 1.0f, 10.0f);
	ImGui::End();
};

void GameTitleScene::Draw()
{
	Model::PreDraw();

	Model::PostDraw();

	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	groundModel_->Draw(groundWorldTransform_, camera_);

	Model::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();

	ParticleModel::PreDraw();

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