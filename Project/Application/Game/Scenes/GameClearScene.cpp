#include "GameClearScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameClearScene::GameClearScene() {};

GameClearScene::~GameClearScene() {};

void GameClearScene::Initialize()
{
	//textureManagerのinstance
	textureManager_ = TextureManager::GetInstance();

	//modelManagerのinstance
	modelManager_ = ModelManager::GetInstance();

	//inputのinstance
	input_ = Input::GetInstance();

	//audioのinstance
	audio_ = Audio::GetInstance();

	//postProcessのinstance
	PostProcess::GetInstance()->SetIsPostProcessActive(true);

	//postEffectの切り替え
	PostProcess::GetInstance()->SetIsBloomActive(true);
	PostProcess::GetInstance()->SetIsVignetteActive(true);
	PostProcess::GetInstance()->SetIsGrayScaleActive(true);
	PostProcess::GetInstance()->SetIsGaussianFilterActive(true);

	//modelの読み込み
	modelManager_->LoadModel("resource/skydome", "skydome.obj");

	//skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	//debugCameraの初期化
	debugCamera_.Initialize();
};

void GameClearScene::Update()
{
	//skydomeの更新
	skydome_->Update();

	//シーン切り替え
	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GameTitleScene");
			return;
		}
	}

	if (input_->PushKey(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GameTitleScene");
		return;
	}

	//camera、debugCameraの処理
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

	//imGui
	ImGui::Begin("ClearScene");
	ImGui::Text("Abutton or SpaceKey : TitleScene");
	ImGui::End();
};

void GameClearScene::Draw()
{
	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	//skydomeの描画
	skydome_->Draw(modelManager_->FindModel("skydome.obj"), camera_);

	Model::PostDraw();

	ParticleModel::PreDraw();

	ParticleModel::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();
};

void GameClearScene::Finalize()
{

}