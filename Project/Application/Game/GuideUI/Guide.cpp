/**
 * @file Guide.cpp
 * @brief 操作説明UIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 2024/01/16
 */

#include "Guide.h"

//スプライトの遷移用のベクター
const std::vector<CommandSpriteType> Guide::sSpriteOrder_ = {
	CommandSpriteType::GeneralCommandSprite,
	CommandSpriteType::ComboAttackCommandSprite,
	CommandSpriteType::FinisherAttackCommandSprite
};

void Guide::Initialize()
{
	//Inputのインスタンスの取得
	input_ = Engine::Input::GetInstance();

	//Audioのインスタンスの取得
	audio_ = Engine::Audio::GetInstance();

	//基本操作説明用のSprite
	generalCommandListTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/PlayGeneralGuide.png");
	generalCommandListSprite_.reset(Sprite::Create(generalCommandListTextureHandle_, { 0.0f,0.0f }));

	//攻撃操作説明用のSprite
	attackCommandListTextureHandle_[0] = Engine::TextureManager::LoadTexture("Resource/Images/PlayDefaultAttackGuide.png");
	attackCommandListSprite_[0].reset(Sprite::Create(attackCommandListTextureHandle_[0], { 0.0f,0.0f }));

	attackCommandListTextureHandle_[1] = Engine::TextureManager::LoadTexture("Resource/Images/PlayFinisherAttackGuide.png");
	attackCommandListSprite_[1].reset(Sprite::Create(attackCommandListTextureHandle_[1], { 0.0f,0.0f }));
}

void Guide::Update()
{
	UpdateCommandSprite();
}

void Guide::Draw()
{
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

void Guide::UpdateCommandSprite()
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

void Guide::ChangeCommandSprite()
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

void Guide::ApplyCommandSprite(int changeAmount)
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
