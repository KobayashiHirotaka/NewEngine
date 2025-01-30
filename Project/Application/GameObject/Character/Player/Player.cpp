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
	//コントローラーの取得
	if (input_->GetJoystickState())
	{
		if (!characterState_.isDown && comboCount_ == 0)
		{
			//移動
			Move();
		}

		if (!characterState_.isDown)
		{
			//ジャンプ
			if ((input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) || input_->GetLeftStickY() > kStickDeadZone_) && worldTransform_.translation.y == 0.0f)
			{
				characterState_.behaviorRequest = Behavior::kJump;
			}

			//攻撃
		    //弱攻撃
			if ((input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) || input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y)))
			{
				attackType_ = "弱攻撃";
				StartAttack(attackData_.isLightPunch);
			}
			//弾攻撃
			else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
			{
				attackType_ = "ショット";
				StartAttack(attackData_.isShot);
			}
			//特殊技
			else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
			{
				//アッパー攻撃
				if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && 
					input_->GetLeftStickX() < kStickDeadZone_ && input_->GetLeftStickX() > -kStickDeadZone_)
				{
					attackType_ = "アッパー";
					StartAttack(attackData_.isUppercut);
				}
				//タックル攻撃
				else
				{
					attackType_ = "タックル";
					StartAttack(attackData_.isTackle);
				}
			}
			//必殺技
			else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_LEFT_SHOULDER) && isFinisherCharge_)
			{
				attackType_ = "必殺技";
				StartAttack(attackData_.isFinisher);
				isFinisherInvincible_ = true;
				baseData_.finisherGauge_ = 0.0f;
				timerData_.finisherTimer = timerData_.maxFinisherTimer;
				attackData_.isFinisherFirstAttack = false;
			}
		}
	}
}

void Player::InitializeBehaviorAttack()
{
	attackData_.attackAnimationFrame = 0;
}

