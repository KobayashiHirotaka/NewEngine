#include "GameTitleSceneUI.h"

//スプライトの遷移用のベクター
const std::vector<CommandSpriteType> GameTitleSceneUI::sSpriteOrder_ = {
	CommandSpriteType::GeneralCommandSprite,
	CommandSpriteType::ComboAttackCommandSprite,
	CommandSpriteType::FinisherAttackCommandSprite
};

void GameTitleSceneUI::Initialize()
{
	//Inputのインスタンスの取得
	input_ = Input::GetInstance();

	//Audioのインスタンスの取得
	audio_ = Audio::GetInstance();

	//UIの初期化
	//タイトル(モノクロファイター)のSprite
	titleTextureHandle_ = TextureManager::LoadTexture("Resource/Images/Title.png");
	titleSprite_.reset(Sprite::Create(titleTextureHandle_, { titleSpritePosition_.x, titleSpritePosition_.y }));

	//タイトル(操作用)のSprite
	titleUITextureHandle_ = TextureManager::LoadTexture("Resource/Images/TitleUI.png");
	titleUISprite_.reset(Sprite::Create(titleUITextureHandle_, { 0.0f,0.0f }));

	//基本操作説明用のSprite
	generalCommandListTextureHandle_ = TextureManager::LoadTexture("Resource/Images/PlayGeneralGuide.png");
	generalCommandListSprite_.reset(Sprite::Create(generalCommandListTextureHandle_, { 0.0f,0.0f }));

	//攻撃操作説明用のSprite
	attackCommandListTextureHandle_[0] = TextureManager::LoadTexture("Resource/Images/PlayDefaultAttackGuide.png");
	attackCommandListSprite_[0].reset(Sprite::Create(attackCommandListTextureHandle_[0], { 0.0f,0.0f }));

	attackCommandListTextureHandle_[1] = TextureManager::LoadTexture("Resource/Images/PlayFinisherAttackGuide.png");
	attackCommandListSprite_[1].reset(Sprite::Create(attackCommandListTextureHandle_[1], { 0.0f,0.0f }));
}

void GameTitleSceneUI::Update()
{
	UpdateCommandSprite();

	AnimationTitle();
}

void GameTitleSceneUI::Draw()
{
	//タイトルの描画
	if (!isOpen_)
	{
		titleSprite_->Draw();
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
}

void GameTitleSceneUI::UpdateCommandSprite()
{
	//操作説明の開閉
	if (input_->GetJoystickState())
	{
		//オプションボタンを押して操作説明を開く
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_START) && !isOpen_)
		{
			isChangedSprite_ = true;
			isOpen_ = true;
			spriteCount_ = CommandSpriteType::GeneralCommandSprite;
		}
		//Bボタンを押して操作説明を閉じる
		else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_START) && isOpen_)
		{
			isChangedSprite_ = true;
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

void GameTitleSceneUI::ChangeCommandSprite()
{
	//スプライトの移動に必用な定数
	const int kNextSprite = 1;
	const int kPreviousSprite = -1;

	//操作説明の移動処理
	if ((input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_RIGHT) || input_->GetLeftStickX() > kStickDeadZone_) &&
		spriteCount_ != CommandSpriteType::FinisherAttackCommandSprite)
	{
		isChangedSprite_ = true;
		//次の説明に進む
		ApplyCommandSprite(kNextSprite);
	}
	else if ((input_->IsPressButtonEnter(XINPUT_GAMEPAD_DPAD_LEFT) || input_->GetLeftStickX() < -kStickDeadZone_) &&
		spriteCount_ != CommandSpriteType::GeneralCommandSprite)
	{
		isChangedSprite_ = true;
		//前の説明に戻る
		ApplyCommandSprite(kPreviousSprite);
	}
}

void GameTitleSceneUI::ApplyCommandSprite(int changeAmount)
{
	//現在のスプライトインデックスを取得
	auto it = std::find(sSpriteOrder_.begin(), sSpriteOrder_.end(), static_cast<CommandSpriteType>(spriteCount_));
	if (it == sSpriteOrder_.end()) return;

	size_t currentIndex = std::distance(sSpriteOrder_.begin(), it);

	//次のスプライトインデックスを計算
	size_t nextIndex = (currentIndex + changeAmount + sSpriteOrder_.size()) % sSpriteOrder_.size();

	//スプライトを変更
	spriteCount_ = sSpriteOrder_[nextIndex];

	//サウンド再生とスティックのクールダウンを設定
	stickInputCooldown_ = kStickInputCooldownTime_;
}

void GameTitleSceneUI::AnimationTitle()
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