#include "GameStartScene.h"
#include "Project/Game/Scenes/Manager/SceneManager.h"
#include "GamePlayScene.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>
#include <algorithm>

GameStartScene::GameStartScene() {};

GameStartScene::~GameStartScene() {};

void GameStartScene::Initialize(SceneManager* sceneManager)
{
	
};

void GameStartScene::Update(SceneManager* sceneManager)
{
	ImGui::Begin("Start");

	ImGui::End();
	/*sceneManager->ChangeScene(new GamePlayScene);*/
};

void GameStartScene::Draw(SceneManager* sceneManager)
{
	
};