void Player::UpdateBehaviorAttack()
{
	//TODO:関数化して同じ処理をまとめる
	//弱攻撃
	if (attackData_.isLightPunch)
	{
		//アニメーション
		const int kAnimationLighhtPunch = 13;
		const float animationSpeed = 1.5f;

		animationIndex_ = kAnimationLighhtPunch;
		characterState_.isGuard = false;
	
		if (!characterState_.isDown)
		{
			UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);
		}

		//当たり判定を設定
		if (characterState_.direction == Direction::Right)
		{
			collider_->SetAABB(aabb_);
		}
		else if (characterState_.direction == Direction::Left)
		{
			collider_->SetAABB(aabb_);
		}

		//攻撃判定をつけるタイミングの設定
		EvaluateAttackTiming();

		//終了処理
		if (characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime)
		{
			EndAttack(attackData_.isLightPunch);
			ResetCollision();
		}

		//キャンセルの処理
		if (input_->GetJoystickState())
		{
			if (!characterState_.isDown && attackData_.attackAnimationFrame >= attackData_.cancelStartTime && attackData_.attackAnimationFrame < attackData_.cancelEndTime &&
				(input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) || input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y))  && enemy_->GetIsDown())
			{
				attackType_ = "中攻撃(ターゲット)";
				attackData_.isAttack = false;
				attackData_.isLightPunch = false;
				attackData_.isTCMiddlePunch = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}
		}

		attackData_.attackAnimationFrame += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
	}

	//TC用の攻撃(2発目)
	if (attackData_.isTCMiddlePunch)
	{
		//アニメーション
		const int kAnimationTCMiddlePunch = 12;
		const float animationSpeed = 1.5f;

		animationIndex_ = kAnimationTCMiddlePunch;
		characterState_.isGuard = false;

		if (!characterState_.isDown)
		{
			UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);
		}

		attackData_.isAttack = true;

		//移動用のパラメータ
		const int kMoveTime = 5;
		const float kMoveSpeed = 0.03f;

		//当たり判定を設定
		if (characterState_.direction == Direction::Right)
		{
			collider_->SetAABB(aabb_);

			//コンボがつながりやすくなるように移動する
			if (attackData_.attackAnimationFrame < kMoveTime)
			{
				worldTransform_.translation.x += kMoveSpeed;
			}
		}
		else if (characterState_.direction == Direction::Left)
		{
			collider_->SetAABB(aabb_);

			//コンボがつながりやすくなるように移動する
			if (attackData_.attackAnimationFrame < kMoveTime)
			{
				worldTransform_.translation.x -= kMoveSpeed;
			}
		}

		//攻撃判定をつけるタイミングの設定
		EvaluateAttackTiming();

		//終了処理
		if (characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime)
		{
			EndAttack(attackData_.isTCMiddlePunch);
			ResetCollision();
		}

		//キャンセルの処理
		if (input_->GetJoystickState())
		{
			//弱コンボ
			if (!characterState_.isDown && attackData_.attackAnimationFrame > attackData_.cancelStartTime && attackData_.attackAnimationFrame < attackData_.cancelEndTime
				&& input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) && characterState_.isHitCharacter)
			{
				attackType_ = "強攻撃";
				attackData_.isAttack = false;
				attackData_.isTCMiddlePunch = false;
				attackData_.isHighPunch = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}

			//強コンボ
			if (!characterState_.isDown && attackData_.attackAnimationFrame > attackData_.cancelStartTime && attackData_.attackAnimationFrame < attackData_.cancelEndTime
				&& input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y) && characterState_.isHitCharacter)
			{
				attackType_ = "アッパー";
				attackData_.isAttack = false;
				attackData_.isTCMiddlePunch = false;
				attackData_.isUppercut = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}
		}

		attackData_.attackAnimationFrame += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
	}

	//強攻撃
	if (attackData_.isHighPunch)
	{
		//アニメーション
		const int kAnimationHighPunch = 3;
		const float animationSpeed = 1.8f;

		animationIndex_ = kAnimationHighPunch;
		characterState_.isGuard = false;

		if (!characterState_.isDown)
		{
			UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);
		}

		//移動用のパラメータ
		const int kMoveTime = 15;
		const float kMoveSpeed = 0.05f;
		
		//当たり判定を設定
		if (characterState_.direction == Direction::Right)
		{
			collider_->SetAABB(aabb_);

			//コンボがつながりやすくなるように移動する
			if (characterState_.isHitCharacter && attackData_.attackAnimationFrame <= kMoveTime)
			{
				worldTransform_.translation.x -= kMoveSpeed;
			}
		}
		else if (characterState_.direction == Direction::Left)
		{
			collider_->SetAABB(aabb_);

			//コンボがつながりやすくなるように移動する
			if (characterState_.isHitCharacter && attackData_.attackAnimationFrame <= kMoveTime)
			{
				worldTransform_.translation.x += kMoveSpeed;
			}
		}

		//攻撃判定をつけるタイミングの設定
		EvaluateAttackTiming();

		//終了処理
		if (characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime)
		{
			EndAttack(attackData_.isHighPunch);
			ResetCollision();
		}

		//キャンセルの処理
		if (input_->GetJoystickState())
		{
			//タックル攻撃
			if (!characterState_.isDown && attackData_.attackAnimationFrame > attackData_.cancelStartTime && attackData_.attackAnimationFrame < attackData_.cancelEndTime &&
				input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
			{
				attackType_ = "タックル";
				attackData_.isAttack = false;
				attackData_.isHighPunch = false;
				attackData_.isTackle = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}
		}

		attackData_.attackAnimationFrame += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
	}

	//弾攻撃
	if (attackData_.isShot)
	{
		//アニメーション
		const int kAnimationShot = 19;
		const float animationSpeed = 1.2f;

		animationIndex_ = kAnimationShot;
		characterState_.isGuard = false;

		if (!characterState_.isDown)
		{
			UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);
		}

		//まだ弾を発射していない場合
		const Vector2 kRespownPos = { 0.2f,0.5f };
		if (!hasShot_)
		{
			if (characterState_.direction == Direction::Right)
			{
				//弾の発射位置を敵の位置に設定
				Vector3 bulletStartPosition = { worldTransform_.translation.x + kRespownPos.x, worldTransform_.translation.y + kRespownPos.y, worldTransform_.translation.z };
				Vector3 bulletVelocity = { 0.1f, 0.0f, 0.0f };

				ShootBullet(bulletStartPosition, bulletVelocity);
			}
			else if (characterState_.direction == Direction::Left)
			{
				//弾の発射位置を敵の位置に設定
				Vector3 bulletStartPosition = { worldTransform_.translation.x - kRespownPos.x, worldTransform_.translation.y + kRespownPos.y, worldTransform_.translation.z };
				Vector3 bulletVelocity = { -0.1f, 0.0f, 0.0f };

				ShootBullet(bulletStartPosition, bulletVelocity);
			}

			//弾を発射したことを記録
			hasShot_ = true;
		}

		//終了処理
		const int kShotEndTimer = 50;
		if (characterState_.isDown || attackData_.attackAnimationFrame >= kShotEndTimer)
		{
			EndAttack(attackData_.isShot);
			hasShot_ = false;
			ResetCollision();
		}

		attackData_.attackAnimationFrame += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
	}

	//タックル攻撃
	if (attackData_.isTackle)
	{
		//アニメーション
		const int kAnimationTackle = 9;
		const float animationSpeed = 1.8f;

		animationIndex_ = kAnimationTackle;
		characterState_.isGuard = false;

		if (!characterState_.isDown)
		{
			UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);
		}

		//パーティクル用のパラメータ
		const int kParticleTime = 60;
		const float kParticleMoveSpeed = 0.3f;
		float particlePositionX = 0.1f;
		float particlePositionY = 0.6f;

		//移動用のパラメータ
		const int kMoveTime = 40;
		const float kMoveSpeed = 9.0f;

		//攻撃判定をつけるタイミングの設定
		EvaluateAttackTiming();

		if (characterState_.direction == Direction::Right)
		{
			if (attackData_.attackAnimationFrame >= attackData_.attackStartTime && attackData_.attackAnimationFrame < kMoveTime)
			{
				//当たり判定を設定
				collider_->SetAABB(aabb_);

				//移動
				worldTransform_.translation.x += kMoveSpeed * GameTimer::GetDeltaTime();
			}

			//パーティクルの移動
			if (attackData_.attackAnimationFrame >= attackData_.attackStartTime && attackData_.attackAnimationFrame < kParticleTime)
			{
				particlePositionX += kParticleMoveSpeed;

				particleEffectPlayer_->PlayParticle("PlayerRightNackle", { worldTransform_.translation.x + particlePositionX,
					worldTransform_.translation.y + particlePositionY,worldTransform_.translation.z });
			}
			else
			{
				//硬直中の当たり判定を設定
				const AABB recoveryCollsion = { {-0.1f,0.0f,-0.3f},{0.2f,1.0f,0.3f} };
				aabb_ = recoveryCollsion;
				collider_->SetAABB(aabb_);
			}
		}
		else if (characterState_.direction == Direction::Left)
		{
			if (attackData_.attackAnimationFrame >= attackData_.attackStartTime && attackData_.attackAnimationFrame < kMoveTime)
			{
				//当たり判定を設定
				collider_->SetAABB(aabb_);

				//移動
				worldTransform_.translation.x -= kMoveSpeed * GameTimer::GetDeltaTime();
			}

			//パーティクルの移動
			if (attackData_.attackAnimationFrame >= attackData_.attackStartTime && attackData_.attackAnimationFrame < kParticleTime)
			{
				particlePositionX += kParticleMoveSpeed;

				particleEffectPlayer_->PlayParticle("PlayerLeftNackle", { worldTransform_.translation.x - particlePositionX,
					worldTransform_.translation.y + particlePositionY,worldTransform_.translation.z });
			}
			else
			{
				//硬直中の当たり判定を設定
				const AABB recoveryCollsion = { {-0.2f,0.0f,-0.3f},{0.1f,1.0f,0.3f} };
				aabb_ = recoveryCollsion;
				collider_->SetAABB(aabb_);
			}
		}

		//終了処理
		if (characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime)
		{
			EndAttack(attackData_.isTackle);
			ResetCollision();
		}

		attackData_.attackAnimationFrame += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
	}

	//アッパー攻撃
	if (attackData_.isUppercut)
	{
		//アニメーション
		const int kAnimationUpperCut = 14;
		const float animationSpeed = 1.5f;

		animationIndex_ = kAnimationUpperCut;
		characterState_.isGuard = false;

		if (!characterState_.isDown)
		{
			UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);
		}

		//移動用のパラメータ
		const int kMoveTime = 5;
		const float kMoveSpeed = 0.03f;

		//当たり判定を設定
		if (characterState_.direction == Direction::Right)
		{
			collider_->SetAABB(aabb_);

			//コンボがつながりやすくなるように移動する
			if (attackData_.attackAnimationFrame < kMoveTime)
			{
				worldTransform_.translation.x += kMoveSpeed;
			}
		}
		else if (characterState_.direction == Direction::Left)
		{
			collider_->SetAABB(aabb_);

			//コンボがつながりやすくなるように移動する
			if (attackData_.attackAnimationFrame < kMoveTime)
			{
				worldTransform_.translation.x -= kMoveSpeed;
			}
		}

		//攻撃判定をつけるタイミングの設定
		EvaluateAttackTiming();

		//終了処理
		if (characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime)
		{
			EndAttack(attackData_.isUppercut);
			ResetCollision();
		}

		//キャンセルの処理
		if (input_->GetJoystickState())
		{
			//強コンボ
			if (!characterState_.isDown && attackData_.attackAnimationFrame > attackData_.cancelStartTime && attackData_.attackAnimationFrame < attackData_.cancelEndTime
				&& input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y) && enemy_->GetComboCount() >= kComboCount_[3] && isFinisherCharge_ &&
				characterState_.isHitCharacter)
			{
				attackData_.isAttack = false;
				attackData_.isUppercut = false;
				attackData_.isFinisher = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
				baseData_.finisherGauge_ = 0.0f;
			}
		}

		attackData_.attackAnimationFrame += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
	}

	//超必
	if (attackData_.isFinisher)
	{
		//アニメーション
		const int kAnimationFinisherRight = 15;
		const int kAnimationFinisherLeft = 18;
		const int kAnimationFinisherFirstAttack = 16;
		const int kAnimationFinisherSecondAttack = 17;
		const float animationSpeed = 1.5f;

		characterState_.isGuard = false;

		//移動用のパラメータ
		const float kMoveSpeed = 0.01f;

		//必殺技の時間
		const int kFinisherStartTime = 40;
		const int kFinisherEndTime = 80;

		//必殺技演出
		if (timerData_.finisherTimer > kFinisherStartTime && attackData_.attackAnimationFrame < kFinisherEndTime && !attackData_.isFinisherFirstAttack && !attackData_.isFinisherSecondAttack)
		{
			isFinisherEffect_ = true;
			timerData_.finisherTimer--;
			UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);
		}
		else
		{
			isFinisherEffect_ = false;
		}

		if (isFinisherEffect_)
		{
			if (isDirectionRight_)
			{
				animationIndex_ = kAnimationFinisherRight;
			}
			else 
			{
				animationIndex_ = kAnimationFinisherLeft;
			}
		}
		//演出が終わったら必殺技(初段)に移る
		else if(!attackData_.isFinisherFirstAttack && !attackData_.isFinisherSecondAttack)
		{
			attackType_ = "必殺技(初段)";
			timerData_.finisherTimer = timerData_.maxFinisherTimer;
			attackData_.isFinisherFirstAttack = true;
			animationTime_ = 0.0f;
			attackData_.attackAnimationFrame = 0;
			model_->SetAnimationTime(animationTime_);
		}

		//必殺技(初段)
		if (attackData_.isFinisherFirstAttack)
		{
			//アニメーション
			animationIndex_ = kAnimationFinisherFirstAttack;
			UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

			//当たり判定を設定
			if (characterState_.direction == Direction::Right)
			{
				collider_->SetAABB(aabb_);
			}
			else if (characterState_.direction == Direction::Left)
			{
				collider_->SetAABB(aabb_);
			}

			//攻撃判定をつけるタイミングの設定
			EvaluateAttackTiming();

			//2段目への移行処理
			if (enemy_->GetIsDown() && attackData_.attackAnimationFrame > attackData_.cancelStartTime && attackData_.attackAnimationFrame < attackData_.cancelEndTime)
			{
				attackType_ = "必殺技(2段目)";
				timerData_.finisherTimer = timerData_.maxFinisherTimer;
				attackData_.isAttack = false;
				attackData_.isFinisherFirstAttack = false;
				isFinisherInvincible_ = false;
				attackData_.isFinisherSecondAttack = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}

			//終了処理
			if (characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime)
			{
				timerData_.finisherTimer = timerData_.maxFinisherTimer;
				EndAttack(attackData_.isFinisher);
				ResetCollision();
				attackData_.isFinisherFirstAttack = false;
				isFinisherInvincible_ = false;
			}
		}

		//必殺技(2段目)
		if (attackData_.isFinisherSecondAttack && !characterState_.isDown)
		{
			//アニメーション
			animationIndex_ = kAnimationFinisherSecondAttack;
			UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

			//当たり判定を設定
			if (characterState_.direction == Direction::Right)
			{
				collider_->SetAABB(aabb_);

				//コンボがつながりやすくなるように移動
				if (!characterState_.isHitCharacter)
				{
					worldTransform_.translation.x += kMoveSpeed;
				}
			}
			else if (characterState_.direction == Direction::Left)
			{
				collider_->SetAABB(aabb_);

				//コンボがつながりやすくなるように移動
				if (!characterState_.isHitCharacter)
				{
					worldTransform_.translation.x -= kMoveSpeed;
				}
			}

			//攻撃判定をつけるタイミングの設定
			EvaluateAttackTiming();

			//終了処理
			if (attackData_.attackAnimationFrame > attackData_.recoveryTime)
			{
				attackType_ = "タックル";
				attackData_.isAttack = false;
				attackData_.isFinisher = false;
				attackData_.isFinisherSecondAttack = false;
				attackData_.isTackle = true;
				animationTime_ = 0.0f;
				attackData_.attackAnimationFrame = 0;
				model_->SetAnimationTime(animationTime_);
				ResetCollision();
			}
		}

		//相打ちの場合
		if (characterState_.isDown)
		{
			timerData_.finisherTimer = timerData_.maxFinisherTimer;
			EndAttack(attackData_.isFinisher);
			ResetCollision();
			attackData_.isFinisherFirstAttack = false;
			attackData_.isFinisherSecondAttack = false;
			isFinisherInvincible_ = false;
		}

		attackData_.attackAnimationFrame += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
	}
}

