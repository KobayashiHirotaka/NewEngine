/**
 * @file IScene.h
 * @brief シーンの基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Camera/DebugCamera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/3D/Particle/ParticleSystem.h"
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/Utility/Collision/CollisionManager.h"
#include "Engine/Utility/LevelLoader/LevelLoader.h"
#include "Application/Game/Transition/Transition.h"
#include <memory>

class SceneManager;

class IScene 
{
public:
	/// <summary>デストラクタ</summary>
	virtual ~IScene() = default;

	/// <summary>初期化</summary>
	virtual void Initialize() = 0;

	/// <summary>更新</summary>
	virtual void Update() = 0;

	/// <summary>描画</summary>
	virtual void Draw() = 0;

	/// <summary>終了</summary>
	virtual void Finalize() = 0;

	/// <summary>ImGui</summary>
	virtual void ImGui() = 0;

	//Setter
	//SceneManager
	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; };

protected:
	//SceneManagerのポインタ
	SceneManager* sceneManager_ = nullptr;

	//Transitionのポインタ
	std::unique_ptr<Transition> transition_ = nullptr;

	//Transition開始・終了フラグ
	bool isTransitionStart_ = false;
	bool isTransitionEnd_ = false;
};