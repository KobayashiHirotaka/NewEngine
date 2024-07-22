#include "IScene.h"
#include "Application/GameObject/Skydome/Skydome.h"

class GameWinScene : public IScene
{
public:
	static const int kTransitionTime = 60;

	GameWinScene();

	~GameWinScene();

	void Initialize()override;

	void Update()override;

	void Draw()override;

	void Finalize()override;

	void ImGui()override;

private:
	//textureManager
	TextureManager* textureManager_ = nullptr;

	//modelManager
	ModelManager* modelManager_ = nullptr;

	//input
	Input* input_ = nullptr;

	//audio
	Audio* audio_ = nullptr;

	//camera
	Camera camera_;
	DebugCamera debugCamera_;
	bool isDebugCamera_ = false;

	//skydome
	std::unique_ptr<Skydome> skydome_;

	std::unique_ptr<Sprite>winSceneSprite_ = nullptr;
	uint32_t winSceneTextureHandle_ = 0;

	//サウンド
	uint32_t selectSoundHandle_ = 0u;

	//トランジション
	std::unique_ptr<Sprite> transitionSprite_ = nullptr;
	uint32_t transitionTextureHandle_ = 0;
	Vector4 transitionColor_ = { 0.0f,0.0f,0.0f,1.0f };
	float transitionTimer_ = 0;
	bool isTransitionStart_ = false;
	bool isTransitionEnd_ = false;
};