void Player::InitializeBehaviorJump()
{
	//ジャンプの速さ
	const float kJumpFirstSpeed_ = 18.0f;

	moveData_.velocity.y = kJumpFirstSpeed_;

	if (input_->GetJoystickState())
	{
		//スティック用のパラメータ
		float joystickInput = input_->GetLeftStickX();

		const float kStickThreshold = 0.3f;
		const float kMoveSpeedX = 3.0f;  

		//入力に応じたジャンプの初期設定
		if (fabs(joystickInput) > kStickThreshold)
		{
			moveData_.velocity.x = (joystickInput > 0 ? kMoveSpeedX : -kMoveSpeedX);
		}
		else
		{
			if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT))
			{
				moveData_.velocity.x = kMoveSpeedX;
			}
			else if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT))
			{
				moveData_.velocity.x = -kMoveSpeedX;
			}
		}
	}
}

void Player::UpdateBehaviorJump()
{
	//離れているとき
	if (distance_ >= kMaxDistance_)
	{
		moveData_.velocity.x = 0.0f;
	}

	//ジャンプ
	Vector3 deltaVelocity = { moveData_.velocity.x * GameTimer::GetDeltaTime(),
		moveData_.velocity.y * GameTimer::GetDeltaTime(),
		moveData_.velocity.z * GameTimer::GetDeltaTime() };

	worldTransform_.translation = Add(worldTransform_.translation, deltaVelocity);

	const float kGravityAcceleration_ = 80.0f;

	Vector3 accelerationVector_ = { 0.0f,-kGravityAcceleration_ * GameTimer::GetDeltaTime(),0.0f };

	moveData_.velocity = Add(moveData_.velocity, accelerationVector_);

	//ジャンプ攻撃の入力
	const float kPlayerPositionY = 0.3f;

	if (input_->GetJoystickState())
	{
		if (worldTransform_.translation.y > kPlayerPositionY && !attackData_.isJumpAttack && (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X) || input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y)
			|| input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) || input_->IsPressButtonEnter(XINPUT_GAMEPAD_A)))
		{
			attackType_ = "ジャンプ攻撃";
			attackData_.isAttack = false;
			attackData_.isJumpAttack = true;
			animationTime_ = 0.0f;
			attackData_.attackAnimationFrame = 0;
			model_->SetAnimationTime(animationTime_);
			ResetCollision();
		}
	}

	//ジャンプ攻撃
	if (attackData_.isJumpAttack)
	{
		//アニメーション
		const int kAnimationJumpAttack = 6;
		const float animationSpeed = 1.5f;

		animationIndex_ = kAnimationJumpAttack;
		characterState_.isGuard = false;

		if (!characterState_.isDown)
		{
			UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, model_);
		}

		//当たり判定を設定
		if (characterState_.direction == Direction::Right)
		{
			collider_->SetAABB(aabb_);
		}
		else if (characterState_.direction == Direction::Left)
		{
			collider_->SetAABB(aabb_);
		}

		//攻撃判定をつけるタイミングの設定
		EvaluateAttackTiming();

		//終了処理
		if (worldTransform_.translation.y <= 0.0f && attackData_.attackAnimationFrame > attackData_.recoveryTime)
		{
			EndAttack(attackData_.isJumpAttack);
			ResetCollision();
		}

		if (worldTransform_.translation.y <= 0.0f || characterState_.isDown)
		{
			worldTransform_.translation.y = 0.0f;
			moveData_.velocity = { 0.0f,0.0f,0.0f };
			EndAttack(attackData_.isJumpAttack);
			ResetCollision();
		}

		attackData_.attackAnimationFrame += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
	}
	else
	{
		//アニメーション
		const int kAnimationIdle = 5;
		const float animationSpeed = 1.0f;

		animationIndex_ = kAnimationIdle;

		UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, model_);

		//終了処理
		if (worldTransform_.translation.y <= 0.0f || characterState_.isDown)
		{
			characterState_.behaviorRequest = Behavior::kRoot;
			worldTransform_.translation.y = 0.0f;
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);
		}
	}
}

void Player::InitializeBehaviorStan()
{

}

void Player::UpdateBehaviorStan()
{
	timerData_.stanTimer--;

	//アニメーション
	const int kAnimationStun = 10;
	const float animationSpeed = 1.0f;

	animationIndex_ = kAnimationStun;

	animationTime_ = model_->GetAnimationTime();
	float animationDuration = model_->GetAnimation()[animationIndex_].duration;

	if (!characterState_.isDown)
	{
		UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);
	}

	//当たり判定を設定
	if (characterState_.direction == Direction::Left)
	{
		collider_->SetAABB(aabb_);
	}
	else if (characterState_.direction == Direction::Right)
	{
		collider_->SetAABB(aabb_);
	}

	//終了処理
	if (animationTime_ >= animationDuration || characterState_.isDown)
	{
		characterState_.behaviorRequest = Behavior::kRoot;
		animationTime_ = 0.0f;
		attackData_.attackAnimationFrame = 0;
		baseData_.guardGauge_ = 0.0f;
		timerData_.stanTimer = timerData_.maxStanTimer;
		model_->SetAnimationTime(animationTime_);
		
	}
}

