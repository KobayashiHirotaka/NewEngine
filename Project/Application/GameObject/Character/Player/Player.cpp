/**
 * @file Player.cpp
 * @brief プレイヤーの管理(移動、攻撃など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Player.h"
#include "Application/GameObject/Character/Enemy/Enemy.h"
#include "Application/Game/Scenes/GamePlayScene/GamePlayScene.h"
#include "Application/Game/GameTimer/GameTimer.h"

Player::~Player()
{
	
}

void Player::Initialize()
{
	//タグの設定
	IGame3dObject::SetTag("Player");

	//初期化
	BaseCharacter::Initialize();

	//WorldTransformの初期化
	worldTransform_.Initialize();

	//当たり判定の設定
	collider_ = std::make_unique<Collider>();
	collider_->SetAABB(aabb_);

	collider_->SetCollisionAttribute(kCollisionAttributePlayer);
	collider_->SetCollisionMask(kCollisionMaskPlayer);
	collider_->SetCollisionPrimitive(kCollisionPrimitiveAABB);

	collider_->SetGameObject(this);

	//LineBoxの描画
	lineBox_.reset(LineBox::Create(aabb_));

	//Stateの生成、初期化
	currentState_ = std::make_unique<PlayerIdleState>();
	currentState_->SetPlayer(this);
	currentState_->Initialize();

	//リソース
	//体力ゲージ
	const Vector2 kHpBarPosition = { 537.0f, kBarSpace_ };
	const Vector2 kHpBarSize = { -barSize_  ,7.2f };

	hpBar_ = {
		true,
		Engine::TextureManager::LoadTexture("Resource/Images/HP.png"),
		kHpBarPosition,
		0.0f,
		kHpBarSize,
		nullptr,
	};

	hpBar_.sprite_.reset(Sprite::Create(hpBar_.textureHandle_, hpBar_.position_));

	//ガードゲージ
	const Vector2 kGuardGaugeBarPosition = { 537.0f, kGuardGaugeBarSpace_ };
	const Vector2 kGuardGaugeBarSize = { -guardGaugeBarSize_  ,7.0f };

	guardGaugeBar_ = {
		true,
		Engine::TextureManager::LoadTexture("Resource/Images/GuardGauge.png"),
		{kGuardGaugeBarPosition},
		0.0f,
		{kGuardGaugeBarSize},
		nullptr,
	};

	guardGaugeBar_.sprite_.reset(Sprite::Create(guardGaugeBar_.textureHandle_, guardGaugeBar_.position_));

	//必殺技ゲージ
	const Vector2 kFinisherGaugeBarPosition = { 299.0f, kFinisherGaugeBarSpace_ };
	const Vector2 kFinisherGaugeBarSize = { -finisherGaugeBarSize_  ,19.3f };

	finisherGaugeBar_ = {
		true,
		Engine::TextureManager::LoadTexture("Resource/Images/FinisherGauge.png"),
		{kFinisherGaugeBarPosition},
		0.0f,
		{kFinisherGaugeBarSize},
		nullptr,
	};

	finisherGaugeBar_.sprite_.reset(Sprite::Create(finisherGaugeBar_.textureHandle_, finisherGaugeBar_.position_));

	//ヒット表示
	const Vector2 kHitSpritePosition = { 1090.0f, 180.0f };

	hitTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/Hit.png");
	hitSprite_.reset(Sprite::Create(hitTextureHandle_, kHitSpritePosition));

	//コンボ表示
	const Vector2 kComboNumSpritePosition = { 1060.0f, 290.0f };

	comboNumTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Number/0.png");
	comboNumSprite_.reset(Sprite::Create(comboNumTextureHandle_, kComboNumSpritePosition));

	//キャラクターアイコン
	const Vector2 kPlayerIconPosition = { 53.0f, 20.0f };
	const Vector2 kPlayerIconSize = { 120.0f,120.0f };

	playerIconTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/PlayerIcon.png");

	playerIconSprite_.reset(Sprite::Create(playerIconTextureHandle_, kPlayerIconPosition));
	playerIconSprite_->SetSize(kPlayerIconSize);

	//カーソル
	const float kAdjustCursolPositionY = 0.4f;
	const float kCursolRotationY = 1.5f;
	const Vector3 kCursolScale = { 0.3f, 0.3f, 0.3f };

	playerCursol_.reset(Model::CreateFromOBJ("Resource/PlayerCursol", "PlayerCursol.obj"));

	worldTransformCursol_.Initialize();
	worldTransformCursol_.translation = { worldTransform_.translation.x, worldTransform_.translation.y + kAdjustCursolPositionY, worldTransform_.translation.z };
	worldTransformCursol_.rotation.y = kCursolRotationY;
	worldTransformCursol_.scale = kCursolScale;

	worldTransformCursol_.parent = &worldTransform_;

	//弾のモデルを生成
	bulletModel_.reset(Model::CreateFromOBJ("Resource/Bullet", "Bullet.obj"));

	//パーティクル
	particleEffectPlayer_ = std::make_unique<ParticleEffectPlayer>();
	particleEffectPlayer_->Initialize();

	//SEの読み込み
	attackSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/Attack.mp3");
	weaponAttackSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/WeaponAttack.mp3");
	damageSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/HitPunch1.mp3");
	guardSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/Guard.mp3");

	//基本データの設定
	baseData_.hp_ = -baseData_.kMaxHp_;
	moveData_.frontSpeed_ = kMaxFrontSpeed_;
	moveData_.backSpeed_ = kMaxBackSpeed_;

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Player::Update()
{
#ifdef _ADJUSTMENT

	//デバッグ用の処理
	const float kMaxFinisherGauge = -50.0f;
	if (input_->PushKey(DIK_L))
	{
		baseData_.finisherGauge_ = kMaxFinisherGauge;
	}

	if (isDebug_)
	{
		Vector4 kAttackColor = { 1.0f,0.0f,0.0f,1.0f };
		Vector4 kRecoveryColor = { 0.0f,0.0f,1.0f,1.0f };
		Vector4 kDefaultColor = { 1.0f,1.0f,1.0f,1.0f };

		if (attackData_.isAttack)
		{
			//攻撃中(攻撃判定あり)にモデルの色を変える
			model_->GetMaterial()->SetColor(kAttackColor);
		}
		else if (attackData_.isRecovery)
		{
			//硬直中にモデルの色を変える
			model_->GetMaterial()->SetColor(kRecoveryColor);
		}
		else
		{
			model_->GetMaterial()->SetColor(kDefaultColor);
		}
	}

#endif

	//更新
	BaseCharacter::Update();

	//エディターで設定したパラメータをセット
	AttackEditor::GetInstance()->SetAttackParameters(attackType_, attackData_.attackStartTime, attackData_.attackEndTime, attackData_.recoveryTime,
		attackData_.cancelStartTime, attackData_.cancelEndTime,attackData_.damage, attackData_.hitRecoveryTime, attackData_.guardGaugeIncreaseAmount,
		attackData_.finisherGaugeIncreaseAmount, attackData_.hitStop, aabb_, true, characterState_.direction);

	//Stateの更新
	currentState_->Update();

	//振り向きの処理
	Vector3 playerWorldPosition = worldTransform_.translation;
	Vector3 enemyWorldPosition = enemy_->GetWorldPosition();

	if (enemyWorldPosition.x > playerWorldPosition.x && characterState_.behavior != Behavior::kJump
		&& characterState_.behavior != Behavior::kAttack && !characterState_.isDown)
	{
		characterState_.direction = Direction::Right;
		worldTransform_.rotation.y = characterState_.rightDirectionRotation;
		isDirectionRight_ = true;
	}

	if (enemyWorldPosition.x < playerWorldPosition.x && characterState_.behavior != Behavior::kJump
		&& characterState_.behavior != Behavior::kAttack && !characterState_.isDown)
	{
		characterState_.direction = Direction::Left;
		worldTransform_.rotation.y = characterState_.leftDirectionRotation;
		isDirectionRight_ = false;
	}

	difference_ = playerWorldPosition - enemyWorldPosition;
	difference_.y = 0.0f;
	distance_ = Length(difference_);

	//後ろに戻れないようにする
	if (distance_ >= kMaxDistance_)
	{
		if (worldTransform_.translation.x < previousPositionX_ && characterState_.direction == Direction::Right)
		{
			worldTransform_.translation.x = enemyWorldPosition.x - kMaxDistance_;
			moveData_.velocity.x = 0.0f;
		}
		else if (worldTransform_.translation.x > previousPositionX_ && characterState_.direction == Direction::Left)
		{
			worldTransform_.translation.x = enemyWorldPosition.x + kMaxDistance_;
			moveData_.velocity.x = 0.0f;
		}
	}

	previousPositionX_ = worldTransform_.translation.x;

	//コンボ関連の処理
	HitCombo();
	UpdateComboNumberSprite();

	//ガードアニメーションタイマーのリセット
	if (!enemy_->GetIsAttack())
	{
		timerData_.guardAnimationTimer = timerData_.maxGuardAnimationTimer;
	}

	//弾の更新
	UpdateBullets();

	//LineBoxの更新
	lineBox_->Update(aabb_);

	//ParticleEffectPlayerの更新
	particleEffectPlayer_->Update();

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
	worldTransformCursol_.UpdateMatrixEuler();

	//当たり判定の更新
	collider_->Update();
}

void Player::Draw(const Camera& camera)
{
	//モデル描画
	model_->Draw(worldTransform_, camera, animationIndex_);

	//カーソル描画
	if (!characterState_.isDown && timerData_.finisherTimer == timerData_.maxFinisherTimer)
	{
		playerCursol_->Draw(worldTransformCursol_, camera, 0);
	}
}

void Player::DrawBone(const Camera& camera)
{
	//骨の描画
	model_->DrawBone(worldTransform_, camera);
}

void Player::DrawCollision(const Camera& camera)
{
	//LineBoxの描画
	lineBox_->Draw(worldTransform_, camera);
}

void Player::DrawSprite()
{
	//体力ゲージの描画
	if (baseData_.hp_ <= 0)
	{
		hpBar_.sprite_->Draw();
	}

	//ガードゲージの描画
	guardGaugeBar_.sprite_->Draw();

	//必殺技ゲージの描画
	finisherGaugeBar_.sprite_->Draw();

	//プレイヤーアイコンの描画
	playerIconSprite_->Draw();

	//コンボ表示の描画
	if (comboCount_ >= kComboCount_[2])
	{
		hitSprite_->Draw();
		comboNumSprite_->Draw();
	}
}

void Player::DrawParticle(const Camera& camera)
{
	//パーティクルの描画
	particleEffectPlayer_->Draw(camera);

	for (auto& bullet : bullets_)
	{
		bullet->DrawParticle(camera);
	}
}

void Player::ImGui()
{
	
}

void Player::InitializeBehaviorRoot()
{
	//アニメーション
	const int kAnimationIdle = 5;
	animationIndex_ = kAnimationIdle;
}

void Player::UpdateBehaviorRoot()
{
	
}

void Player::Move(const Vector3 velocity)
{
	//移動
	worldTransform_.translation = Add(worldTransform_.translation, velocity);

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Player::ChangeState(PlayerBaseState* state)
{
	currentState_.reset(state);
}

void Player::InitializeBehaviorAttack()
{

}

void Player::UpdateBehaviorAttack()
{
	
}

void Player::InitializeBehaviorJump()
{
	
}

void Player::UpdateBehaviorJump()
{
	
}

void Player::InitializeBehaviorStan()
{

}

void Player::UpdateBehaviorStan()
{
	
}

void Player::OnCollision(Collider*)
{

}

void Player::ShootBullet(const Vector3&, const Vector3&)
{

}

void Player::UpdateBullets()
{

}

void Player::StartAttack(bool& isAttackType)
{
	//攻撃の開始処理
	BaseCharacter::StartAttack(isAttackType);
}

void Player::EndAttack(bool& isAttackType)
{
	//攻撃の終了処理
	enemy_->SetIsGuarded(false);
	BaseCharacter::EndAttack(isAttackType);
}

void Player::EvaluateAttackTiming()
{
	//攻撃判定をつけるタイミングの設定
	BaseCharacter::EvaluateAttackTiming();
}

void Player::ApplyDamage()
{

}

void Player::ResetCollision()
{

}

void Player::UpdateHPBar()
{

}

void Player::UpdateGuardGaugeBar()
{

}

void Player::AdjustGuardGauge()
{

}

void Player::UpdateFinisherGaugeBar()
{

}

void Player::AdjustFinisherGauge(float)
{
	
}

void Player::Reset()
{
	
}

void Player::DownAnimation()
{

}

void Player::EndDownAnimation(int animationIndex, bool& isHitAttackType)
{
	//ダウンアニメーションの終了処理
	BaseCharacter::EndDownAnimation(animationIndex, isHitAttackType);
}

void Player::UpdateComboNumberSprite()
{
	
}

void Player::HitCombo()
{
	
}

void Player::ComboCountUpdate(const int kRecoveryTime)
{
	comboCount_++;
	timerData_.comboTimer = kRecoveryTime;
}

Vector3 Player::GetWorldPosition()
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld.m[3][0];
	pos.y = worldTransform_.matWorld.m[3][1];
	pos.z = worldTransform_.matWorld.m[3][2];
	return pos;
}

Vector3 Player::GetRightHandJointWorldPosition()
{
	WorldTransform handJointWorldTransform = model_->GetJointWorldTransform("mixamorig:RightHandIndex1");

	handJointWorldTransform.matWorld = Multiply(handJointWorldTransform.matWorld, worldTransform_.matWorld);

	return Vector3 { handJointWorldTransform.matWorld.m[3][0], handJointWorldTransform.matWorld.m[3][1], handJointWorldTransform.matWorld.m[3][2] };
}
