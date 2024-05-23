#include "GamePlayScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GamePlayScene::GamePlayScene() {};

GamePlayScene::~GamePlayScene() {};

void GamePlayScene::Initialize()
{
	textureManager_ = TextureManager::GetInstance();

	modelManager_ = ModelManager::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	modelManager_->LoadModel("resource/hammer", "hammer.obj");
	modelManager_->LoadModel("resource/models", "monsterBall.obj");

	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->SetModel(modelManager_->FindModel("hammer.obj"));

	debugCamera_.Initialize();

	camera_.UpdateMatrix();
};

void GamePlayScene::Update()
{
	player_->Update();

	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GameClearScene");
			return;
		}
	}

	if (input_->PushKey(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GameClearScene");
		return;
	}

	//モデル切り替え
	if (input_->PushKey(DIK_RETURN))
	{
		player_->SetModel(modelManager_->FindModel("monsterBall.obj"));
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

	modelManager_->FindModel("monsterBall.obj")->GetLight()->ImGui("DirectionalLight");

	modelManager_->FindModel("monsterBall.obj")->GetPointLight()->ImGui("PointLight");

	modelManager_->FindModel("monsterBall.obj")->GetSpotLight()->ImGui("SpotLight");

	ImGui::Begin("PlayScene");
	ImGui::Text("Abutton or SpaceKey : ClearScene");
	ImGui::End();
};

void GamePlayScene::Draw()
{
	Model::PreDraw();

	Model::PostDraw();

	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	player_->Draw(camera_);

	Model::PostDraw();

	ParticleModel::PreDraw();

	player_->DrawParticle(camera_);

	ParticleModel::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);
	
	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();
};

void GamePlayScene::Finalize()
{

}