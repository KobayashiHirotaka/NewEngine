/**
 * @file GameTitleScene.h
 * @brief タイトルシーンの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Application/Game/Scenes/IScene.h"
#include "Application/Game/GuideUI/Guide.h"
#include "Application/GameObject/Skydome/Skydome.h"
#include "GameTitleSceneUI.h"

class GameTitleScene : public IScene
{
public:
	/// <summary>コンストラクタ</summary>
	GameTitleScene();

	/// <summary>デストラクタ</summary>
	~GameTitleScene();

	/// <summary>初期化</summary>
	void Initialize()override;

	/// <summary>更新</summary>
	void Update()override;

	/// <summary>描画</summary>
	void Draw()override;

	/// <summary>終了</summary>
	void Finalize()override;

	/// <summary>ImGui</summary>
	void ImGui()override;

private:
	//TextureManager
	Engine::TextureManager* textureManager_ = nullptr;

	//ModelManager
	ModelManager* modelManager_ = nullptr;

	//Input
	Engine::Input* input_ = nullptr;

	//Audio
	Engine::Audio* audio_ = nullptr;

	//Camera
	Camera camera_;
	DebugCamera debugCamera_;
	bool isDebugCamera_ = false;

	//Skydome
	std::unique_ptr<Skydome> skydome_;

	//GameTitleSceneUI
	std::unique_ptr<GameTitleSceneUI> gameTitleSceneUI_;

	//Guide
	std::unique_ptr<Guide> guide_;

	//サウンド
	uint32_t titleSoundHandle_ = 0u;
	uint32_t selectSoundHandle_ = 0u;

	//大きさ
	float volume_ = 1.0f;

	//再生されているか
	bool isPlayAudio_ = false;
};
