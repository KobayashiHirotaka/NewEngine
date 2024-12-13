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
	attackCommandListTextureHandle_[0] = TextureManager::LoadTexture("resource/images/NewPlayAttackCommandList.png");
	attackCommandListSprite_[0].reset(Sprite::Create(attackCommandListTextureHandle_[0], {0.0f,0.0f}));

	attackCommandListTextureHandle_[1] = TextureManager::LoadTexture("resource/images/NewPlayAttackCommandList2.png");
	attackCommandListSprite_[1].reset(Sprite::Create(attackCommandListTextureHandle_[1], { 0.0f,0.0f }));

	//トランジション用のSprite
	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(transitionTextureSize_);

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

			if (!isPlayAudio_)
			{
				audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
			}

			isPlayAudio_ = true;
		}
	}

	//トランジション
	const float deltaTime = 1.0f / kTransitionTime;

	if (!isTransitionEnd_)
	{
		transitionTimer_ += deltaTime;
		transitionColor_.w = Lerp(transitionColor_.w, kTransitionEndAlpha_, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w <= kTransitionEndAlpha_)
		{
			isTransitionEnd_ = true;
			transitionTimer_ = 0.0f;
		}
	}

	if (isTransitionStart_)
	{
		transitionTimer_ += deltaTime;
		transitionColor_.w = Lerp(transitionColor_.w, kTransitionStartAlpha_, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w >= kTransitionStartAlpha_)
		{
			sceneManager_->ChangeScene("GamePlayScene");
			return;
		}
	}

	//クールダウンタイム
	const int kStickInputCooldownTime = 10;

	//操作説明の開閉
	if (input_->GetJoystickState())
	{
		//Bボタンを押して操作説明を開く
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && !isOpen_)
		{
			audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
			isOpen_ = true;
			spriteCount_ = static_cast<int>(CommandSpriteType::GeneralCommandSprite);
		}
		//Bボタンを押して操作説明を閉じる
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && isOpen_)
		{
			audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
			isOpen_ = false;
			spriteCount_ = static_cast<int>(CommandSpriteType::GeneralCommandSprite);
		}

		//操作説明が開いている場合
		if (isOpen_)
		{
			//右の方向キーかスティック右入力で次の説明へ
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_RIGHT) || (input_->GetLeftStickX() > value_ && stickInputCooldown_ <= 0))
			{
				if (spriteCount_ != static_cast<int>(CommandSpriteType::FinisherAttackCommandSprite)) 
				{
					if (spriteCount_ == static_cast<int>(CommandSpriteType::GeneralCommandSprite))
					{
						spriteCount_ = static_cast<int>(CommandSpriteType::ComboAttackCommandSprite);
					}
					else if (spriteCount_ == static_cast<int>(CommandSpriteType::ComboAttackCommandSprite))
					{
						spriteCount_ = static_cast<int>(CommandSpriteType::FinisherAttackCommandSprite);
					}

					audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
					stickInputCooldown_ = kStickInputCooldownTime; 
				}
			}
			//左の方向キーかスティック左入力で前の説明に戻る
			else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_LEFT) || (input_->GetLeftStickX() < -value_ && stickInputCooldown_ <= 0))
			{
				if (spriteCount_ != static_cast<int>(CommandSpriteType::GeneralCommandSprite)) 
				{
					if (spriteCount_ == static_cast<int>(CommandSpriteType::FinisherAttackCommandSprite))
					{
						spriteCount_ = static_cast<int>(CommandSpriteType::ComboAttackCommandSprite);
					}
					else if (spriteCount_ == static_cast<int>(CommandSpriteType::ComboAttackCommandSprite))
					{
						spriteCount_ = static_cast<int>(CommandSpriteType::GeneralCommandSprite);
					}

					audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
					stickInputCooldown_ = kStickInputCooldownTime;
				}
			}

			// クールダウンを減らす
			if (stickInputCooldown_ > 0)
			{
				stickInputCooldown_--;
			}
		}
	}

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
	if (isOpen_ && spriteCount_ == static_cast<int>(CommandSpriteType::GeneralCommandSprite))
	{
		//基本操作
		generalCommandListSprite_->Draw();
	}

	if (isOpen_ && spriteCount_ == static_cast<int>(CommandSpriteType::ComboAttackCommandSprite))
	{
		//コンボ攻撃
		attackCommandListSprite_[0]->Draw();
	}

	if (isOpen_ && spriteCount_ == static_cast<int>(CommandSpriteType::FinisherAttackCommandSprite))
	{
		//必殺技攻撃
		attackCommandListSprite_[1]->Draw();
	}

	Sprite::PostDraw();

	ParticleModel::PreDraw();

	ParticleModel::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	//トランジション
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