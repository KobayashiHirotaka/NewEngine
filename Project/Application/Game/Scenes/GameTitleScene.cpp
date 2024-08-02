#include "GameTitleScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameTitleScene::GameTitleScene() {};

GameTitleScene::~GameTitleScene() {};

void GameTitleScene::Initialize()
{
	//TextureManagerのインスタンス
	textureManager_ = TextureManager::GetInstance();

	//ModelManagerのインスタンス
	modelManager_ = ModelManager::GetInstance();

	//Inputのインスタンス
	input_ = Input::GetInstance();

	//Audioのインスタンス
	audio_ = Audio::GetInstance();

	//PostProcessのインスタンス
	PostProcess::GetInstance()->SetIsPostProcessActive(true);

	//PostEffectの切り替え
	PostProcess::GetInstance()->SetIsBloomActive(true);
	PostProcess::GetInstance()->SetIsGaussianFilterActive(true);
	PostProcess::GetInstance()->SetIsLuminanceBasedOutlineActive(true);

	//Skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	//Skyboxの生成、初期化
	skybox_.reset(Skybox::Create());

	skyboxWorldTransform_.Initialize();
	skyboxWorldTransform_.scale = { 500.0f, 500.0f, 500.0f };

	//DebugCameraの初期化
	debugCamera_.Initialize();

	//UIの初期化
	//タイトル(モノクロファイター)のSprite
	titleTextureHandle_ = TextureManager::LoadTexture("resource/images/title.png");
	titleSprite_.reset(Sprite::Create(titleTextureHandle_, { titleSpritePosition_.x, titleSpritePosition_.y }));

	//タイトル(操作用)のSprite
	titleUITextureHandle_ = TextureManager::LoadTexture("resource/images/titleUI.png");
	titleUISprite_.reset(Sprite::Create(titleUITextureHandle_, { 0.0f,0.0f }));

	//基本操作説明用のSprite
	generalCommandListTextureHandle_ = TextureManager::LoadTexture("resource/images/PlayGeneralCommandList.png");
	generalCommandListSprite_.reset(Sprite::Create(generalCommandListTextureHandle_, { 0.0f,0.0f }));

	//攻撃操作説明用のSprite
	attackCommandListTextureHandle_ = TextureManager::LoadTexture("resource/images/PlayAttackCommandList.png");
	attackCommandListSprite_.reset(Sprite::Create(attackCommandListTextureHandle_, { 0.0f,0.0f }));

	//トランジション用のSprite
	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(Vector2{ 1280.0f,720.0f });

	//BGM,SEの読み込み
	titleSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/BGM.mp3");
	selectSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Select.mp3");

	//BGMの再生,停止
	audio_->StopAudio(titleSoundHandle_);
	audio_->SoundPlayMP3(titleSoundHandle_, true, 0.2f);
};

void GameTitleScene::Update()
{
	//Skydomeの更新
	skydome_->Update();

	//タイトルの文字を動かす
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

	//デバッグ用のシーン切り替え
	if (input_->PushKey(DIK_SPACE))
	{
		isTransitionStart_ = true;
		audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
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
			sceneManager_->ChangeScene("GamePlayScene");
			return;
		}
	}

	//操作説明の開閉
	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && !isOpen_)
		{
			audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
			isOpen_ = true;
			spriteCount_ = 1;
		}
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && isOpen_)
		{
			audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
			isOpen_ = false;
			spriteCount_ = 0;
		}

		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_RIGHT) && isOpen_ && spriteCount_ == 1)
		{
			audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
			spriteCount_ = 2;
		}

		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_LEFT) && isOpen_ && spriteCount_ == 2)
		{
			audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
			spriteCount_ = 1;
		}
	}

	skyboxWorldTransform_.UpdateMatrixEuler();

	//Camera、DebugCameraの処理
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

void GameTitleScene::Draw()
{
	Model::PreDraw();

	Model::PostDraw();

	PostProcess::GetInstance()->PreDraw();

	Skybox::PreDraw();

	skybox_->Draw(skyboxWorldTransform_, camera_);

	Skybox::PostDraw();

	Model::PreDraw();

	//Skydomeの描画
	//skydome_->Draw(camera_);

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

	//操作説明
	if (isOpen_ && spriteCount_ == 1)
	{
		generalCommandListSprite_->Draw();
	}

	if (isOpen_ && spriteCount_ == 2)
	{
		attackCommandListSprite_->Draw();
	}

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

void GameTitleScene::ImGui()
{
	ImGui::Begin("TitleScene");
	ImGui::End();

	skydome_->ImGui();
}

void GameTitleScene::AnimationTitle()
{
	//タイトルの文字を動かす
	titleSpriteMoveTimer_--;

	//開始座標
	float startY = titleSpritePosition_.y;

	//終了座標
	float endY = startY + titleSpriteMoveSpeed_;

	//移動
	float interpolatedY = Lerp(startY, endY, 0.4f);
	titleSpritePosition_.y = interpolatedY;

	//切り替えし
	if (titleSpriteMoveTimer_ < 0)
	{
		titleSpriteMoveSpeed_ *= -1.0f;
		titleSpriteMoveTimer_ = 30;
	}

	titleSprite_->SetPosition(titleSpritePosition_);
}