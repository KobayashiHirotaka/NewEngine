#include "GamePlayScene.h"
#include "Engine/Utility/GlobalVariables.h"

GamePlayScene::GamePlayScene()
{

}

GamePlayScene::~GamePlayScene()
{
	
}

void GamePlayScene::Initialize()
{
	debugCamera_ = new DebugCamera();
	debugCamera_->Initialize(1280, 720);

	dxCore_ = DirectXCore::GetInstance();
	
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureManager_ = TextureManager::GetInstance();

	light_ = Light::GetInstance();
	light_->Initialize();

	game3dObjectManager_ = Game3dObjectManager::GetInstance();
	game3dObjectManager_->Initialize();
	
	camera_.Initialize();
}

void GamePlayScene::Update()
{
	debugCamera_->Update();

	game3dObjectManager_->Update();


	camera_.UpdateMatrix();
}

void GamePlayScene::Draw()
{
	
}