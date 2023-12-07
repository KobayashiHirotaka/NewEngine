#include "GameStartScene.h"

void GameStartScene::Initialize()
{
	
}

void GameStartScene::Update()
{
	Quaternion rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f,0.71f,0.0f }, 0.3f);
	Quaternion rotation1 = { -rotation0.x,-rotation0.y,-rotation0.z,-rotation0.w };

	Quaternion interpolate0 = Slerp(rotation0, rotation1, 0.0f);
	Quaternion interpolate1 = Slerp(rotation0, rotation1, 0.3f);
	Quaternion interpolate2 = Slerp(rotation0, rotation1, 0.5f);
	Quaternion interpolate3 = Slerp(rotation0, rotation1, 0.7f);
	Quaternion interpolate4 = Slerp(rotation0, rotation1, 1.0f);

	ImGui::Begin("Quaternion");

	ImGui::Text("%1.2f,%1.2f,%1.2f,%1.2f      : interpolate0, Slerp(q0, q1, 0.0f)",
		interpolate0.x, interpolate0.y, interpolate0.z, interpolate0.w);
	ImGui::Text("%1.2f,%1.2f,%1.2f,%1.2f      : interpolate1, Slerp(q0, q1, 0.3f)",
		interpolate1.x, interpolate1.y, interpolate1.z, interpolate1.w);
	ImGui::Text("%1.2f,%1.2f,%1.2f,%1.2f      : interpolate2, Slerp(q0, q1, 0.5f)",
		interpolate2.x, interpolate2.y, interpolate2.z, interpolate2.w);
	ImGui::Text("%1.2f,%1.2f,%1.2f,%1.2f      : interpolate3, Slerp(q0, q1, 0.7f)",
		interpolate3.x, interpolate3.y, interpolate3.z, interpolate3.w);
	ImGui::Text("%1.2f,%1.2f,%1.2f,%1.2f      : interpolate4, Slerp(q0, q1, 1.0f)",
		interpolate4.x, interpolate4.y, interpolate4.z, interpolate4.w);

	ImGui::End();

	ImGui::Begin("SceneNum");
	ImGui::Text("SceneNum %d", sceneNum);
	ImGui::InputInt("SceneNum", &sceneNum);
	ImGui::End();
}

void GameStartScene::Draw()
{

}
