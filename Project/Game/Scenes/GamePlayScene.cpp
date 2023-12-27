#include "GamePlayScene.h"
#include "Project/Game/Scenes/Manager/SceneManager.h"
#include "GameStartScene.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>
#include <algorithm>

GamePlayScene::GamePlayScene() {};

GamePlayScene::~GamePlayScene() {};

void GamePlayScene::Initialize(SceneManager* sceneManager)
{

};

void GamePlayScene::Update(SceneManager* sceneManager)
{
	ImGui::Begin("Play");

	ImGui::End();
};

void GamePlayScene::Draw(SceneManager* sceneManager)
{

};