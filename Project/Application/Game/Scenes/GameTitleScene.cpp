/**
 * @file GameTitleScene.cpp
 * @brief タイトルシーンの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "GameTitleScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameTitleScene::GameTitleScene() {};

GameTitleScene::~GameTitleScene() {};

void GameTitleScene::Initialize()
{
	//TextureManagerのインスタンスの取得
	textureManager_ = TextureManager::GetInstance();

	//ModelManagerのインスタンスの取得
	modelManager_ = ModelManager::GetInstance();

	//Inputのインスタンスの取得
	input_ = Input::GetInstance();

	//Audioのインスタンスの取得
	audio_ = Audio::GetInstance();

	//PostProcessのインスタンスの取得
	PostProcess::GetInstance()->SetIsPostProcessActive(true);

	//PostEffectの切り替え
	PostProcess::GetInstance()->SetIsBloomActive(true);
	PostProcess::GetInstance()->SetIsGaussianFilterActive(true);
	PostProcess::GetInstance()->SetIsLuminanceBasedOutlineActive(true);

	//Skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

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
	attackCommandListTextureHandle_[0] = TextureManager::LoadTexture("resource/images/PlayDefaultAttackCommandList.png");
	attackCommandListSprite_[0].reset(Sprite::Create(attackCommandListTextureHandle_[0], {0.0f,0.0f}));

	attackCommandListTextureHandle_[1] = TextureManager::LoadTexture("resource/images/PlayFinisherAttackCommandList.png");
	attackCommandListSprite_[1].reset(Sprite::Create(attackCommandListTextureHandle_[1], { 0.0f,0.0f }));

	//Transition生成、初期化
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

	//BGM,SEの読み込み
	titleSoundHandle_ = audio_->LoadSoundMP3("resource/Sounds/BGM.mp3");
	selectSoundHandle_ = audio_->LoadSoundMP3("resource/Sounds/Select.mp3");

	//BGMの再生,停止
	if (!audio_->IsAudioPlaying(titleSoundHandle_))
	{
		const float bgmVolume = 0.2f;
		audio_->PlaySoundMP3(titleSoundHandle_, true, bgmVolume);
	}
};

void GameTitleScene::Update()
{
#ifdef _ADJUSTMENT

	//デバッグ用のシーン切り替え
	if (input_->PushKey(DIK_SPACE))
	{
		isTransitionStart_ = true;
		audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
	}

#endif 

	//操作説明の開閉処理
	UpdateCommandSprite();

	//Skydomeの更新
	skydome_->Update();

	//タイトルの文字を動かす
	AnimationTitle();

	//シーン切り替え
	if (input_->GetJoystickState() && !isOpen_)
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && isTransitionEnd_)
		{
			isTransitionStart_ = true;

			if (!isPlayAudio_)
			{
				audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
			}

			isPlayAudio_ = true;
		}
	}

	//Transition終了処理
	transition_->EndSceneTransition(isTransitionEnd_);

	//Transition開始処理
	transition_->StartSceneTransition(isTransitionStart_, sceneManager_, "GamePlayScene");

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

	Model::PreDraw();

	//Skydomeの描画
	skydome_->Draw(camera_);

	Model::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	//タイトルの描画
	if (!isOpen_)
	{
		titleSprite_->Draw();
	}

	if (!isOpen_)
	{
		titleUISprite_->Draw();
	}

	//操作説明の描画
	if (isOpen_ && spriteCount_ == CommandSpriteType::GeneralCommandSprite)
	{
		//基本操作
		generalCommandListSprite_->Draw();
	}

	if (isOpen_ && spriteCount_ == CommandSpriteType::ComboAttackCommandSprite)
	{
		//コンボ攻撃
		attackCommandListSprite_[0]->Draw();
	}

	if (isOpen_ && spriteCount_ == CommandSpriteType::FinisherAttackCommandSprite)
	{
		//必殺技攻撃
		attackCommandListSprite_[1]->Draw();
	}

	Sprite::PostDraw();

	ParticleModel::PreDraw();

	ParticleModel::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	//Transition用Spriteの描画
	transition_->Draw();

	Sprite::PostDraw();
};

void GameTitleScene::Finalize()
{

}

void GameTitleScene::ImGui()
{
	ImGui::Begin("TitleScene");
	ImGui::End();
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
	const float lerpSpeed = 0.4f;
	float interpolatedY = Lerp(startY, endY, lerpSpeed);
	titleSpritePosition_.y = interpolatedY;

	//切り替えし
	if (titleSpriteMoveTimer_ < 0)
	{
		titleSpriteMoveSpeed_ *= kSpriteMoveSpeed_;
		titleSpriteMoveTimer_ = kMaxSpriteMoveTime_;
	}

	titleSprite_->SetPosition(titleSpritePosition_);
}

void GameTitleScene::UpdateCommandSprite()
{
	//操作説明の開閉
	if (input_->GetJoystickState())
	{
		//オプションボタンを押して操作説明を開く
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_START) && !isOpen_)
		{
			audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
			isOpen_ = true;
			spriteCount_ = CommandSpriteType::GeneralCommandSprite;
		}
		//Bボタンを押して操作説明を閉じる
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_START) && isOpen_)
		{
			audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
			isOpen_ = false;
			spriteCount_ = CommandSpriteType::GeneralCommandSprite;
		}

		//操作説明が開いている場合
		if (isOpen_)
		{
			// スティック入力のクールダウンが終わっている場合のみ移動
			if (stickInputCooldown_ <= 0)
			{
				ChangeCommandSprite();
			}

			// クールダウンを減らす
			if (stickInputCooldown_ > 0)
			{
				stickInputCooldown_--;
			}
		}
	}
}

void GameTitleScene::ChangeCommandSprite()
{
	//スプライトの移動に必用な定数
	const int kNextSprite = 1;
	const int kPreviousSprite = -1;

	//操作説明の移動処理
	if ((input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_RIGHT) || input_->GetLeftStickX() > kValue_) &&
		spriteCount_ != CommandSpriteType::FinisherAttackCommandSprite)
	{
		//次の説明に進む
		ApplyCommandSprite(kNextSprite);
	}
	else if ((input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_LEFT) || input_->GetLeftStickX() < -kValue_) &&
		spriteCount_ != CommandSpriteType::GeneralCommandSprite)
	{
		//前の説明に戻る
		ApplyCommandSprite(kPreviousSprite);
	}
}

void GameTitleScene::ApplyCommandSprite(int changeAmount)
{
	//スプライトの遷移用のベクター
	static const std::vector<CommandSpriteType> spriteOrder = {
		CommandSpriteType::GeneralCommandSprite,
		CommandSpriteType::ComboAttackCommandSprite,
		CommandSpriteType::FinisherAttackCommandSprite
	};

	//現在のスプライトインデックスを取得
	auto it = std::find(spriteOrder.begin(), spriteOrder.end(), static_cast<CommandSpriteType>(spriteCount_));
	if (it == spriteOrder.end()) return;

	size_t currentIndex = std::distance(spriteOrder.begin(), it);

	//次のスプライトインデックスを計算
	size_t nextIndex = (currentIndex + changeAmount + spriteOrder.size()) % spriteOrder.size();

	//スプライトを変更
	spriteCount_ = spriteOrder[nextIndex];

	//サウンド再生とスティックのクールダウンを設定
	audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
	stickInputCooldown_ = kStickInputCooldownTime_;
}