void Player::OnCollision(Collider* collider)
{
	//アニメーション
	const int kAnimationGuard = 2;
	const float animationSpeed = 1.5f;

	//パーティクル
	const int kParticleTime = 55;
	const float kParticlePositionX = 0.1f;
	const float kParticlePositionY = 0.5f;

	//敵の弾との当たり判定
	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemyBullet)
	{
		//ガードバック
		const float kGuardBackSpeed = 0.1f;

		//地上で弾に当たった場合
		if (!characterState_.isDown && !characterState_.isGuard && worldTransform_.translation.y <= 0.0f)
		{
			//アニメーション
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);

			//サウンド再生
			audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);

			//ダメージの適応
			ApplyDamage();

			//体力に応じてダウンの仕方を変更
			if (baseData_.hp_ < 0)
			{
				characterState_.isHitBullet = true;
			}
			else
			{
				characterState_.isHitTCHighPunch = true;
			}

			//ゲージ増加
			AdjustFinisherGauge(enemy_->GetFinisherGaugeIncreaseAmount());
		}

		//空中で弾に当たった場合
		if (!characterState_.isDown && !characterState_.isGuard && worldTransform_.translation.y > 0.0f)
		{
			//アニメーション
			animationTime_ = 0.0f;
			model_->SetAnimationTime(animationTime_);

			//サウンド再生
			audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);

			//ダメージの適応
			ApplyDamage();

			//ダウン状態の設定
			characterState_.isHitAirBullet = true;

			//ゲージ増加
			AdjustFinisherGauge(enemy_->GetFinisherGaugeIncreaseAmount());
		}

		if (characterState_.isGuard && characterState_.direction == Direction::Right)
		{
			//ガードタイマー
			timerData_.guardAnimationTimer--;

			//アニメーション
			animationIndex_ = kAnimationGuard;
			UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

			//サウンド再生
			audio_->PlaySoundMP3(guardSoundHandle_, false, volume_);

			//ガードバック
			worldTransform_.translation.x -= kGuardBackSpeed;

			//ゲージ増加
			AdjustGuardGauge();

			//パーティクル
			if (timerData_.guardAnimationTimer > kParticleTime)
			{

				particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x + kParticlePositionX,
					worldTransform_.translation.y + kParticlePositionY,worldTransform_.translation.z });
			}
		}
		else if (characterState_.isGuard && characterState_.direction == Direction::Left)
		{
			//ガードタイマー
			timerData_.guardAnimationTimer--;

			//アニメーション
			animationIndex_ = kAnimationGuard;
			UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

			//サウンド再生
			audio_->PlaySoundMP3(guardSoundHandle_, false, volume_);

			//ガードバック
			worldTransform_.translation.x += kGuardBackSpeed;

			//ゲージ増加
			AdjustGuardGauge();

			//パーティクル
			if (timerData_.guardAnimationTimer > kParticleTime)
			{

				particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x - kParticlePositionX,
					worldTransform_.translation.y + kParticlePositionY,worldTransform_.translation.z });
			}
		}
	}

	//敵との当たり判定
	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemy)
	{
		//キャラクター同士が当たっている
		characterState_.isHitCharacter = true;

		//押し出し
		if (characterState_.isHitCharacter && !characterState_.isDown && !attackData_.isAttack && !attackData_.isTackle && !attackData_.isUppercut && !isFinisherEffect_)
		{
			//プレイヤーと敵のAABB
			float playerMinX = worldTransform_.translation.x + aabb_.min.x;
			float playerMaxX = worldTransform_.translation.x + aabb_.max.x;
			float enemyMinX = enemy_->GetWorldTransform().translation.x + enemy_->GetAABB().min.x;
			float enemyMaxX = enemy_->GetWorldTransform().translation.x + enemy_->GetAABB().max.x;

			//重なりチェック
			if (!(playerMaxX < enemyMinX || playerMinX > enemyMaxX))
			{
				float overlapX =
					((playerMaxX < enemyMaxX) ? playerMaxX : enemyMaxX) -
					((playerMinX > enemyMinX) ? playerMinX : enemyMinX);

				//補正量を調整
				const float kCorrectionFactor = 3.0f; 
				float adjustedOverlapX = overlapX * (kCorrectionFactor * GameTimer::GetDeltaTime());

				//位置補正
				if (worldTransform_.translation.x < enemy_->GetWorldTransform().translation.x)
				{
					worldTransform_.translation.x -= adjustedOverlapX;
				}
				else
				{
					worldTransform_.translation.x += adjustedOverlapX;
				}
			}
		}

		//タックル以外の攻撃をガードした場合
		if (enemy_->GetIsAttack() && !enemy_->GetIsTackle() && characterState_.isGuard)
		{
			//ガードバック
			const float kGuardBackSpeed = 0.3f;

			//ガードタイマー
			timerData_.guardAnimationTimer--;

			//アニメーション
			animationIndex_ = kAnimationGuard;
			UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

			//サウンド再生
			audio_->PlaySoundMP3(guardSoundHandle_, false, volume_);

			//敵の座標
			float enemyPosition = enemy_->GetWorldPosition().x;

			//ゲージ増加
			AdjustGuardGauge();

			//向きに応じて処理を変える
			if (characterState_.direction == Direction::Right)
			{
				//ガードバック
				worldTransform_.translation.x -= kGuardBackSpeed;
				enemyPosition += kGuardBackSpeed;

				//パーティクル
				if (timerData_.guardAnimationTimer > kParticleTime)
				{

					particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x + kParticlePositionX,
						worldTransform_.translation.y + kParticlePositionY,worldTransform_.translation.z });
				}
			}
			else
			{
				//ガードバック
				worldTransform_.translation.x += kGuardBackSpeed;
				enemyPosition -= kGuardBackSpeed;

				//パーティクル
				if (timerData_.guardAnimationTimer > kParticleTime)
				{

					particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x - kParticlePositionX,
						worldTransform_.translation.y + kParticlePositionY,worldTransform_.translation.z });
				}
			}

			//エネミーのざひょうをセット
			enemy_->SetPositionX(enemyPosition);
		}

		//タックル攻撃をガードした場合
		if (enemy_->GetIsAttack() && enemy_->GetIsTackle() && characterState_.isGuard)
		{
			//ガードバック
			const float kGuardBackSpeed = 0.2f;

			//ガードタイマー
			timerData_.guardAnimationTimer--;

			//アニメーション
			animationIndex_ = kAnimationGuard;
			UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);
			
			//サウンド再生
			audio_->PlaySoundMP3(guardSoundHandle_, false, volume_);

			//ゲージ増加
			AdjustGuardGauge();

			//向きに応じて処理を変える
			if (characterState_.direction == Direction::Right)
			{
				//ガードバック
				worldTransform_.translation.x -= kGuardBackSpeed;

				//パーティクル
				if (timerData_.guardAnimationTimer > kParticleTime)
				{

					particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x + kParticlePositionX,
						worldTransform_.translation.y + kParticlePositionY,worldTransform_.translation.z });
				}
			}
			else
			{
				//ガードバック
				worldTransform_.translation.x += kGuardBackSpeed;

				//パーティクル
				if (timerData_.guardAnimationTimer > kParticleTime)
				{

					particleEffectPlayer_->PlayParticle("Guard", { worldTransform_.translation.x - kParticlePositionX,
						worldTransform_.translation.y + kParticlePositionY,worldTransform_.translation.z });
				}
			}
		}

		//無敵状態ではない場合
		if (!isFinisherInvincible_ && enemy_->GetIsAttack() && !characterState_.isDown && !characterState_.isGuard)
		{
			//サウンド再生
			audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);

			//ダメージの適応
			ApplyDamage();

			//ゲージ増加
			AdjustFinisherGauge(enemy_->GetFinisherGaugeIncreaseAmount());

			//弱パンチ
			if (enemy_->GetIsLightPunch())
			{
				//体力に応じてダウン状態を変更
				if (baseData_.hp_ < 0)
				{
					characterState_.isHitLightPunch = true;
				}
				else
				{
					characterState_.isHitTCHighPunch = true;
				}
			}
			//TC中パンチ
			else if (enemy_->GetIsTCMiddlePunch())
			{
				//体力に応じてダウン状態を変更
				if (baseData_.hp_ < 0)
				{
					characterState_.isHitTCMiddlePunch = true;
				}
				else
				{
					characterState_.isHitTCHighPunch = true;
				}
			}
			//タックル
			else if (enemy_->GetIsTackle())
			{
				//キャンセルではない場合
				//ダウン状態の設定
				characterState_.isHitTackle = true;

				//ヒットストップ
				hitStop_->Start(enemy_->GetHitStop());

			}
		}

		//強パンチ
		if (enemy_->GetIsHighPunch() && !characterState_.isDown && !characterState_.isGuard)
		{
			//サウンド再生
			audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);

			//ダメージの適応
			ApplyDamage();

			//ゲージ増加
			AdjustFinisherGauge(enemy_->GetFinisherGaugeIncreaseAmount());

			//ダウン状態の設定
			characterState_.isHitHighPunch = true;
		}

		//空中にいる場合
		const float kPlayerPositionY = 0.5f;
		if (!isFinisherInvincible_ && enemy_->GetIsTackle() &&  characterState_.isDown && worldTransform_.translation.y > kPlayerPositionY && !isCancel_)
		{
			//キャンセルのとき
			const float kHitStop = 0.3f;
			attackData_.isDamaged = false;
			attackData_.isFinisherGaugeIncreased = false;

			//サウンド再生
			audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);

			//ダメージの適応
			ApplyDamage();

			//ゲージ増加
			AdjustFinisherGauge(enemy_->GetFinisherGaugeIncreaseAmount());

			//ダウンアニメーションタイマーの設定
			timerData_.downAnimationTimer = timerData_.maxDownAnimationTimer;

			//アニメーション
			float animationTime = 0.0f;
			model_->SetAnimationTime(animationTime);

			//ダウン状態の設定
			characterState_.isHitHighPunch = false;
			characterState_.isHitTackle = true;

			//ヒットストップ
			hitStop_->Start(kHitStop);
		}
	}
}

