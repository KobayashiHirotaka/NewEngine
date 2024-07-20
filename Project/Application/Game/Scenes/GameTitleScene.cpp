#include "GameTitleScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameTitleScene::GameTitleScene() {};

GameTitleScene::~GameTitleScene() {};

void GameTitleScene::Initialize()
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
	PostProcess::GetInstance()->SetIsLuminanceBasedOutlineActive(true);
	//PostProcess::GetInstance()->SetIsDepthBasedOutlineActive(true);

	//modelの読み込み
	//modelManager_->LoadModel("resource/models", "remakePlayer.gltf");

	//skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	//debugCameraの初期化
	debugCamera_.Initialize();

	//UIの初期化
	//title(モノクロファイター)のsprite
	titleTextureHandle_ = TextureManager::LoadTexture("resource/images/title.png");
	titleSprite_.reset(Sprite::Create(titleTextureHandle_, { titleSpritePosition_.x, titleSpritePosition_.y }));

	//title(操作用)のsprite
	titleUITextureHandle_ = TextureManager::LoadTexture("resource/images/titleUISub.png");
	titleUISprite_.reset(Sprite::Create(titleUITextureHandle_, { 0.0f,0.0f }));

	//基本操作説明用のsprite
	generalCommandListTextureHandle_ = TextureManager::LoadTexture("resource/images/GeneralCommandList.png");
	generalCommandListSprite_.reset(Sprite::Create(generalCommandListTextureHandle_, { 0.0f,0.0f }));

	//攻撃操作説明用のsprite
	attackCommandListTextureHandle_ = TextureManager::LoadTexture("resource/images/AttackCommandList.png");
	attackCommandListSprite_.reset(Sprite::Create(attackCommandListTextureHandle_, { 0.0f,0.0f }));

	//トランジション用のsprite
	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(Vector2{ 1280.0f,720.0f });

	//bgm,seの読み込み
	titleSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Title.mp3");
	selectSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Select.mp3");

	//bgmの再生
	audio_->StopAudio(titleSoundHandle_);
	audio_->SoundPlayMP3(titleSoundHandle_, true, 1.0f);
};

void GameTitleScene::Update()
{
	//skydomeの更新
	skydome_->Update();

	AnimationTitle();

	//シーン切り替え
	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			isTransitionStart_ = true;
			audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
		}
	}

	if (input_->PushKey(DIK_SPACE))
	{
		isTransitionStart_ = true;
		audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
	}

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
			sceneManager_->ChangeScene("GamePlayScene");
			return;
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

	//imGui
	ImGui::Begin("TitleScene");
	ImGui::End();

	skydome_->ImGui();
};

void GameTitleScene::Draw()
{
	Model::PreDraw();

	Model::PostDraw();

	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	//skydomeの描画
	skydome_->Draw(camera_);

	Model::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	if (!isOpen_)
	{
		titleSprite_->Draw();
	}

	if (!isOpen_)
	{
		titleUISprite_->Draw();
	}

	/*if (isOpen_ && spriteCount_ == 1)
	{
		generalCommandListSprite_->Draw();
	}

	if (isOpen_ && spriteCount_ == 2)
	{
		attackCommandListSprite_->Draw();
	}*/

	Sprite::PostDraw();

	ParticleModel::PreDraw();

	ParticleModel::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	transitionSprite_->Draw();

	Sprite::PostDraw();
};

void GameTitleScene::Finalize()
{

}

void GameTitleScene::AnimationTitle()
{
	//タイトルの文字を動かす
	titleSpriteMoveTimer_--;

	float startY = titleSpritePosition_.y;
	float endY = startY + titleSpriteMoveSpeed_;

	float interpolatedY = Lerp(startY, endY, 0.4f);

	titleSpritePosition_.y = interpolatedY;

	if (titleSpriteMoveTimer_ < 0)
	{
		titleSpriteMoveSpeed_ *= -1.0f;
		titleSpriteMoveTimer_ = 30;
	}

	titleSprite_->SetPosition(titleSpritePosition_);
}