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
	modelManager_->LoadModel("resource/models", "tryangle.gltf");

	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->SetModel(modelManager_->FindModel("hammer.obj"));

	PostProcess::GetInstance()->SetIsPostProcessActive(true);

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
			sceneManager_->ChangeScene("GameTitleScene");
			return;
		}
	}

	if (input_->PushKey(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GameTitleScene");
		return;
	}

	//モデル切り替え
	if (input_->PushKey(DIK_RETURN) && modelCount_ == 0)
	{
		modelCount_ = 1;
		player_->SetModel(modelManager_->FindModel("tryangle.gltf"));
	}
	else if (input_->PushKey(DIK_RETURN) && modelCount_ == 1)
	{
		modelCount_ = 0;
		player_->SetModel(modelManager_->FindModel("hammer.obj"));
	}

	modelManager_->FindModel("hammer.obj")->GetLight()->ImGui("Light");
	//modelManager_->FindModel("monsterBall.obj")->GetLight()->ImGui("Light");
	
	//Bloom
	if (input_->PressKey(DIK_1))
	{
		PostProcess::GetInstance()->SetIsBloomActive(true);
	}

	//Vignette
	if (input_->PressKey(DIK_2))
	{
		PostProcess::GetInstance()->SetIsVignetteActive(true);
	}

	//Disable PostEffect
	if (input_->PressKey(DIK_3))
	{
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

	ImGui::Begin("PlayScene");
	ImGui::Text("A button or Space Key : TitleScene");
	ImGui::Text("Enter Key : ModelChange");
	ImGui::Text("K Key : DebugCamera On");
	ImGui::Text("L Key : DebugCamera Off");
	ImGui::Text("WASD Key : DebugCamera Move");
	ImGui::Text("Arrow Key : DebugCamera Rotate");
	ImGui::Text("1 Key : Bloom");
	ImGui::Text("2 Key : Vignette");
	ImGui::Text("3 Key : Disable PostEffect");
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

	Sprite::PreDraw(Sprite::kBlendModeNormal);
	
	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();
};

void GamePlayScene::Finalize()
{

}