void Player::Move()
{
	//コントローラーの取得
	if (input_->GetJoystickState())
	{
		//入力
		const float kValueY = -0.3f;
		bool isFrontMove_ = false;
		bool isBackMove_ = false;

		//アニメーション
		const int kAnimationBackMove = 0;
		const int kAnimationFrontMove = 1;
		const int kAnimationGuard = 2;
		const int kAnimationIdle = 5;
		const float animationSpeed = 1.5f;

		//速度
		const float koveSpeed = 0.01f;
		moveData_.velocity = { 0.0f, 0.0f, 0.0f };
		moveData_.velocity.x = (float)input_->GetLeftStickX();

		//敵の位置を取得する
		Vector3 enemyPosition = enemy_->GetWorldPosition();

		if (characterState_.isHitCharacter)
		{
			const float kPushSpeed = 0.05f;
			if (characterState_.direction == Direction::Right && !attackData_.isAttack && (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) || input_->GetLeftStickX() > kStickDeadZone_))
			{
				//敵を右方向に押す
				PushEnemy(enemyPosition, kPushSpeed);
			}
			else if (characterState_.direction == Direction::Left && !attackData_.isAttack && (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) || input_->GetLeftStickX() < -kStickDeadZone_))
			{
				//敵を左方向に押す
				PushEnemy(enemyPosition, -kPushSpeed);
			}
		}

		//移動処理
		//前方向に移動(左を向いている場合)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && characterState_.direction == Direction::Left && !characterState_.isDown)
		{
			//速度の設定
			moveData_.velocity.x = -koveSpeed;
			isFrontMove_ = true;
			characterState_.isGuard = false;
		}

		if (input_->GetLeftStickX() < -kStickDeadZone_ && characterState_.direction == Direction::Left && !characterState_.isDown)
		{
			//速度の設定
			moveData_.velocity.x = -koveSpeed;
			isFrontMove_ = true;
			characterState_.isGuard = false;
		}
		
		//前方向に移動(右を向いている場合)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && characterState_.direction == Direction::Right && !characterState_.isDown)
		{
			//速度の設定
			moveData_.velocity.x = koveSpeed;
			isFrontMove_ = true;
			characterState_.isGuard = false;
		}

		if (input_->GetLeftStickX() > kStickDeadZone_ && characterState_.direction == Direction::Right && !characterState_.isDown)
		{
			//速度の設定
			moveData_.velocity.x = koveSpeed;
			isFrontMove_ = true;
			characterState_.isGuard = false;
		}

		//後ろ方向に移動(右を向いている場合)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && characterState_.direction == Direction::Right && !characterState_.isDown)
		{
			characterState_.isGuard = true;

			//移動しながらガード
			if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
			{
				//速度の設定
				moveData_.velocity.x = -koveSpeed;
				isBackMove_ = true;
			}

			//止まってガード
			if (characterState_.isGuard && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
			{
				//アニメーション
				animationIndex_ = kAnimationGuard;
				UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

				//速度の設定
				moveData_.velocity.x = 0.0f;
				isBackMove_ = false;
			}
		}

		if (input_->GetLeftStickX() < -kStickDeadZone_ && characterState_.direction == Direction::Right && !characterState_.isDown)
		{
			characterState_.isGuard = true;

			//移動しながらガード
			if (!(input_->GetLeftStickY() < kValueY))
			{
				//速度の設定
				moveData_.velocity.x = -koveSpeed;
				isBackMove_ = true;
			}

			//止まってガード
			if (characterState_.isGuard && input_->GetLeftStickY() < kValueY)
			{
				//アニメーション
				animationIndex_ = kAnimationGuard;
				UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

				//速度の設定
				moveData_.velocity.x = 0.0f;
				isBackMove_ = false;
			}
		}

		//後ろ方向に移動(左を向いている場合)
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && characterState_.direction == Direction::Left && !characterState_.isDown)
		{
			characterState_.isGuard = true;

			//移動しながらガード
			if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
			{
				//速度の設定
				moveData_.velocity.x = koveSpeed;
				isBackMove_ = true;
			}

			//止まってガード
			if (characterState_.isGuard && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
			{
				//アニメーション
				animationIndex_ = kAnimationGuard;
				UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

				//速度の設定
				moveData_.velocity.x = 0.0f;
				isBackMove_ = false;
			}
		}

		if (input_->GetLeftStickX() > kStickDeadZone_ && characterState_.direction == Direction::Left && !characterState_.isDown)
		{
			characterState_.isGuard = true;

			//移動しながらガード
			if (!(input_->GetLeftStickY() < kValueY))
			{
				//速度の設定
				moveData_.velocity.x = koveSpeed;
				isBackMove_ = true;
			}

			//止まってガード
			if (characterState_.isGuard && input_->GetLeftStickY() < kValueY)
			{
				//アニメーション
				animationIndex_ = kAnimationGuard;
				UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

				//速度の設定
				moveData_.velocity.x = 0.0f;
				isBackMove_ = false;
			}
		}

		//移動していない場合
		if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && 
			!(input_->GetLeftStickX() > kStickDeadZone_) && !(input_->GetLeftStickX() < -kStickDeadZone_))
		{
			moveData_.velocity = { 0.0f, 0.0f, 0.0f };
			characterState_.isGuard = false;
		}

		//移動
		if (isFrontMove_)
		{
			//アニメーション
			animationIndex_ = kAnimationFrontMove;
			UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, model_);

			//移動処理
			moveData_.velocity = Normalize(moveData_.velocity);
			moveData_.velocity = Multiply(moveData_.frontSpeed_, moveData_.velocity);
			worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

			//WorldTransformの更新
			worldTransform_.UpdateMatrixEuler();
		}
		else if (isBackMove_)
		{
			//アニメーション
			animationIndex_ = kAnimationBackMove;
			UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, model_);

			//移動処理
			moveData_.velocity = Normalize(moveData_.velocity);
			moveData_.velocity = Multiply(moveData_.backSpeed_, moveData_.velocity);
			worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

			//WorldTransformの更新
			worldTransform_.UpdateMatrixEuler();
		}
		else if(timerData_.guardAnimationTimer == timerData_.maxGuardAnimationTimer && !characterState_.isGuard)
		{
			//アニメーション
			const float kIdleAnimationSpeed = 1.0f;
			animationIndex_ = kAnimationIdle;
			UpdateAnimationTime(animationTime_, true, kIdleAnimationSpeed, animationIndex_, model_);
		}
	}
}

