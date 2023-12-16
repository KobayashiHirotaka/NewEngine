#pragma once
#include "Engine/Base/MyEngine/MyEngine.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Camera/DebugCamera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/2D/Triangle/Triangle.h"
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/3D/Sphere/Sphere.h"
#include "Engine//3D/Model/Game3dObjectManager.h"
#include "Engine/3D/Light/Light.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/Utility/Structs/ModelData.h"
#include "IScene.h"

#define DIRECTINPUT_VERSION 0x0800//DirectInputのバージョン指定
#include <dinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

class GamePlayScene : public IScene
{
public:
	GamePlayScene();
	~GamePlayScene();

	void Initialize()override;
	void Update()override;
	void Draw()override;

private:
	MyEngine* engine_ = nullptr;

	DebugCamera* debugCamera_ = nullptr;

	TextureManager* textureManager_ = nullptr;

	Light* light_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	DirectXCore* dxCore_ = nullptr;

	Model* model_ = nullptr;

	Game3dObjectManager* game3dObjectManager_ = nullptr;

	WorldTransform worldTransform_[2];

	WorldTransform worldTransformSprite_;

	WorldTransform worldTransformModel_;

	Camera camera_;

	Sprite* sprite_;

	Sphere* sphere_[2];
	
	int texture_;

	bool changeTexture_;

	int monsterBall_;

	int uvChecker_;

	int white_;

	uint32_t sound_;
	
	Vector4 LeftTop_[2] = {
		{ 0.0f,0.0f,0.0f,1.0f },
		{ 360.0f,0.0f,0.0f,1.0f }
	};
	Vector4 LeftBottom_[2] = {
		{ 0.0f,360.0f,0.0f,1.0f },
		{ 360.0f,360.0f,0.0f,1.0f }
	};
	Vector4 RightTop_[2] = {
		{ 360.0f,0.0f,0.0f,1.0f },
		{ 640.0f,0.0f,0.0f,1.0f }
	};
	Vector4 RightBottom_[2] = {
		{ 360.0f,180.0f,0.0f,1.0f },
		{ 640.0f,360.0f,0.0f,1.0f }
	};

	int count_ = 0;

	Vector4 color_ = { 1.0f,1.0f,1.0f,0.5f };
};
