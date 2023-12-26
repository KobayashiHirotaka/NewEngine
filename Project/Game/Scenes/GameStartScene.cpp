#include "GameStartScene.h"

void GameStartScene::Initialize()
{
	
}

void GameStartScene::Update()
{
	ImGui::Begin("SceneNum");
	ImGui::Text("SceneNum %d", sceneNum);
	ImGui::InputInt("SceneNum", &sceneNum);
	ImGui::End();
}

void GameStartScene::Draw()
{

}