void Player::ShootBullet(const Vector3& startPosition, const Vector3& velocity)
{
	//弾を生成してリストに追加する
	auto newBullet = std::make_unique<PlayerBullet>();
	newBullet->Create(bulletModel_.get(), startPosition, velocity);
	bullets_.push_back(std::move(newBullet));
}

void Player::UpdateBullets()
{
	//弾の更新と衝突判定などを行う
	for (auto it = bullets_.begin(); it != bullets_.end();)
	{
		(*it)->Update();
		if ((*it)->GetIsDead())
		{
			it = bullets_.erase(it);
		}
		else
		{
			++it;
		}
	}
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
	//ダメージの適応
	if (!attackData_.isDamaged)
	{
		attackData_.isDamaged = true;
		baseData_.hp_ += enemy_->GetDamage();
	}
}

void Player::ResetCollision()
{
	//当たり判定のリセット
	aabb_ = defaultCollsiion_;
	collider_->SetAABB(aabb_);
}

void Player::UpdateHPBar()
{
	//体力ゲージ
	const float kHpBarSizeY = 7.0f;
	const int kDivisionFactor = 2;
	const int kHalfHp = baseData_.kMaxHp_ / kDivisionFactor;
	const int kQuarterHp = kHalfHp / kDivisionFactor;

	//色
	const Vector4 kDefaultHpColor = { 0.0f, 1.0f, 0.0f, 1.0f };
	const Vector4 kHalfHpColor = { 1.0f, 0.8f, 0.0f, 1.0f };
	const Vector4 kQuarterHpColor = { 1.0f, 0.0f, 0.0f, 1.0f };

	//サイズを設定
	hpBar_.size_ = { (static_cast<float>(baseData_.hp_) / static_cast<float>(baseData_.kMaxHp_)) * barSize_, kHpBarSizeY };
	hpBar_.sprite_->SetSize(hpBar_.size_);

	//体力に応じて色を変化
	if (baseData_.hp_ < -kHalfHp)
	{
		hpBar_.sprite_->SetColor(kDefaultHpColor);
	}
	else if (baseData_.hp_ >= -kHalfHp && baseData_.hp_ < -kQuarterHp)
	{
		hpBar_.sprite_->SetColor(kHalfHpColor);
	}
	else if (baseData_.hp_ >= -kQuarterHp)
	{
		hpBar_.sprite_->SetColor(kQuarterHpColor);
	}
}

void Player::UpdateGuardGaugeBar()
{
	//ガードゲージ
	const float kGuradGaugeBarSizeY = 7.0f;
	const float kGuardGaugeIncreaseSpeed = 0.03f;

	//色
	const Vector4 kDefaultGuardGaugeColor = { 0.0f, 0.5f, 1.0f, 1.0f };

	if (baseData_.guardGauge_ < 0.0f && baseData_.guardGauge_ > -baseData_.kMaxGuardGauge_)
	{
		baseData_.guardGauge_ += kGuardGaugeIncreaseSpeed;
	}

	//サイズを設定
	guardGaugeBar_.size_ = { (baseData_.guardGauge_ / baseData_.kMaxGuardGauge_) * guardGaugeBarSize_,kGuradGaugeBarSizeY };
	guardGaugeBar_.sprite_->SetSize(guardGaugeBar_.size_);

	//色を設定
	guardGaugeBar_.sprite_->SetColor(kDefaultGuardGaugeColor);

	//ガードゲージが最大になった場合
	if (baseData_.guardGauge_ <= -baseData_.kMaxGuardGauge_)
	{
		baseData_.guardGauge_ = -baseData_.kMaxGuardGauge_;
		characterState_.isGuard = false;
		attackData_.isAttack = false;
		characterState_.behaviorRequest = Behavior::kStan;
	}
}

void Player::AdjustGuardGauge()
{
	//攻撃パラメータに設定されているデータを適応
	const float kMaxGuardGauge = -50.0f;

	if (!attackData_.isGuarded)
	{
		if (baseData_.guardGauge_ > kMaxGuardGauge)
		{
			//ガードゲージを増加
			baseData_.guardGauge_ -= enemy_->GetGuardGaugeIncreaseAmount();
		}

		attackData_.isGuarded = true;
	}
}

void Player::UpdateFinisherGaugeBar()
{
	//必殺技ゲージ
	const float kFinisherGaugeBarSizeY = 19.3f;

	//色
	const Vector4 kDefaultHpColor = { 0.0f, 0.5f, 1.0f, 1.0f };
	const Vector4 kMaxChargeColor = { 1.0f, 0.5f, 0.0f, 1.0f };

	//サイズを設定
	finisherGaugeBar_.size_ = { (baseData_.finisherGauge_ / baseData_.kMaxFinisherGauge_) * finisherGaugeBarSize_,kFinisherGaugeBarSizeY };
	finisherGaugeBar_.sprite_->SetSize(finisherGaugeBar_.size_);

	//ゲージの状態に応じて色を変化
	if (baseData_.finisherGauge_ > -baseData_.kMaxFinisherGauge_)
	{
		finisherGaugeBar_.sprite_->SetColor(kDefaultHpColor);
	}
	else
	{
		finisherGaugeBar_.sprite_->SetColor(kMaxChargeColor);
	}

	//必殺技ゲージが最大になった場合
	if (baseData_.finisherGauge_ <= -baseData_.kMaxFinisherGauge_)
	{
		baseData_.finisherGauge_ = -baseData_.kMaxFinisherGauge_;
		isFinisherCharge_ = true;
	}
	else
	{
		isFinisherCharge_ = false;
	}
}

void Player::AdjustFinisherGauge(float value)
{
	//敵の必殺技ゲージを取得
	float finisherGaugeEnemy = enemy_->GetFinisherGauge();
	const float kEnemyMaxFinisherGauge = 50.0f;

	//攻撃パラメータに設定されているデータを適応
	const float kMaxFinisherGauge = -50.0f;
	if (!attackData_.isFinisherGaugeIncreased)
	{
		if (baseData_.finisherGauge_ > kMaxFinisherGauge)
		{
			baseData_.finisherGauge_ -= value * attackData_.takeFinisherGaugeIncreaseAmount;
		}

		if (finisherGaugeEnemy < kEnemyMaxFinisherGauge)
		{
			finisherGaugeEnemy += value;
		}

		attackData_.isFinisherGaugeIncreased = true;
	}

	//最大値に設定
	if (baseData_.finisherGauge_ < kMaxFinisherGauge)
	{
		baseData_.finisherGauge_ = kMaxFinisherGauge;
	}

	if (finisherGaugeEnemy > kEnemyMaxFinisherGauge)
	{
		finisherGaugeEnemy = kEnemyMaxFinisherGauge;
	}

	enemy_->SetFinisherGauge(finisherGaugeEnemy);
}

