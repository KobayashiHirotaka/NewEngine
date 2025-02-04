/**
 * @file BaseCharacter.cpp
 * @brief 各キャラクター(Player,Enemy)の基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "BaseCharacter.h"
#include "Application/Game/Scenes/GamePlayScene/GamePlayScene.h"
#include "Application/Game/GameTimer/GameTimer.h"

void BaseCharacter::Initialize()
{
#ifdef _ADJUSTMENT

	    //デバッグ状態かどうか
		isDebug_ = true;

#endif 
		//isDebug_ = true;

		//Inputのインスタンスを取得
		input_ = Engine::Input::GetInstance();

		//Audioのインスタンスを取得
		audio_ = Engine::Audio::GetInstance();
}

void BaseCharacter::Update()
{
	//シェイク
	effectState_.isShake = false;

	//アニメーションの適応
	model_->ApplyAnimation(animationIndex_);

	model_->Update();

	//リセットのときの処理
	if (isReset_)
	{
		Reset();
	}

	//TODO:StatePatternでやる
	//行動
	if (characterState_.behaviorRequest)
	{
		characterState_.behavior = characterState_.behaviorRequest.value();

		switch (characterState_.behavior)
		{
		case Behavior::kRoot:
		default:
			//移動
			InitializeBehaviorRoot();
			break;

		case Behavior::kAttack:
			//攻撃
			InitializeBehaviorAttack();
			break;

		case Behavior::kJump:
			//ジャンプ
			InitializeBehaviorJump();
			break;

		case Behavior::kStan:
			//スタン
			InitializeBehaviorStan();
			break;
		}

		characterState_.behaviorRequest = std::nullopt;
	}

	switch (characterState_.behavior)
	{
	case Behavior::kRoot:
	default:
		if (GamePlayScene::sRoundStartTimer_ <= 0 && GamePlayScene::sMigrationTimer == kMaxMigrationTime_)
		{
			//移動
			UpdateBehaviorRoot();
		}
		break;

	case Behavior::kAttack:
		//攻撃
		UpdateBehaviorAttack();
		break;

	case Behavior::kJump:
		//ジャンプ
		UpdateBehaviorJump();
		break;

	case Behavior::kStan:
		//スタン
		UpdateBehaviorStan();
		break;
	}

	//画面端の処理
	if (worldTransform_.translation.x >= rightEdge_)
	{
		worldTransform_.translation.x = rightEdge_;
	}

	if (worldTransform_.translation.x <= leftEdge_)
	{
		worldTransform_.translation.x = leftEdge_;
	}

	//端での攻撃時の処理
	if (!attackData_.isAttack && worldTransform_.translation.x >= attackRightEdge_ && characterState_.direction == Direction::Right)
	{
		worldTransform_.translation.x = Lerp(worldTransform_.translation.x, attackRightEdge_, kLerpSpeed_);
	}

	if (!attackData_.isAttack && worldTransform_.translation.x <= attackLeftEdge_ && characterState_.direction == Direction::Left)
	{
		worldTransform_.translation.x = Lerp(worldTransform_.translation.x, attackLeftEdge_, kLerpSpeed_);
	}

	//ジャンプ中にプレイヤーと当たったときの処理
	if (characterState_.behaviorRequest == Behavior::kJump && characterState_.isHitCharacter)
	{
		worldTransform_.translation.y = 0.0f;
	}

	//床についているかどうか
	if (worldTransform_.translation.x <= 0.0f)
	{
		characterState_.isGround = true;
	}
	else
	{
		characterState_.isGround = false;
	}
	
	//コンボ関連の処理
	HitCombo();
	UpdateComboNumberSprite();

	//ダウン時のアニメーション
	DownAnimation();

	//各ゲージの更新処理
	UpdateHPBar();

	UpdateGuardGaugeBar();

	UpdateFinisherGaugeBar();

	//キャラクターと当たっているか
	characterState_.isHitCharacter = false;
}

void BaseCharacter::ImGui()
{
	ImGui::Begin("CharacterDEBUG");
	ImGui::Checkbox("isDebug", &isDebug_);
	ImGui::End();
}

void BaseCharacter::Reset()
{
	//行動
	characterState_.behaviorRequest = Behavior::kRoot;

	//アニメーション用の変数
	attackData_.attackAnimationFrame = 0;
	animationTime_ = 0;

	//時間
	timerData_.downAnimationTimer = timerData_.maxDownAnimationTimer;
	timerData_.guardAnimationTimer = timerData_.maxGuardAnimationTimer;
	timerData_.stanTimer = timerData_.maxStanTimer;
	timerData_.comboTimer = timerData_.maxComboTimer;
	timerData_.finisherTimer = timerData_.maxFinisherTimer;

	//コンボカウント
	comboCount_ = 0;

	//ゲージ
	baseData_.guardGauge_ = 0.0f;

	//キャラクターがガードしているかどうか
	characterState_.isGuard = false;

	//各攻撃をしているかどうか
	attackData_.isAttack = false;
	attackData_.isLightPunch = false;
	attackData_.isMiddlePunch = false;
	attackData_.isHighPunch = false;
	attackData_.isTCMiddlePunch = false;
	attackData_.isTCHighPunch = false;
	attackData_.isTackle = false;
	attackData_.isUppercut = false;
	attackData_.isShot = false;
	attackData_.isFinisher = false;
	attackData_.isFinisherFirstAttack = false;
	attackData_.isFinisherSecondAttack = false;

	//ダメージを受けているかどうか
	attackData_.isDamaged = false;

	//攻撃をガードしているかどうか
	attackData_.isGuarded = false;

	//必殺技ゲージが増えているかどうか
	attackData_.isFinisherGaugeIncreased = false;

	//キャラクターと当たっているかどうか
	characterState_.isHitCharacter = false;

	//各攻撃を受けているかどうか
	characterState_.isHitLightPunch = false;
	characterState_.isHitMiddlePunch = false;
	characterState_.isHitHighPunch = false;
	characterState_.isHitTCMiddlePunch = false;
	characterState_.isHitTCHighPunch = false;
	characterState_.isHitTackle = false;
	characterState_.isHitUppercut = false;
	characterState_.isHitFinisherFirstAttack = false;
	characterState_.isHitFinisherSecondAttack = false;
	characterState_.isHitBullet = false;
	characterState_.isHitAirBullet = false;
	characterState_.isDown = false;

	//判定をリセット
	ResetCollision();

	//リセット
	isReset_ = false;
}

void BaseCharacter::EndDownAnimation(int animationIndex, bool& isHitAttackType)
{
	//Behaviorの設定
	characterState_.behaviorRequest = Behavior::kRoot;

	//アニメーション,演出用変数の設定
	animationIndex_ = animationIndex;
	timerData_.downAnimationTimer = timerData_.maxDownAnimationTimer;
	timerData_.effectTimer = timerData_.maxEffectTimer;
	animationTime_ = 0.0f;
	model_->SetAnimationTime(animationTime_);

	//くらった攻撃
	isHitAttackType = false;

	//ダメージを受けたかどうか
	attackData_.isDamaged = false;

	//必殺技ゲージの増加がされたかどうか
	attackData_.isFinisherGaugeIncreased = false;

	//ダウンしているかどうか
	characterState_.isDown = false;
}

void BaseCharacter::StartAttack(bool& isAttackType)
{   
	//Behaviorの設定
	characterState_.behaviorRequest = Behavior::kAttack;

	//アニメーション用変数の設定
	animationTime_ = 0.0f;
	model_->SetAnimationTime(animationTime_);

	//攻撃した技
	isAttackType = true;
}

void BaseCharacter::EndAttack(bool& isAttackType)
{
	//Behaviorの設定
	characterState_.behaviorRequest = Behavior::kRoot;

	//攻撃しているかどうか
	attackData_.isAttack = false;

	//硬直中かどうか
	attackData_.isRecovery = false;

	//攻撃した技
	isAttackType = false;
	attackType_ = "";

	//アニメーション用変数の設定
	animationTime_ = 0.0f;
	attackData_.attackAnimationFrame = 0;
	model_->SetAnimationTime(animationTime_);
}

void BaseCharacter::EvaluateAttackTiming(int& attackAnimationFrame)
{
	//攻撃
	if (attackAnimationFrame >= attackData_.attackStartTime && attackAnimationFrame <= attackData_.attackEndTime)
	{
		attackData_.isAttack = true;
	}
	else
	{
		attackData_.isAttack = false;
	}

	//硬直
	if (attackAnimationFrame >= attackData_.attackEndTime && attackAnimationFrame <= attackData_.recoveryTime)
	{
		attackData_.isRecovery = true;
	}
	else
	{
		attackData_.isRecovery = false;
	}
}

void BaseCharacter::UpdateAnimationTime(float& animationTime, const bool isLoop, const float frameRate,
	const int animationIndex, Model* model)
{
	//アニメーションの再生
	animationTime += frameRate * GameTimer::GetDeltaTime();

	//ループするか
	if (isLoop)
	{
		animationTime = std::fmod(animationTime, model->GetAnimation()[animationIndex].duration);
	}
	else
	{
		float duration = model->GetAnimation()[animationIndex].duration;

		if (animationTime > duration)
		{
			animationTime = duration;
		}
	}


	model->SetAnimationTime(animationTime);
	model->ApplyAnimation(animationIndex);
}