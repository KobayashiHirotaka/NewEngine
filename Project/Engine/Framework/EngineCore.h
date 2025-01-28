/**
 * @file EngineCore.h
 * @brief エンジン機能の管理(初期化、更新、描画など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/WindowsApp/WindowsApp.h"
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/Base/D3DResourceLeakChecker.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/Line/Line.h"
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include "Engine/3D/Particle/Random.h"
#include "Engine/3D/Model/Game3dObjectFactory.h"
#include "Engine/3D/Model/Game3dObjectManager.h"
#include "Engine/Utility/LevelLoader/LevelLoader.h"
#include "Engine/Framework/SceneManager.h"
#include "Application/Game/GameTimer/GameTimer.h"

class EngineCore
{
public:
	/// <summary>デストラクタ</summary>
	virtual ~EngineCore() = default;

	/// <summary>初期化</summary>
	virtual void Initialize();

	/// <summary>終了</summary>
	virtual void Finalize();

	/// <summary>更新</summary>
	virtual void Update();

	/// <summary>描画</summary>
	virtual void Draw() = 0;

	/// <summary>終了リクエスト</summary>
	virtual bool IsEndRequst();

	/// <summary>メインループ</summary>
	void Run();

protected:
	//リークチェッカー
	static D3DResourceLeakChecker sLeakCheck_;

	//WindowsAPPのポインタ
	WindowsApp* win_ = nullptr;

	//DirectXCoreのポインタ
	DirectXCore* dxCore_ = nullptr;

	//TextureManagerのポインタ
	Engine::TextureManager* textureManager_ = nullptr;
	
	//ImGuiManagerのポインタ
	ImGuiManager* imguiManager_ = nullptr;

	//Audioのポインタ
	Engine::Audio* audio_ = nullptr;

	//Inputのポインタ
	Engine::Input* input_ = nullptr;

	//PostProcessのポインタ
	Engine::PostProcess* postProcess_ = nullptr;

	//Game3dObjectFactoryのポインタ
	std::unique_ptr<Game3dObjectFactory> game3dObjectFactory_ = nullptr;

	//Game3dObjectManagerのポインタ
	Game3dObjectManager* game3dObjectManager_ = nullptr;

	//LevelLoaderのポインタ
	LevelLoader* levelLoader_ = nullptr;

	//SceneManagerのポインタ
	SceneManager* sceneManager_ = nullptr;

	//終了リクエストフラグ
	bool endRequst_ = false;

	//シーン生成用ファクトリー
	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;
};