void Player::Reset()
{
	//リセット
	BaseCharacter::Reset();

	//HPの設定
	baseData_.hp_ = -baseData_.kMaxHp_;

	//アニメーション
	const int kAnimationIdle = 5;
	animationIndex_ = kAnimationIdle;
	animationTime_ = 0.0f;
	model_->SetAnimationTime(animationTime_);

	//WorldTransformの設定
	const float kDefaultTranslationX = -1.5f;
	worldTransform_.translation = { kDefaultTranslationX,0.0f,0.0f };
	worldTransform_.rotation = { 0.0f,characterState_.rightDirectionRotation,0.0f };
	characterState_.direction = Direction::Right;

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();

	//フラグのリセット
	isKO_ = false;
	isCancel_ = false;
	isFinisherEffect_ = false;
}

void Player::DownAnimation()
{
	//弱攻撃
	if (characterState_.isHitLightPunch)
	{
		//ダウン状態に設定
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		//アニメーション
		const int kAnimationLightDown = 4;
		const float animationSpeed = 1.5f;
		animationIndex_ = kAnimationLightDown;
		UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

		//パーティクル
		const int kParticleTime = 55;
		const float kParticleMoveSpeed = 0.1f;
		float particlePositionX = (characterState_.direction == Direction::Right) ? -kParticleMoveSpeed : kParticleMoveSpeed;
		const float particlePositionY = 0.5f;

		if (timerData_.downAnimationTimer > kParticleTime)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePositionX,
				 worldTransform_.translation.y + particlePositionY,worldTransform_.translation.z });
		}

		//ヒットバック
		const int kHitBackTime = 58;
		const float kHitBackSpeed = 0.05f;

		//修正中
		if (timerData_.downAnimationTimer > kHitBackTime)
		{
			if (characterState_.direction == Direction::Right)
			{
				worldTransform_.translation.x -= kHitBackSpeed;
			}
			else if (characterState_.direction == Direction::Left)
			{
				worldTransform_.translation.x += kHitBackSpeed;
			}
		}

		//終了処理
		if (!enemy_->GetIsLightPunch() && baseData_.hp_ < 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitLightPunch);
			isKO_ = false;
		}
	}

	//TC中攻撃
	if (characterState_.isHitTCMiddlePunch)
	{
		//ダウン状態に設定
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		//アニメーション
		const int kAnimationLightDown = 4;
		const float animationSpeed = 1.5f;
		animationIndex_ = kAnimationLightDown;
		UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

		//パーティクル
		const int kParticleTime = 55;
		const float kParticleMoveSpeed = 0.1f;
		float particlePositionX = (characterState_.direction == Direction::Right) ? -kParticleMoveSpeed : kParticleMoveSpeed;
		const float particlePositionY = 0.5f;

		if (timerData_.downAnimationTimer > kParticleTime)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePositionX,
				 worldTransform_.translation.y + particlePositionY,worldTransform_.translation.z });
		}

		//終了処理
		if (!enemy_->GetIsTCMiddlePunch() && baseData_.hp_ < 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitTCMiddlePunch);
			isKO_ = false;
		}
	}

	//TC強攻撃
	if (characterState_.isHitTCHighPunch)
	{
		//ダウン状態に設定
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		//アニメーション
		const int kAnimationHeavyDown = 7;
		const float animationSpeed = 1.5f;
		animationIndex_ = kAnimationHeavyDown;
		UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

		//パーティクル
		const int kParticleTime = 55;
		const float kParticleMoveSpeed = 0.1f;
		float particlePositionX = (characterState_.direction == Direction::Right) ? -kParticleMoveSpeed : kParticleMoveSpeed;
		const float particlePositionY = 0.5f;

		if (timerData_.downAnimationTimer > kParticleTime)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePositionX,
						worldTransform_.translation.y + particlePositionY, worldTransform_.translation.z });
		}

		//当たり判定の設定
		const AABB kDownAABB = (characterState_.direction == Direction::Right) ? AABB{ {-1.1f, 0.0f, -0.3f}, {-0.1f, 0.2f, 0.3f} } :
			AABB{ {0.1f, 0.0f, -0.3f}, {1.1f, 0.2f, 0.3f} };

		aabb_ = kDownAABB;
		collider_->SetAABB(aabb_);

		//移動処理
		const int kMoveTime = 35;
		const float kMoveSpeed = 0.02f;
		float moveX = (characterState_.direction == Direction::Right) ? -kMoveSpeed : kMoveSpeed;

		if (timerData_.downAnimationTimer > kMoveTime && ((characterState_.direction == Direction::Left && worldTransform_.translation.x < rightEdge_) ||
			(characterState_.direction == Direction::Right && worldTransform_.translation.x > leftEdge_)))
		{
			worldTransform_.translation.x += moveX;
		}

		//終了処理
		if (!enemy_->GetIsTCHighPunch() && baseData_.hp_ < 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitTCHighPunch);
			ResetCollision();
			isKO_ = false;
		}
	}

	//強攻撃
	if (characterState_.isHitHighPunch)
	{
		//ダウン状態に設定
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		//アニメーション
		const int kAnimationHeavyDown = 7;
		const float animationSpeed = 1.5f;
		animationIndex_ = kAnimationHeavyDown;
		UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

		//パーティクル
		const int kParticleTime = 55;
		const float kParticleMoveSpeed = 0.1f;
		float particlePositionX = (characterState_.direction == Direction::Right) ? -kParticleMoveSpeed : kParticleMoveSpeed;
		const float particlePositionY = 0.5f;

		if (timerData_.downAnimationTimer > kParticleTime)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePositionX,
				 worldTransform_.translation.y + particlePositionY,worldTransform_.translation.z });
		}

		//当たり判定の設定
		const AABB kDownAABB = (enemy_->GetDirection() == Direction::Right) ? AABB{ {0.1f, 0.0f, -0.3f}, {1.1f, 0.2f, 0.3f} } :
			AABB{ {-1.1f, 0.0f, -0.3f}, {-0.1f, 0.2f, 0.3f} };

		aabb_ = kDownAABB;
		collider_->SetAABB(aabb_);

		//移動処理
		const int kJumpTime = 55;
		const int kFallTime = -30;
		const float kJumpFirstSpeed_ = 0.15f;
		const float kMoveSpeed = 0.025f;
		float moveX = (enemy_->GetDirection() == Direction::Right) ? kMoveSpeed : -kMoveSpeed;

		if (timerData_.downAnimationTimer > kJumpTime)
		{
			//ジャンプする
			moveData_.velocity.x = moveX;
			moveData_.velocity.y = kJumpFirstSpeed_;
		}
		else if (timerData_.downAnimationTimer > kFallTime)
		{
			//落ちる
			worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

			const float kGravityAcceleration_ = 0.005f;
			Vector3 accelerationVector_ = { 0.0f, -kGravityAcceleration_, 0.0f };

			moveData_.velocity = Add(moveData_.velocity, accelerationVector_);

			if (worldTransform_.translation.y <= 0.0f)
			{
				moveData_.velocity.x = 0.0f;
				worldTransform_.translation.y = 0.0f;
			}
		}

		//終了処理
		if (!enemy_->GetIsHighPunch() && worldTransform_.translation.y <= 0.0f && baseData_.hp_ < 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitHighPunch);
			ResetCollision();
			isKO_ = false;
		}
	}

	//タックル攻撃
	if (characterState_.isHitTackle)
	{
		//ダウン状態に設定
		characterState_.isDown = true;
		isCancel_ = true;
		timerData_.downAnimationTimer -= static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
		timerData_.effectTimer--;

		//アニメーション
		const int kAnimationHeavyDown = 7;
		const float animationSpeed = 1.5f;
		animationIndex_ = kAnimationHeavyDown;
		UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

		//パーティクル
		const int kParticleTime = 55;
		const float kParticleMoveSpeed = 0.1f;
		float particlePositionX = (characterState_.direction == Direction::Right) ? -kParticleMoveSpeed : kParticleMoveSpeed;
		const float particlePositionY = 0.5f;

		if (timerData_.effectTimer > kParticleTime)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePositionX,
				 worldTransform_.translation.y + particlePositionY,worldTransform_.translation.z });
		}

		//当たり判定の設定
		const AABB kDownAABB = (enemy_->GetDirection() == Direction::Right) ? AABB{ {0.1f, 0.0f, -0.3f}, {1.1f, 0.2f, 0.3f} } :
			AABB{ {-1.1f, 0.0f, -0.3f}, {-0.1f, 0.2f, 0.3f} };

		aabb_ = kDownAABB;
		collider_->SetAABB(aabb_);

		const int kMoveTime = 35;
		const float kDefaultMoveSpeed = 4.8f;
		const float kTradeMoveSpeed = 2.0f;
		const float kFallSpeed = 2.0f;
		float moveX = 0.0f;

		//相手かダウン状態かどうかで速さを変化
		if (!enemy_->GetIsDown())
		{
			moveX = (enemy_->GetDirection() == Direction::Right) ? kDefaultMoveSpeed : -kDefaultMoveSpeed;
		}
		else
		{
			moveX = (enemy_->GetDirection() == Direction::Right) ? kTradeMoveSpeed : -kTradeMoveSpeed;
		}

		//移動
		if (timerData_.downAnimationTimer > kMoveTime && ((characterState_.direction == Direction::Left && worldTransform_.translation.x < rightEdge_) ||
			(characterState_.direction == Direction::Right && worldTransform_.translation.x > leftEdge_)))
		{
			worldTransform_.translation.x += moveX * GameTimer::GetDeltaTime();
		}

		//空中でくらった場合
		if (worldTransform_.translation.y > 0.0f)
		{
			worldTransform_.translation.y -= kFallSpeed * GameTimer::GetDeltaTime();
		}
		else if (worldTransform_.translation.y <= 0.0f)
		{
			worldTransform_.translation.y = 0.0f;
		}

		//終了処理
		if (timerData_.downAnimationTimer < 0 && baseData_.hp_ < 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitTackle);
			isCancel_ = false;
			isKO_ = false;
			ResetCollision();
		}
	}

	//弾攻撃(地上)
	if (characterState_.isHitBullet)
	{
		//ダウン状態に設定
		const int kDownTime = 30;
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		//アニメーション
		const int kAnimationLightDown = 4;
		const float animationSpeed = 1.5f;
		animationIndex_ = kAnimationLightDown;
		UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

		//パーティクル
		const float kParticleMoveSpeed = 0.1f;
		float particlePositionX = (characterState_.direction == Direction::Right) ? -kParticleMoveSpeed : kParticleMoveSpeed;
		const float particlePositionY = 0.5f;

		if (!isParticle_)
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePositionX,
				worldTransform_.translation.y + particlePositionY, worldTransform_.translation.z });

			isParticle_ = true;
		}

		//終了処理
		if (timerData_.downAnimationTimer < kDownTime && baseData_.hp_ < 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitBullet);
			ResetCollision();

			isParticle_ = false;
			isKO_ = false;
		}
	}
	//弾攻撃(空中)
	else if (characterState_.isHitAirBullet)
	{
		//ダウン状態に設定
		const int kDownTime = 30;
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;
		
		//アニメーション
		const int kAnimationHeavyDown = 7;
		const float animationSpeed = 1.5f;
		animationIndex_ = kAnimationHeavyDown;
		UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);

		//パーティクル
		const float kParticleMoveSpeed = 0.1f;
		float particlePositionX = (characterState_.direction == Direction::Right) ? -kParticleMoveSpeed : kParticleMoveSpeed;
		const float particlePositionY = 0.5f;

		if (!isParticle_) 
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePositionX,
				 worldTransform_.translation.y + particlePositionY,worldTransform_.translation.z });

			isParticle_ = true;
		}

		//当たり判定の設定
		const AABB kDownAABB = (enemy_->GetDirection() == Direction::Right) ? AABB{ {0.1f, 0.0f, -0.3f}, {1.1f, 0.2f, 0.3f} } :
			AABB{ {-1.1f, 0.0f, -0.3f}, {-0.1f, 0.2f, 0.3f} };

		aabb_ = kDownAABB;
		collider_->SetAABB(aabb_);

		//移動処理
		const int kMoveTime = 35;
		const float kMoveSpeed = enemy_->GetDirection() == Direction::Right ? -0.08f : 0.08f;
		const float kFallSpeed = 0.03f;
		float moveX = kMoveSpeed;
		float rotationY = enemy_->GetDirection() == Direction::Right ? characterState_.leftDirectionRotation : characterState_.rightDirectionRotation;

		//移動
		if (timerData_.downAnimationTimer > kMoveTime && worldTransform_.translation.x > -4.0f)
		{
			worldTransform_.translation.x -= moveX;
			worldTransform_.rotation.y = rotationY;
		}

		//落ちる
		if (worldTransform_.translation.y > 0.0f)
		{
			worldTransform_.translation.y -= kFallSpeed;
		}
		else if (worldTransform_.translation.y <= 0.0f)
		{
			worldTransform_.translation.y = 0.0f;
		}

		//終了処理
		if (timerData_.downAnimationTimer < kDownTime && baseData_.hp_ < 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitAirBullet);
			ResetCollision();

			isParticle_ = false;
			isKO_ = false;
		}
	}

	//KOの場合
	const int kKOTime = 50;
	if (timerData_.downAnimationTimer < kKOTime && baseData_.hp_ >= 0)
	{
		isKO_ = true;
	}
}

