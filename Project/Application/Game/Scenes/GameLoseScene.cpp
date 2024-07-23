#include "GameLoseScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameLoseScene::GameLoseScene() {};

GameLoseScene::~GameLoseScene() {};

void GameLoseScene::Initialize()
{
	//textureManagerのinstance
	textureManager_ = TextureManager::GetInstance();

	//modelManagerのinstance
	modelManager_ = ModelManager::GetInstance();

	//inputのinstance
	input_ = Input::GetInstance();

	//audioのinstance
	audio_ = Audio::GetInstance();

	//postProcessのinstance
	PostProcess::GetInstance()->SetIsPostProcessActive(true);

	//postEffectの切り替え
	PostProcess::GetInstance()->SetIsBloomActive(true);
	PostProcess::GetInstance()->SetIsGaussianFilterActive(true);
	PostProcess::GetInstance()->SetIsVignetteActive(false);

	//modelの読み込み
	//modelManager_->LoadModel("resource/skydome", "skydome.obj");

	//skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	//debugCameraの初期化
	debugCamera_.Initialize();

	loseSceneTextureHandle_ = TextureManager::LoadTexture("resource/images/LoseScene.png");
	loseSceneSprite_.reset(Sprite::Create(loseSceneTextureHandle_, { 0.0f,0.0f }));

	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(Vector2{ 1280.0f,720.0f });

	selectSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Select.mp3");
};

void GameLoseScene::Update()
{
	//skydomeの更新
	skydome_->Update();

	//シーン切り替え
	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			if (isTransitionEnd_)
			{
				isTransitionStart_ = true;
				audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
			}
		}
	}

	if (input_->PushKey(DIK_SPACE))
	{
		sceneManager_->ChangeScene("GameTitleScene");
		return;
	}

	//トランジション
	if (!isTransitionEnd_)
	{
		transitionTimer_ += 1.0f / kTransitionTime;
		transitionColor_.w = Lerp(transitionColor_.w, 0.0f, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w <= 0.0f)
		{
			isTransitionEnd_ = true;
			transitionTimer_ = 0.0f;
		}
	}

	if (isTransitionStart_)
	{
		transitionTimer_ += 1.0f / kTransitionTime;
		transitionColor_.w = Lerp(transitionColor_.w, 1.0f, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w >= 1.0f)
		{
			sceneManager_->ChangeScene("GameTitleScene");
		}
	}

	//camera、debugCameraの処理
	debugCamera_.Update();

	if (input_->PushKey(DIK_K))
	{
		isDebugCamera_ = true;
	}
	else if (input_->PushKey(DIK_L))
	{
		isDebugCamera_ = false;
	}

	if (isDebugCamera_)
	{
		camera_.matView_ = debugCamera_.GetCamera().matView_;
		camera_.matProjection_ = debugCamera_.GetCamera().matProjection_;
		camera_.TransferMatrix();
	}
	else
	{
		camera_.UpdateMatrix();
	}
};

void GameLoseScene::Draw()
{
	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	//skydomeの描画
	skydome_->Draw(camera_);

	Model::PostDraw();

	ParticleModel::PreDraw();

	ParticleModel::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	loseSceneSprite_->Draw();

	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	transitionSprite_->Draw();

	Sprite::PostDraw();
};

void GameLoseScene::Finalize()
{

}

void GameLoseScene::ImGui()
{
	ImGui::Begin("LoseScene");
	ImGui::Text("Abutton or SpaceKey : TitleScene");
	ImGui::End();
}