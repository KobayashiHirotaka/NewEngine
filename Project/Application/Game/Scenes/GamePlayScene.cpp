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

	game3dObjectManager_ = Game3dObjectManager::GetInstance();

	////Levelの読み込み
	//levelLoarder_ = LevelLoader::GetInstance();
	//levelLoarder_->LoadLevel("LevelData");

	PostProcess::GetInstance()->SetIsPostProcessActive(true);
	PostProcess::GetInstance()->SetIsBloomActive(true);
	PostProcess::GetInstance()->SetIsVignetteActive(true);
	PostProcess::GetInstance()->SetIsGrayScaleActive(true);
	PostProcess::GetInstance()->SetIsGaussianFilterActive(true);

	//modelの読み込み
	modelManager_->LoadModel("resource/skydome", "skydome.obj");
	modelManager_->LoadModel("resource/newEnemy", "newEnemy.gltf");

	//skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	//playerの生成、初期化
	//player_ = game3dObjectManager_->GetGameObject<Player>("Player");
	player_ = std::make_unique<Player>();
	player_->Initialize();

	debugCamera_.Initialize();

	camera_.UpdateMatrix();
};

void GamePlayScene::Update()
{
	//playerの更新
	player_->Update(modelManager_->FindModel("newEnemy.gltf"));

	//skydomeの更新
	skydome_->Update();

	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			sceneManager_->ChangeScene("GameClearScene");
			return;
		}
	}

	if (input_->PushKey(DIK_N))
	{
		PostProcess::GetInstance()->SetIsGaussianFilterActive(false);
	}

	if (input_->PushKey(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GameClearScene");
		return;
	}

	game3dObjectManager_->Update();

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
	ImGui::Text("Abutton or SpaceKey : ClearScene");
	ImGui::End();
};

void GamePlayScene::Draw()
{
	Model::PreDraw();

	Model::PostDraw();

	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	//game3dObjectManager_->Draw(camera_);

	//playerの描画
	player_->Draw(modelManager_->FindModel("newEnemy.gltf"), camera_);

	//skydomeの描画
	skydome_->Draw(modelManager_->FindModel("skydome.obj"), camera_);

	Model::PostDraw();

	ParticleModel::PreDraw();

	player_->DrawParticle(camera_);

	ParticleModel::PostDraw();

	Model::BonePreDraw();

	player_->BoneDraw(modelManager_->FindModel("newEnemy.gltf"), camera_);

	//testObject_->BoneDraw(worldTransformTestObject_, camera_, 0);

	Model::BonePostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);
	
	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();
};

void GamePlayScene::Finalize()
{

}