void Player::EndDownAnimation(int animationIndex, bool& isHitAttackType)
{
	//ダウンアニメーションの終了処理
	BaseCharacter::EndDownAnimation(animationIndex, isHitAttackType);
}

void Player::PushEnemy(Vector3& enemyPosition, float pushSpeed)
{
	//敵の位置を押す速度分だけ更新
	enemyPosition.x += pushSpeed;

	//敵のワールドトランスフォームを更新
	enemy_->GetWorldTransform().translation = enemyPosition;
	enemy_->GetWorldTransform().UpdateMatrixEuler();
}

void Player::UpdateComboNumberSprite()
{
	//コンボ表示の更新
	int comboNum = comboCount_;

	comboNumTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Number/" + std::to_string(comboNum) + ".png");

	comboNumSprite_->SetTexture(comboNumTextureHandle_);
}

void Player::HitCombo()
{
	//コンボを食らっているとき
	//弱パンチ
	if (characterState_.isHitLightPunch && !characterState_.isDown)
	{
		ComboCountUpdate(enemy_->GetHitRecoveryTime());
	}

	//TC中パンチ
	if (characterState_.isHitTCMiddlePunch && !characterState_.isDown)
	{
		ComboCountUpdate(enemy_->GetHitRecoveryTime());
	}

	//TC強パンチ
	if (characterState_.isHitTCHighPunch && !characterState_.isDown)
	{
		ComboCountUpdate(enemy_->GetHitRecoveryTime());
	}

	//強パンチ
	if (characterState_.isHitHighPunch && !characterState_.isDown)
	{
		ComboCountUpdate(enemy_->GetHitRecoveryTime());
	}

	//タックル
	if (characterState_.isHitTackle && !isCancel_)
	{
		ComboCountUpdate(enemy_->GetHitRecoveryTime());
	}

	//コンボタイマーを減らす
	if (timerData_.comboTimer >= 0)
	{
		timerData_.comboTimer--;
	}

	//終了処理
	if (timerData_.comboTimer < 0)
	{
		timerData_.comboTimer = 0;
		comboCount_ = 0;
		firstAttack_ = "";
	}
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
