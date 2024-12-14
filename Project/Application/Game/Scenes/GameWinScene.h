/**
 * @file GameWinScene.h
 * @brief 勝利シーンの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "IScene.h"
#include "Application/GameObject/Skydome/Skydome.h"

class GameWinScene : public IScene
{
public:
	//トランジション用の時間
	static const int kTransitionTime = 60;

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

	//Win表示のSprite
	std::unique_ptr<Sprite>winSceneSprite_ = nullptr;
	uint32_t winSceneTextureHandle_ = 0;

	//サウンド
	uint32_t selectSoundHandle_ = 0u;

	//大きさ
	float volume_ = 1.0f;

	//再生されているか
	bool isPlayAudio_ = false;

	//トランジション
	//Sprite関係
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	uint32_t transitionTextureHandle_ = 0;
	Vector2 transitionTextureSize_ = { 1280.0f,720.0f };
	Vector4 transitionColor_ = { 0.0f,0.0f,0.0f,1.0f };

	//トランジションタイマー
	float transitionTimer_ = 0;

	//トランジション開始時のアルファ値
	const float kTransitionStartAlpha_ = 1.0f;

	//トランジション終了時のアルファ値
	const float kTransitionEndAlpha_ = 0.0f;

	//トランジション開始・終了フラグ
	bool isTransitionStart_ = false;
	bool isTransitionEnd_ = false;
};



