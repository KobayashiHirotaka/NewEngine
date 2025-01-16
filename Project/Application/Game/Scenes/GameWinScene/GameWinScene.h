/**
 * @file GameWinScene.h
 * @brief 勝利シーンの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Application/Game/Scenes/IScene.h"
#include "Application/GameObject/Skydome/Skydome.h"
#include "GameWinSceneUI.h"

class GameWinScene : public IScene
{
public:
	/// <summary>コンストラクタ</summary>
	GameWinScene();

	/// <summary>デストラクタ</summary>
	~GameWinScene();

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
	TextureManager* textureManager_ = nullptr;

	//ModelManager
	ModelManager* modelManager_ = nullptr;

	//Input
	Input* input_ = nullptr;

	//Audio
	Audio* audio_ = nullptr;

	//Camera
	Camera camera_;
	DebugCamera debugCamera_;
	bool isDebugCamera_ = false;

	//Skydome
	std::unique_ptr<Skydome> skydome_;

	//GameWinSceneUI
	std::unique_ptr<GameWinSceneUI> gameWinSceneUI_;

	//サウンド
	uint32_t selectSoundHandle_ = 0u;

	//大きさ
	float volume_ = 1.0f;

	//再生されているか
	bool isPlayAudio_ = false;
};



