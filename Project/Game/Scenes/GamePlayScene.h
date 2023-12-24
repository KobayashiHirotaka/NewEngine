#pragma once
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
	DebugCamera* debugCamera_ = nullptr;

	TextureManager* textureManager_ = nullptr;

	Light* light_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	DirectXCore* dxCore_ = nullptr;

	Model* model_ = nullptr;

	Game3dObjectManager* game3dObjectManager_ = nullptr;

	WorldTransform worldTransform_[2];

	Camera camera_;
	
	Vector4 color_ = { 1.0f,1.0f,1.0f,0.5f };
};
