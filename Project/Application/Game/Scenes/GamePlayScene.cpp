#include "GamePlayScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GamePlayScene::GamePlayScene() {};

GamePlayScene::~GamePlayScene() {};

void GamePlayScene::Initialize()
{
	//textureManagerのinstance
	textureManager_ = TextureManager::GetInstance();

	//modelManagerのinstance
	modelManager_ = ModelManager::GetInstance();

	//inputのinstance
	input_ = Input::GetInstance();

	//audioのinstance
	audio_ = Audio::GetInstance();

	//game3dObjectManagerのinstance
	game3dObjectManager_ = Game3dObjectManager::GetInstance();
	game3dObjectManager_->Initialize();

	//postProcessのinstance
	PostProcess::GetInstance()->SetIsPostProcessActive(true);

	//postEffectの切り替え
	PostProcess::GetInstance()->SetIsBloomActive(true);
	PostProcess::GetInstance()->SetIsVignetteActive(true);
	PostProcess::GetInstance()->SetIsGrayScaleActive(true);
	PostProcess::GetInstance()->SetIsGaussianFilterActive(true);

	//Levelの読み込み
	levelLoarder_ = LevelLoader::GetInstance();
	levelLoarder_->LoadLevel("LevelData");

	//modelの読み込み
	modelManager_->LoadModel("resource/skydome", "skydome.obj");

	//playerの生成、初期化
	player_ = game3dObjectManager_->GetGameObject<Player>("Player");

	//enemyの生成、初期化
	enemy_ = game3dObjectManager_->GetGameObject<Enemy>("Enemy");

	//skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	//debugCameraの初期化
	debugCamera_.Initialize();
};

void GamePlayScene::Update()
{
	//player,enemyの更新
	game3dObjectManager_->Update();

	//skydomeの更新
	skydome_->Update();

	//シーン切り替え
	if (input_->PushKey(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GameClearScene");
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

	//player,enemyの描画
	game3dObjectManager_->Draw(camera_);

	//skydomeの描画
	skydome_->Draw(modelManager_->FindModel("skydome.obj"), camera_);

	Model::PostDraw();

	ParticleModel::PreDraw();

	//playerのparticle描画
	player_->DrawParticle(camera_);

	//enemyのparticle描画
	enemy_->DrawParticle(camera_);

	ParticleModel::PostDraw();

	Model::BonePreDraw();

	//playerのbone描画
	player_->BoneDraw(camera_);

	//enemyのbone描画
	enemy_->BoneDraw(camera_);

	Model::BonePostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	player_->DrawSprite();
	
	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	Sprite::PostDraw();
};

void GamePlayScene::Finalize()
{
	
}