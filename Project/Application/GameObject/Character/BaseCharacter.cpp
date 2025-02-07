/**
 * @file BaseCharacter.cpp
 * @brief 各キャラクターの基底クラス
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

		//パーティクル
		particleEffectPlayer_ = std::make_unique<ParticleEffectPlayer>();
		particleEffectPlayer_->Initialize();

		//SEの読み込み
		attackSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/Attack.mp3");
		weaponAttackSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/WeaponAttack.mp3");
		damageSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/HitPunch1.mp3");
		guardSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/Guard.mp3");
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


	//床についているかどうか
	if (worldTransform_.translation.x <= 0.0f)
	{
		characterState_.isGround = true;
	}
	else
	{
		characterState_.isGround = false;
	}

	//ParticleEffectPlayerの更新
	particleEffectPlayer_->Update();



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

void BaseCharacter::UpdateDirection(Vector3 character1Position, Vector3 character2Position)
{
	//振り向きの処理
	if (character2Position.x > character1Position.x && !characterState_.isDown)
	{
		characterState_.direction = Direction::Right;
		worldTransform_.rotation.y = characterState_.rightDirectionRotation;
	}

	if (character2Position.x < character1Position.x && !characterState_.isDown)
	{
		characterState_.direction = Direction::Left;
		worldTransform_.rotation.y = characterState_.leftDirectionRotation;
	}

	difference_ = character1Position - character2Position;
	difference_.y = 0.0f;
	distance_ = Length(difference_);

	//後ろに戻れないようにする
	if (distance_ >= kMaxDistance_)
	{
		if (worldTransform_.translation.x < previousPositionX_ && characterState_.direction == Direction::Right)
		{
			worldTransform_.translation.x = character2Position.x - kMaxDistance_;
			moveData_.velocity.x = 0.0f;
		}
		else if (worldTransform_.translation.x > previousPositionX_ && characterState_.direction == Direction::Left)
		{
			worldTransform_.translation.x = character2Position.x + kMaxDistance_;
			moveData_.velocity.x = 0.0f;
		}
	}

	previousPositionX_ = worldTransform_.translation.x;
}

void BaseCharacter::Move(const Vector3 velocity)
{
	//移動
	worldTransform_.translation = Add(worldTransform_.translation, velocity);

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void BaseCharacter::StartAttack(bool& isAttackType)
{   
	//アニメーション用変数の設定
	animationTime_ = 0.0f;
	model_->SetAnimationTime(animationTime_);

	//攻撃した技
	isAttackType = true;
}

void BaseCharacter::EndAttack(bool& isAttackType)
{
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

void BaseCharacter::EndDownAnimation(const int animationIndex)
{
	//アニメーション,演出用変数の設定
	animationIndex_ = animationIndex;
	timerData_.downAnimationTimer = timerData_.maxDownAnimationTimer;
	timerData_.effectTimer = timerData_.maxEffectTimer;
	animationTime_ = 0.0f;
	model_->SetAnimationTime(animationTime_);

	//ダメージを受けたかどうか
	attackData_.isDamaged = false;

	//必殺技ゲージの増加がされたかどうか
	attackData_.isFinisherGaugeIncreased = false;

	//ダウンしているかどうか
	characterState_.isDown = false;
}


Vector3 BaseCharacter::GetWorldPosition()
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld.m[3][0];
	pos.y = worldTransform_.matWorld.m[3][1];
	pos.z = worldTransform_.matWorld.m[3][2];
	return pos;
}