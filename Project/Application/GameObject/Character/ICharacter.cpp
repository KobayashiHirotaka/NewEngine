#include "ICharacter.h"
#include "Application/Game/Scenes/GamePlayScene.h"

void ICharacter::Initialize()
{
#ifdef _ADJUSTMENT

		isDebug_ = true;

#endif 

		//isDebug_ = true;
}

void ICharacter::Update()
{
	effectState_.isShake = false;

	//worldTransform_.scale = { 1.3f,1.3f,1.3f };

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
			BehaviorRootInitialize();
			break;

		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;

		case Behavior::kJump:
			BehaviorJumpInitialize();
			break;

		case Behavior::kStan:
			BehaviorStanInitialize();
			break;
		}

		characterState_.behaviorRequest = std::nullopt;
	}

	switch (characterState_.behavior)
	{
	case Behavior::kRoot:
	default:
		if (GamePlayScene::roundStartTimer_ <= 0 && GamePlayScene::migrationTimer == 200)
		{
			BehaviorRootUpdate();
		}
		break;

	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;

	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;

	case Behavior::kStan:
		BehaviorStanUpdate();
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
		worldTransform_.translation.x = attackRightEdge_;
	}

	if (!attackData_.isAttack && worldTransform_.translation.x <= attackLeftEdge_ && characterState_.direction == Direction::Left)
	{
		worldTransform_.translation.x = attackLeftEdge_;
	}

	//ジャンプ中にプレイヤーと当たったときの処理
	if (characterState_.behaviorRequest == Behavior::kJump && characterState_.isHitCharacter)
	{
		worldTransform_.translation.y = 0.0f;
	}
	
	//ダウン時のアニメーション
	DownAnimation();

	//各ゲージの更新処理
	HPBarUpdate();

	GuardGaugeBarUpdate();

	FinisherGaugeBarUpdate();

	characterState_.isHitCharacter = false;

	ImGui::Begin("CharacterDEBUG");
	ImGui::Checkbox("isDebug", &isDebug_);
	ImGui::End();
}

void ICharacter::Reset()
{
	//行動
	characterState_.behaviorRequest = Behavior::kRoot;

	//アニメーション用の変数
	attackData_.attackAnimationFrame = 0;
	animationTime_ = 0;

	//時間
	timerData_.downAnimationTimer = 60;
	timerData_.guardAnimationTimer = 60;
	timerData_.stanTimer = 60;
	timerData_.comboTimer = 60;
	timerData_.finisherTimer = 120;

	//コンボカウント
	comboCount_ = 0;

	//ゲージ
	guardGauge_ = 0.0f;

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

void ICharacter::UpdateAnimationTime(float animationTime, bool isLoop, float frameRate, 
	int animationIndex, std::unique_ptr<Model>& modelFighterBody)
{
	//TODO:Engine側に移行する
	animationTime = 0.0f;

	animationTime = modelFighterBody->GetAnimationTime();

	animationTime += 1.0f / frameRate;

	if (isLoop)
	{
		animationTime = std::fmod(animationTime, modelFighterBody->GetAnimation()[animationIndex].duration);
	}
	else
	{
		float duration = modelFighterBody->GetAnimation()[animationIndex].duration;

		if (animationTime > duration)
		{
			animationTime = duration; 
		}
	}


	modelFighterBody->SetAnimationTime(animationTime);
	modelFighterBody->ApplyAnimation(animationIndex);
}

void ICharacter::DownAnimationEnd(int animationIndex, bool& isHitAttackType)
{
	//Behaviorの設定
	characterState_.behaviorRequest = Behavior::kRoot;

	//アニメーション用変数の設定
	animationIndex_ = animationIndex;
	timerData_.downAnimationTimer = 60;
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

void ICharacter::AttackStart(bool& isAttackType)
{   
	//Behaviorの設定
	characterState_.behaviorRequest = Behavior::kAttack;

	//アニメーション用変数の設定
	animationTime_ = 0.0f;
	model_->SetAnimationTime(animationTime_);

	//攻撃した技
	isAttackType = true;
}

void ICharacter::AttackEnd(bool& isAttackType)
{
	//Behaviorの設定
	characterState_.behaviorRequest = Behavior::kRoot;

	//攻撃しているかどうか
	attackData_.isAttack = false;

	//硬直中かどうか
	attackData_.isRecovery = false;

	//攻撃した技
	isAttackType = false;

	//アニメーション用変数の設定
	animationTime_ = 0.0f;
	attackData_.attackAnimationFrame = 0;
	model_->SetAnimationTime(animationTime_);
}

void ICharacter::EvaluateAttackTiming()
{
	//攻撃
	if (attackData_.attackAnimationFrame >= attackData_.attackStartTime && attackData_.attackAnimationFrame <= attackData_.attackEndTime)
	{
		attackData_.isAttack = true;
	}
	else
	{
		attackData_.isAttack = false;
	}

	//硬直
	if (attackData_.attackAnimationFrame >= attackData_.attackEndTime && attackData_.attackAnimationFrame <= attackData_.recoveryTime)
	{
		attackData_.isRecovery = true;
	}
	else
	{
		attackData_.isRecovery = false;
	}
}