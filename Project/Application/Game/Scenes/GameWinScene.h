/**
 * @file GameWinScene.h
 * @brief 勝利シーンの管理(初期化、更新、描画など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "IScene.h"
#include "Engine/3D/Skybox/Skybox.h"
#include "Application/GameObject/Skydome/Skydome.h"

class GameWinScene : public IScene
{
public:
	//Transition用の時間
	static const int kTransitionTime = 60;

	GameWinScene();

	~GameWinScene();

	void Initialize()override;

	void Update()override;

	void Draw()override;

	void Finalize()override;

	void ImGui()override;

private:
	TextureManager* textureManager_ = nullptr;

	ModelManager* modelManager_ = nullptr;

	Input* input_ = nullptr;

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

	//Sounds
	uint32_t selectSoundHandle_ = 0u;

	//Transition
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	uint32_t transitionTextureHandle_ = 0;
	Vector4 transitionColor_ = { 0.0f,0.0f,0.0f,1.0f };
	float transitionTimer_ = 0;
	bool isTransitionStart_ = false;
	bool isTransitionEnd_ = false;

	//Skybox
	std::unique_ptr<Skybox> skybox_;
	WorldTransform skyboxWorldTransform_;

	bool isPlayAudio_ = false;
};



