/**
 * @file Enemy.cpp
 * @brief 敵の管理(移動、攻撃など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Enemy.h"
#include "Application/GameObject/Character/Player/Player.h"
#include "Application/Game/Scenes/GamePlayScene.h"
#include "Application/Game/GameTimer/GameTimer.h"

Enemy::~Enemy()
{
	//各ゲージの削除
	delete hpBar_.sprite_;
	delete guardGaugeBar_.sprite_;
	delete finisherGaugeBar_.sprite_;

	//弾の削除
	for (auto& bullet : bullets_) 
	{
		delete bullet;
	}
	bullets_.clear();
}

void Enemy::Initialize()
{
	//タグの設定
	IGame3dObject::SetTag("Enemy");

	//初期化
	ICharacter::Initialize();

	//WorldTransformの初期化
	worldTransform_.Initialize();

	//当たり判定の設定
	SetAABB(aabb_);

	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	//LineBoxの描画
	lineBox_.reset(LineBox::Create(aabb_));

	//リソース
	//体力ゲージ
	const Vector2 kHpBarPosition = { 742.0f, kBarSpace_ };
	const Vector2 kHpBarSize = { -barSize_  ,7.2f };

	hpBar_ = {
		true,
		TextureManager::LoadTexture("resource/images/HP.png"),
		kHpBarPosition,
		0.0f,
		kHpBarSize,
		nullptr,
	};

	hpBar_.sprite_ = Sprite::Create(hpBar_.textureHandle_, hpBar_.position_);

	//ガードゲージ
	const Vector2 kGuardGaugeBarPosition = { 742.0f, kGuardGaugeBarSpace_ };
	const Vector2 kGuardGaugeBarSize = { -guardGaugeBarSize_  ,7.0f };

	guardGaugeBar_ = {
		true,
		TextureManager::LoadTexture("resource/images/guardGauge.png"),
		{kGuardGaugeBarPosition},
		0.0f,
		{kGuardGaugeBarSize},
		nullptr,
	};

	guardGaugeBar_.sprite_ = Sprite::Create(guardGaugeBar_.textureHandle_, guardGaugeBar_.position_);

	//必殺技ゲージ
	const Vector2 kFinisherGaugeBarPosition = { 979.0f, kFinisherGaugeBarSpace_ };
	const Vector2 kFinisherGaugeBarSize = { -finisherGaugeBarSize_  ,19.3f };

	finisherGaugeBar_ = {
		true,
		TextureManager::LoadTexture("resource/images/finisherGauge.png"),
		{kFinisherGaugeBarPosition},
		0.0f,
		{kFinisherGaugeBarSize},
		nullptr,
	};

	finisherGaugeBar_.sprite_ = Sprite::Create(finisherGaugeBar_.textureHandle_, finisherGaugeBar_.position_);

	//ヒット表示
	const Vector2 kHitSpritePosition = { 40.0f, 180.0f };

	hitTextureHandle_ = TextureManager::LoadTexture("resource/images/Hit.png");
	hitSprite_.reset(Sprite::Create(hitTextureHandle_, kHitSpritePosition));

	//コンボ表示
	const Vector2 kComboNumSpritePosition = { 10.0f, 290.0f };

	comboNumTextureHandle_ = TextureManager::LoadTexture("resource/number/0.png");
	comboNumSprite_.reset(Sprite::Create(comboNumTextureHandle_, kComboNumSpritePosition));

	//キャラクターアイコン
	const Vector2 kEnemyIconPosition = { 1110.0f, 20.0f };
	const Vector2 kEnemyIconSize = { 120.0f,120.0f };

	enemyIconTextureHandle_ = TextureManager::LoadTexture("resource/images/EnemyIcon.png");

	enemyIconSprite_.reset(Sprite::Create(enemyIconTextureHandle_, kEnemyIconPosition));
	enemyIconSprite_->SetSize(kEnemyIconSize);

	//弾のモデルを生成
	bulletModel_.reset(Model::CreateFromOBJ("resource/bullet", "bullet.obj"));

	//パーティクル
	particleEffectPlayer_ = std::make_unique<ParticleEffectPlayer>();
	particleEffectPlayer_->Initialize();

	//SEの初期化
	attackSoundHandle_ = audio_->LoadSoundMP3("resource/Sounds/Attack.mp3");
	weaponAttackSoundHandle_ = audio_->LoadSoundMP3("resource/Sounds/WeaponAttack.mp3");
	damageSoundHandle_ = audio_->LoadSoundMP3("resource/Sounds/HitPunch1.mp3");
	guardSoundHandle_ = audio_->LoadSoundMP3("resource/Sounds/Guard.mp3");

	//行動パターンの初期化
	patternCount_ = RandomMove();

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Enemy::Update()
{
#ifdef _ADJUSTMENT
	

#endif

	//更新
	ICharacter::Update();

	//エディターで設定したパラメータをセット
	AttackEditor::GetInstance()->SetAttackParameters(attackType_, attackData_.attackStartTime, attackData_.attackEndTime, attackData_.recoveryTime,
		attackData_.damage, attackData_.guardGaugeIncreaseAmount, attackData_.finisherGaugeIncreaseAmount, attackData_.hitStop, 
		aabb_, false, characterState_.direction);

	//デバッグ用の処理
	if (isDebug_)
	{
		if (attackData_.isAttack)
		{
			//攻撃中(攻撃判定あり)にモデルの色を変える
			model_->GetMaterial()->SetColor({ 1.0f,0.0f,0.0f,1.0f });
		}
		else if (attackData_.isRecovery)
		{
			//硬直中にモデルの色を変える
			model_->GetMaterial()->SetColor({ 0.0f,0.0f,1.0f,1.0f });
		}
		else
		{
			model_->GetMaterial()->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		}
	}

	//振り向きの処理
	Vector3 playerWorldPosition = player_->GetWorldPosition();
	Vector3 enemyWorldPosition = worldTransform_.translation;

	if (enemyWorldPosition.x > playerWorldPosition.x && characterState_.behavior != Behavior::kJump
		&& characterState_.behavior != Behavior::kAttack && !characterState_.isDown)
	{
		characterState_.direction = Direction::Left;
		worldTransform_.rotation.y = characterState_.leftDirectionRotation;;
	}

	if (enemyWorldPosition.x < playerWorldPosition.x && characterState_.behavior != Behavior::kJump
		&& characterState_.behavior != Behavior::kAttack && !characterState_.isDown)
	{
		characterState_.direction = Direction::Right;
		worldTransform_.rotation.y = characterState_.rightDirectionRotation;;
	}

	difference_ = playerWorldPosition - enemyWorldPosition;
	difference_.y = 0.0f;
	distance_ = Length(difference_);

	//後ろに戻れないようにする
	if (distance_ >= kMaxDistance_)
	{
		if (worldTransform_.translation.x < previousPositionX_ && characterState_.direction == Direction::Right)
		{
			worldTransform_.translation.x = playerWorldPosition.x - kMaxDistance_;
		}
		else if (worldTransform_.translation.x > previousPositionX_ && characterState_.direction == Direction::Left)
		{
			worldTransform_.translation.x = playerWorldPosition.x + kMaxDistance_;
		}
	}

	//必殺技中でない場合
	if (player_->GetFinisherTimer() == timerData_.maxFinisherTimer)
	{
		//弾の更新
		UpdateBullets();

		//ParticleEffectPlayerの更新
		particleEffectPlayer_->Update();
	}

	//コンボ関連の処理
	HitCombo();
	UpdateComboNumberSprite();

	//ガードアニメーションタイマーのリセット
	if (!player_->GetIsAttack())
	{
		timerData_.guardAnimationTimer = timerData_.maxGuardAnimationTimer;
	}

	//LineBoxの更新
	lineBox_->Update(aabb_);

	//ライティングの設定
	model_->GetLight()->SetEnableLighting(true);

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Enemy::Draw(const Camera& camera)
{
	//モデル描画
	model_->Draw(worldTransform_, camera, animationIndex_);
}

void Enemy::DrawBone(const Camera& camera)
{
	//骨の描画
	model_->DrawBone(worldTransform_, camera);
}

void Enemy::DrawCollision(const Camera& camera)
{
	//LineBoxの描画
	lineBox_->Draw(worldTransform_, camera);
}

void Enemy::DrawSprite()
{
	//体力ゲージの描画
	if (hp_ >= 0)
	{
		hpBar_.sprite_->Draw();
	}

	//ガードゲージの描画
	guardGaugeBar_.sprite_->Draw();

	//必殺技ゲージの描画
	finisherGaugeBar_.sprite_->Draw();

	//エネミーアイコンの描画
	enemyIconSprite_->Draw();

	//コンボ表示の描画
	if (comboCount_ >= kComboCount_[2])
	{
		hitSprite_->Draw();
		comboNumSprite_->Draw();
	}
}

void Enemy::DrawBullet(const Camera& camera)
{
	//弾の描画
	for (auto& bullet : bullets_)
	{
		bullet->Draw(camera);
	}
}

void Enemy::DrawParticle(const Camera& camera)
{
	//パーティクルの描画
	particleEffectPlayer_->Draw(camera);

	for (auto& bullet : bullets_)
	{
		bullet->DrawParticle(camera);
	}
}

void Enemy::ImGui()
{
	
}

void Enemy::InitializeBehaviorRoot()
{
	//アニメーション
	const int kAnimationIdle = 5;
	animationIndex_ = kAnimationIdle;

	//行動パターンの設定
	patternCount_ = RandomMove();
}

void Enemy::UpdateBehaviorRoot()
{
	if (!isDebug_ && player_->GetFinisherTimer() == timerData_.maxFinisherTimer)
	{
		if (!characterState_.isDown && comboCount_ == 0)
		{
			if (patternCount_ == kPatternCount_[1])
			{
				const int kAnimationFrontMove = 0;
				animationIndex_ = kAnimationFrontMove;
			}
			else if (patternCount_ == 2)
			{
				const int kAnimationBackMove = 2;
				animationIndex_ = kAnimationBackMove;
			}

			//移動
			Move();
		}

		//攻撃
		//突進攻撃
		if (patternCount_ == kPatternCount_[3] && !characterState_.isDown)
		{
			const int kAnimationTackle = 8;
			animationIndex_ = kAnimationTackle;
			attackType_ = "タックル";
			StartAttack(attackData_.isTackle);
		}

		//弾攻撃
		if (patternCount_ == kPatternCount_[4] && !characterState_.isDown)
		{
			const int kAnimationShot = 1;
			animationIndex_ = kAnimationShot;
			attackType_ = "ショット";
			StartAttack(attackData_.isShot);
		}

		//弱攻撃
		if (patternCount_ == kPatternCount_[5] && !characterState_.isDown)
		{
			const int kAnimationLightPunch = 12;
			animationIndex_ = kAnimationLightPunch;
			attackType_ = "弱攻撃";
			StartAttack(attackData_.isLightPunch);
		}

		if (characterState_.isDown)
		{
			isGuardMode_ = false;
			characterState_.isGuard = false;
		}
	}
}

void Enemy::InitializeBehaviorAttack()
{
	attackData_.attackAnimationFrame = 0;
}

void Enemy::UpdateBehaviorAttack()
{
	//必殺技演出中でない場合
	if (player_->GetFinisherTimer() == timerData_.maxFinisherTimer)
	{
		//弱攻撃
		if (attackData_.isLightPunch)
		{
			//アニメーション
			const int kAnimationLighhtPunch = 12;
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
				SetAABB(aabb_);
			}
			else if (characterState_.direction == Direction::Left)
			{
				SetAABB(aabb_);
			}

			//攻撃判定をつけるタイミングの設定
			EvaluateAttackTiming();

			//終了処理
			if (characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime)
			{
				EndAttack(attackData_.isLightPunch);
				ResetCollision();
			}

			//キャンセルの処理(中TC)
			//キャンセル始まりの時間
			const int kCancelStartTime = 15;

			//キャンセル終わりの時間
			const int kCancelEndTime = 30;

			if (!characterState_.isDown && characterState_.isHitCharacter && player_->GetIsDown() && player_->GetHP() < 0 &&
				attackData_.attackAnimationFrame > kCancelStartTime && attackData_.attackAnimationFrame < kCancelEndTime)
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

			attackData_.attackAnimationFrame += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
		}

		//TC用の攻撃(2発目)
		if (attackData_.isTCMiddlePunch)
		{
			//アニメーション
			const int kAnimationTCMiddlePunch = 11;
			const float animationSpeed = 1.5f;

			animationIndex_ = kAnimationTCMiddlePunch;

			characterState_.isGuard = false;

			if (!characterState_.isDown)
			{
				UpdateAnimationTime(animationTime_, false, animationSpeed, animationIndex_, model_);
			}

			attackData_.isAttack = true;

			//当たり判定を設定
			if (characterState_.direction == Direction::Right)
			{
				SetAABB(aabb_);
			}
			else if (characterState_.direction == Direction::Left)
			{
				SetAABB(aabb_);
			}

			//攻撃判定をつけるタイミングの設定
			EvaluateAttackTiming();

			//終了処理
			if (characterState_.isDown || attackData_.attackAnimationFrame > attackData_.recoveryTime)
			{
				EndAttack(attackData_.isTCMiddlePunch);
				ResetCollision();
			}

			//キャンセルの処理(強攻撃)
			//キャンセル始まりの時間
			const int kCancelStartTime = 15;

			//キャンセル終わりの時間
			const int kCancelEndTime = 30;

			if (!characterState_.isDown && characterState_.isHitCharacter && player_->GetIsDown() && player_->GetHP() < 0 &&
				attackData_.attackAnimationFrame > kCancelStartTime && attackData_.attackAnimationFrame < kCancelEndTime)
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

			attackData_.attackAnimationFrame += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
		}

		//強攻撃
		if (attackData_.isHighPunch)
		{
			//アニメーション
			const int kAnimationTCHighPunch = 3;
			const float animationSpeed = 1.5f;

			animationIndex_ = kAnimationTCHighPunch;
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
				SetAABB(aabb_);

				//コンボがつながりやすくなるように移動する
				if (characterState_.isHitCharacter && attackData_.attackAnimationFrame <= kMoveTime)
				{
					worldTransform_.translation.x -= kMoveSpeed;
				}
			}
			else if (characterState_.direction == Direction::Left)
			{
				SetAABB(aabb_);

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

			//キャンセルの処理(タックル攻撃)
			//キャンセル始まりの時間
			const int kCancelStartTime = 15;

			//キャンセル終わりの時間
			const int kCancelEndTime = 30;

			if (!characterState_.isDown && player_->GetIsDown() && attackData_.attackAnimationFrame > kCancelStartTime && 
				attackData_.attackAnimationFrame < kCancelEndTime && player_->GetHP() < 0)
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

			attackData_.attackAnimationFrame += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
		}

		//タックル攻撃
		if (attackData_.isTackle)
		{
			//アニメーション
			const int kAnimationTackle = 8;
			const float animationSpeed = 1.5f;

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
					SetAABB(aabb_);

					//移動
					worldTransform_.translation.x += kMoveSpeed * GameTimer::GetDeltaTime();
				}

				//パーティクルの移動
				if (attackData_.attackAnimationFrame >= attackData_.attackStartTime && attackData_.attackAnimationFrame < kParticleTime)
				{
					particlePositionX += kParticleMoveSpeed;

					particleEffectPlayer_->PlayParticle("EnemyRightNackle", { worldTransform_.translation.x + particlePositionX,
						worldTransform_.translation.y + particlePositionY,worldTransform_.translation.z });
				}
				else
				{
					//硬直中の当たり判定を設定
					const AABB recoveryCollsion = { {-0.3f,0.0f,-0.3f},{0.3f,1.0f,0.3f} };
					aabb_ = recoveryCollsion;
					SetAABB(aabb_);
				}
			}
			else if (characterState_.direction == Direction::Left)
			{
				if (attackData_.attackAnimationFrame >= attackData_.attackStartTime && attackData_.attackAnimationFrame < kMoveTime)
				{
					//当たり判定を設定
					SetAABB(aabb_);

					//移動
					worldTransform_.translation.x -= kMoveSpeed * GameTimer::GetDeltaTime();
				}

				//パーティクルの移動
				if (attackData_.attackAnimationFrame >= attackData_.attackStartTime && attackData_.attackAnimationFrame < kParticleTime)
				{
					particlePositionX += kParticleMoveSpeed;

					particleEffectPlayer_->PlayParticle("EnemyLeftNackle", { worldTransform_.translation.x - particlePositionX,
						worldTransform_.translation.y + particlePositionY,worldTransform_.translation.z });
				}
				else
				{
					//硬直中の当たり判定を設定
					const AABB recoveryCollsion = { {-0.3f,0.0f,-0.3f},{0.3f,1.0f,0.3f} };
					aabb_ = recoveryCollsion;
					SetAABB(aabb_);
				}
			}

			//if (attackData_.attackAnimationFrame >= attackData_.attackEndTime)
			//{
			//	attackData_.isAttack = false;
			//	ResetCollision();
			//}

			//終了処理
			if (characterState_.isDown || attackData_.attackAnimationFrame >= attackData_.recoveryTime)
			{
				patternCount_ = RandomMove();
				EndAttack(attackData_.isTackle);
				ResetCollision();
			}

			attackData_.attackAnimationFrame += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
		}

		//弾攻撃
		if (attackData_.isShot)
		{
			//アニメーション
			const int kAnimationShot = 1;
			const float animationSpeed = 1.5f;

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
			const int kShotEndTimer = 40;
			if (characterState_.isDown || attackData_.attackAnimationFrame >= kShotEndTimer)
			{
				patternCount_ = RandomMove();
				EndAttack(attackData_.isShot);
				hasShot_ = false;
				ResetCollision();
			}

			attackData_.attackAnimationFrame += static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
		}
	}
}

void Enemy::InitializeBehaviorJump()
{
	//座標の設定
	worldTransform_.translation.y = 0.0f;

	//ジャンプの速さ
	const float kJumpFirstSpeed_ = 0.3f;

	moveData_.velocity.y = kJumpFirstSpeed_;
}

void Enemy::UpdateBehaviorJump()
{
	//アニメーション
	const int kAnimationJump = 4;
	const float animationSpeed = 1.0f;

	animationIndex_ = kAnimationJump;

	UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, model_);

	//ジャンプ
	worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

	const float kGravityAcceleration_ = 0.02f;

	Vector3 accelerationVector_ = { 0.0f,-kGravityAcceleration_,0.0f };

	moveData_.velocity = Add(moveData_.velocity, accelerationVector_);

	//終了処理
	if (worldTransform_.translation.y <= 0.0f)
	{
		characterState_.behaviorRequest = Behavior::kRoot;
		worldTransform_.translation.y = 0.0f;
		animationTime_ = 0.0f;
		model_->SetAnimationTime(animationTime_);
	}
}

void Enemy::InitializeBehaviorStan()
{

}

void Enemy::UpdateBehaviorStan()
{
	timerData_.stanTimer--;

	//アニメーション
	const int kAnimationStun = 9;
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
		SetAABB(aabb_);
	}
	else if (characterState_.direction == Direction::Right)
	{
		SetAABB(aabb_);
	}

	//終了処理
	if (animationTime_ >= animationDuration || characterState_.isDown)
	{
		characterState_.behaviorRequest = Behavior::kRoot;
		animationTime_ = 0.0f;
		attackData_.attackAnimationFrame = 0;
		guardGauge_ = 0.0f;
		timerData_.stanTimer = timerData_.maxStanTimer;
		model_->SetAnimationTime(animationTime_);

	}
}

void Enemy::OnCollision(Collider* collider)
{
	//アニメーション
	const int kAnimationGuard = 2;
	const float animationSpeed = 1.5f;

	//パーティクル
	const int kParticleTime = 55;
	const float kParticlePositionX = 0.1f;
	const float kParticlePositionY = 0.5f;

	//プレイヤーの弾との当たり判定
	if (collider->GetCollisionAttribute() & kCollisionAttributePlayerBullet)
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
			if (hp_ > 0)
			{
				characterState_.isHitBullet = true;
				attackData_.isFinisher = false;
			}
			else
			{
				characterState_.isHitTCHighPunch = true;
				attackData_.isFinisher = false;
			}

			//ゲージ増加
			AdjustFinisherGauge(player_->GetFinisherGaugeIncreaseAmount());
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
			AdjustFinisherGauge(player_->GetFinisherGaugeIncreaseAmount());
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

	//プレイヤーとの当たり判定
	if (collider->GetCollisionAttribute() & kCollisionAttributePlayer)
	{
		//キャラクター同士が当たっている
		characterState_.isHitCharacter = true;

		//タックル以外の攻撃をガードした場合
		if (player_->GetIsAttack() && !player_->GetIsTackle() && characterState_.isGuard)
		{
			//ガードバック
			const float kGuardBackSpeed = 0.3f;

			//ガードタイマー
			timerData_.guardAnimationTimer--;

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

		//タックル攻撃をガードした場合
		if (player_->GetIsAttack() && player_->GetIsTackle() && characterState_.isGuard)
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

		//弱パンチが当たった場合
		if (player_->GetIsAttack() && !characterState_.isGuard)
		{
			//弱パンチ
			if (player_->GetIsLightPunch())
			{
				if (!characterState_.isDown && firstAttack_ != "JumpAttack")
				{
					//サウンド再生
					if (!isHitAudio_)
					{
						audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);
					}

					//ダメージの適応
					ApplyDamage();

					//ゲージ増加
					AdjustFinisherGauge(player_->GetFinisherGaugeIncreaseAmount());

					//ヒットストップ
					hitStop_->Start(player_->GetHitStop());

					//体力に応じてダウン状態を変更
					if (hp_ > 0)
					{
						characterState_.isHitLightPunch = true;
					}
					else
					{
						characterState_.isHitTCHighPunch = true;
					}

					isHitAudio_ = true;
				}
				else if (characterState_.isDown && firstAttack_ == "JumpAttack")
				{
					//サウンド再生
					if (!isHitAudio_)
					{
						audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);
					}

					//ダメージの適応
					ApplyDamage();

					//ゲージ増加
					AdjustFinisherGauge(player_->GetFinisherGaugeIncreaseAmount());

					//体力に応じてダウン状態を変更
					if (hp_ > 0)
					{
						characterState_.isHitLightPunch = true;
					}
					else
					{
						characterState_.isHitTCHighPunch = true;
					}

					isHitAudio_ = true;
				}
			}
			else if (player_->GetIsMiddlePunch() && !characterState_.isDown)
			{

			}
		}

		//各攻撃が当たった場合
		if (!characterState_.isDown && !characterState_.isGuard)
		{
			//強パンチ
			if (player_->GetIsHighPunch())
			{
				//サウンド再生
				audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);

				//ダメージの適応
				ApplyDamage();

				//ゲージ増加
				AdjustFinisherGauge(player_->GetFinisherGaugeIncreaseAmount());

				//ダウン状態の設定
				characterState_.isHitHighPunch = true;
			}
			//TC中パンチ
			else if (player_->GetIsTCMiddlePunch())
			{
				//サウンド再生
				audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);

				//ダメージの適応
				ApplyDamage();

				//ゲージ増加
				AdjustFinisherGauge(player_->GetFinisherGaugeIncreaseAmount());

				//体力に応じてダウン状態を変更
				if (hp_ > 0)
				{
					characterState_.isHitTCMiddlePunch = true;
				}
				else
				{
					characterState_.isHitTCHighPunch = true;
				}
			}
			//TC強パンチ
			else if (player_->GetIsTCHighPunch())
			{
				//サウンド再生
				audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);

				//ダメージの適応
				ApplyDamage();

				//ゲージ増加
				AdjustFinisherGauge(player_->GetFinisherGaugeIncreaseAmount());

				//ダウン状態の設定
				characterState_.isHitTCHighPunch = true;
			}
			//ジャンプ攻撃
			else if (player_->GetIsAttack() && player_->GetIsJumpAttack())
			{
				//サウンド再生
				audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);

				//ダメージの適応
				ApplyDamage();

				//ゲージ増加
				AdjustFinisherGauge(player_->GetFinisherGaugeIncreaseAmount());

				//ヒットストップ
				hitStop_->Start(player_->GetHitStop());

				//体力に応じてダウン状態を変更
				if (hp_ > 0)
				{
					characterState_.isHitJumpAttack = true;
				}
				else
				{
					characterState_.isHitTCHighPunch = true;
				}
			}
			//タックル攻撃
			else if (player_->GetIsTackle() && player_->GetIsAttack())
			{
				//キャンセルじゃないとき
				//サウンド再生
				audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);

				//ダメージの適応
				ApplyDamage();

				//ゲージ増加
				AdjustFinisherGauge(player_->GetFinisherGaugeIncreaseAmount());

				//ヒットストップ
				hitStop_->Start(player_->GetHitStop());

				//ダウン状態の設定
				characterState_.isHitTackle = true;
			}
			//アッパー攻撃
			else if (player_->GetIsUppercut() && player_->GetIsAttack())
			{
				//サウンド再生
				audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);

				//ダメージの適応
				ApplyDamage();

				//ゲージ増加
				AdjustFinisherGauge(player_->GetFinisherGaugeIncreaseAmount());

				//体力に応じてダウン状態を変更
				if (hp_ > 0)
				{
					characterState_.isHitUppercut = true;
				}
				else
				{
					characterState_.isHitHighPunch = true;
				}
			}
			//超必(1段目)
			else if (player_->GetIsFinisherFirstAttack() && player_->GetIsAttack())
			{
				//サウンド再生
				audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);

				//ダメージの適応
				ApplyDamage();

				//体力に応じてダウン状態を変更
				if (hp_ > 0)
				{
					characterState_.isHitFinisherFirstAttack = true;
				}
				else
				{
					characterState_.isHitTCHighPunch = true;
				}
			}
			//超必(2段目)
			else if (player_->GetIsFinisherSecondAttack() && player_->GetIsAttack())
			{
				//サウンド再生
				audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);

				//ダメージの適応
				ApplyDamage();

				//ダウン状態を設定
				characterState_.isHitFinisherSecondAttack = true;
			}
		}

		//空中にいる場合
		const float kEnemyPositionY = 0.5f;
		if (player_->GetIsTackle() && player_->GetIsAttack() && !characterState_.isGuard)
		{
			if (characterState_.isDown && worldTransform_.translation.y > kEnemyPositionY && !isCancel_)
			{
				//キャンセルのとき
				const float kHitStop = 0.3f;
				isCancel_ = true;
				attackData_.isDamaged = false;
				attackData_.isFinisherGaugeIncreased = false;

				//サウンド再生
				audio_->PlaySoundMP3(damageSoundHandle_, false, volume_);

				//ダメージの適応
				ApplyDamage();

				//ゲージ増加
				AdjustFinisherGauge(player_->GetFinisherGaugeIncreaseAmount());

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
}

void Enemy::UpdateAnimationTime(float animationTime, bool isLoop, float frameRate,
	int animationIndex, std::unique_ptr<Model>& modelFighterBody)
{
	//アニメーションの再生
	ICharacter::UpdateAnimationTime(animationTime, isLoop, frameRate, animationIndex, modelFighterBody);
}

void Enemy::Move()
{
	//キャラクター同士の距離を計算
	Vector3 playerWorldPosition = player_->GetWorldPosition();
	Vector3 enemyWorldPosition = GetWorldPosition();
	Vector3 difference = playerWorldPosition - enemyWorldPosition;
	float distance = Length(difference);
	const float kDistance = 4.0f;

	//行動タイマー
	const int kMinMoveTimer = 30;
	const int kMaxMoveTimer = 60;

	//移動処理
	if (patternCount_ == kPatternCount_[1] && !characterState_.isDown)
	{
		moveTimer_--;

		//移動方向のフラグ
		bool isFrontMove_ = false;
		bool isBackMove_ = false;

		//アニメーション
		const int kAnimationFrontMove = 0;
		const float animationSpeed = 1.5f;

		//速度
		const float koveSpeed = 0.01f;
		moveData_.velocity = { 0.0f, 0.0f, 0.0f };

		animationIndex_ = kAnimationFrontMove;
		isFrontMove_ = false;
		isBackMove_ = true;
		characterState_.isGuard = true;

		//向きに応じて処理を変える
		if (characterState_.direction == Direction::Right)
		{
			moveData_.velocity.x = -koveSpeed;
		}
		else
		{
			moveData_.velocity.x = koveSpeed;
		}

		//後ろ方向に移動
		if (isBackMove_)
		{
			//アニメーション
			UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, model_);

			//移動処理
			moveData_.velocity = Normalize(moveData_.velocity);
			moveData_.velocity = Multiply(backSpeed_, moveData_.velocity);
			worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

			//WorldTransformの更新
			worldTransform_.UpdateMatrixEuler();
		}

		//移動後の行動パターンの設定
		if (moveTimer_ <= 0)
		{
			if (distance >= kDistance)
			{
				moveTimer_ = Random(kMinMoveTimer, kMaxMoveTimer);
				patternCount_ = RandomBulletOrMove();
			}
			else
			{
				moveTimer_ = Random(kMinMoveTimer, kMaxMoveTimer);
				patternCount_ = RandomAttackOrMove();
			}
		}

		if (characterState_.isHitCharacter && playerWorldPosition.y <= 0.0f)
		{
			moveTimer_ = Random(kMinMoveTimer, kMaxMoveTimer);
			patternCount_ = kPatternCount_[5];
		}
	}

	//移動処理(前歩き)
	if (patternCount_ == kPatternCount_[2] && !characterState_.isDown)
	{
		moveTimer_--;

		//移動方向のフラグ
		bool isFrontMove_ = false;
		bool isBackMove_ = false;

		//アニメーション
		const int kAnimationBackMove = 2;
		const float animationSpeed = 1.5f;

		//速度
		const float koveSpeed = 0.01f;
		moveData_.velocity = { 0.0f, 0.0f, 0.0f };

		animationIndex_ = kAnimationBackMove;
		isFrontMove_ = true;
		isBackMove_ = false;
		characterState_.isGuard = false;

		//向きに応じて処理を変える
		if (characterState_.direction == Direction::Right)
		{
			moveData_.velocity.x = koveSpeed;
		}
		else
		{
			moveData_.velocity.x = -koveSpeed;
		}

		//前方向に移動
		if (isFrontMove_)
		{
			//アニメーション
			UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, model_);

			//移動処理
			moveData_.velocity = Normalize(moveData_.velocity);
			moveData_.velocity = Multiply(frontSpeed_, moveData_.velocity);
			worldTransform_.translation = Add(worldTransform_.translation, moveData_.velocity);

			//WorldTransformの更新
			worldTransform_.UpdateMatrixEuler();
		}

		//移動後の行動パターンの設定
		if (moveTimer_ <= 0)
		{
			if (distance >= kDistance)
			{
				moveTimer_ = Random(kMinMoveTimer, kMaxMoveTimer);
				patternCount_ = RandomBulletOrMove();
			}
			else
			{
				moveTimer_ = Random(kMinMoveTimer, kMaxMoveTimer);
				patternCount_ = RandomAttackOrMove();
			}
		}

		if (characterState_.isHitCharacter && playerWorldPosition.y <= 0.0f)
		{
			moveTimer_ = Random(kMinMoveTimer, kMaxMoveTimer);
			patternCount_ = kPatternCount_[5];
		}
	}

	//ガード
	if (player_->GetIsAttack() && player_->GetIsTackle() && !characterState_.isDown)
	{
		isGuardMode_ = true;
	}

	//ダウン状態になった場合
	if (characterState_.isDown)
	{
		isGuardMode_ = false;
		characterState_.isGuard = false;
	}

	//ガード状態
	if (isGuardMode_)
	{
		//行動パターンの設定
		patternCount_ = kPatternCount_[6];
		characterState_.isGuard = true;

		//アニメーション
		const int kAnimationGuard = 13;
		const float animationSpeed = 1.5f;

		animationIndex_ = kAnimationGuard;
		UpdateAnimationTime(animationTime_, true, animationSpeed, animationIndex_, model_);

		//当たり判定の設定
		const AABB kDownAABB = { {-0.05f,0.0f,-0.3f},{0.05f,1.0f,0.3f} };

		aabb_ = kDownAABB;
		SetAABB(aabb_);

		//確定反撃
		if (!player_->GetIsAttack())
		{
			const int kGuardTimer = 4;
			guardTimer_--;

			if (guardTimer_ < 0)
			{
				guardTimer_ = kGuardTimer;
				isGuardMode_ = false;
				characterState_.isGuard = false;
				moveTimer_ = Random(kMinMoveTimer, kMaxMoveTimer);
				patternCount_ = kPatternCount_[2];
			}
		}
	}
}

void Enemy::StartAttack(bool& isAttackType)
{
	//攻撃の開始処理
	ICharacter::StartAttack(isAttackType);
}

void Enemy::EndAttack(bool& isAttackType)
{
	//攻撃の終了処理
	player_->SetIsGuarded(false);
	ICharacter::EndAttack(isAttackType);
}

void Enemy::EvaluateAttackTiming()
{
	//攻撃判定をつけるタイミングの設定
	ICharacter::EvaluateAttackTiming();
}

void Enemy::ApplyDamage()
{
	//ダメージの適応
	if (!attackData_.isDamaged)
	{
		attackData_.isDamaged = true;
		hp_ -= player_->GetDamage();
	}
}

void Enemy::ResetCollision()
{
	//当たり判定のリセット
	aabb_ = defaultCollsiion_;
	SetAABB(aabb_);
}

void Enemy::ConfigureCollision(Vector3 min, Vector3 max)
{
	//当たり判定の設定
	aabb_ = { {min.x, min.y, min.z},{max.x, max.y, max.z} };
	SetAABB(aabb_);
}

void Enemy::UpdateHPBar()
{
	//体力ゲージ
	const float kHpBarSizeY = 7.0f;
	const int kDivisionFactor = 2;
	const int kHalfHp = maxHp_ / kDivisionFactor;
	const int kQuarterHp = kHalfHp / kDivisionFactor;

	//色
	const Vector4 kDefaultHpColor = { 0.0f, 1.0f, 0.0f, 1.0f };
	const Vector4 kHalfHpColor = { 1.0f, 0.8f, 0.0f, 1.0f };
	const Vector4 kQuarterHpColor = { 1.0f, 0.0f, 0.0f, 1.0f };

	//サイズを設定
	hpBar_.size_ = { (static_cast<float>(hp_) / static_cast<float>(maxHp_)) * barSize_, kHpBarSizeY };
	hpBar_.sprite_->SetSize(hpBar_.size_);

	//体力に応じて色を変化
	if (hp_ > kHalfHp)
	{
		hpBar_.sprite_->SetColor(kDefaultHpColor);
	}

	if (hp_ <= kHalfHp && hp_ > kQuarterHp)
	{
		hpBar_.sprite_->SetColor(kHalfHpColor);
	}
	else if (hp_ <= kQuarterHp)
	{
		hpBar_.sprite_->SetColor(kQuarterHpColor);
	}
}

void Enemy::UpdateGuardGaugeBar()
{
	//ガードゲージ
	const float kGuradGaugeBarSizeY = 7.0f;
	const float kGuardGaugeIncreaseSpeed = 0.03f;
	const float kMaxGuardGauge = 50.0f;

	//色
	const Vector4 kDefaultGuardGaugeColor = { 0.0f, 0.5f, 1.0f, 1.0f };

	if (guardGauge_ > 0 && guardGauge_ < kMaxGuardGauge)
	{
		guardGauge_ -= kGuardGaugeIncreaseSpeed;
	}

	//サイズを設定
	guardGaugeBar_.size_ = { (guardGauge_ / maxGuardGauge_) * guardGaugeBarSize_,kGuradGaugeBarSizeY };
	guardGaugeBar_.sprite_->SetSize(guardGaugeBar_.size_);

	//色を設定
	guardGaugeBar_.sprite_->SetColor(kDefaultGuardGaugeColor);

	//ガードゲージが最大になった場合
	if (guardGauge_ >= kMaxGuardGauge)
	{
		guardGauge_ = kMaxGuardGauge;
		characterState_.isGuard = false;
		attackData_.isAttack = false;
		characterState_.behaviorRequest = Behavior::kStan;
	}
}

void Enemy::AdjustGuardGauge()
{
	//攻撃パラメータに設定されているデータを適応
	const float kMaxGuardGauge = -50.0f;

	if (!attackData_.isGuarded)
	{
		if (finisherGauge_ < kMaxGuardGauge)
		{
			guardGauge_ += player_->GetGuardGaugeIncreaseAmount();
		}

		attackData_.isGuarded = true;
	}
}

void Enemy::UpdateFinisherGaugeBar()
{
	//必殺技ゲージ
	const float kFinisherGaugeBarSizeY = 19.3f;
	const float kMaxFinisherGauge = 50.0f;

	//色
	const Vector4 kDefaultHpColor = { 0.0f, 0.5f, 1.0f, 1.0f };
	const Vector4 kMaxChargeColor = { 1.0f, 0.5f, 0.0f, 1.0f };

	//サイズを設定
	finisherGaugeBar_.size_ = { (finisherGauge_ / maxFinisherGauge_) * finisherGaugeBarSize_,kFinisherGaugeBarSizeY };
	finisherGaugeBar_.sprite_->SetSize(finisherGaugeBar_.size_);

	if (finisherGauge_ < kMaxFinisherGauge)
	{
		finisherGaugeBar_.sprite_->SetColor(kDefaultHpColor);
	}
	else
	{
		finisherGaugeBar_.sprite_->SetColor(kMaxChargeColor);
	}

	if (finisherGauge_ >= kMaxFinisherGauge)
	{
		finisherGauge_ = kMaxFinisherGauge;
	}
}

void Enemy::AdjustFinisherGauge(float value)
{
	//プレイヤーの必殺技ゲージを取得
	float finisherGaugePlayer = player_->GetFinisherGauge();
	const float kPlayerMaxFinisherGauge = -50.0f;

	//攻撃パラメータに設定されているデータを適応
	const float kMaxFinisherGauge = 50.0f;
	if (!attackData_.isFinisherGaugeIncreased)
	{
		if (finisherGauge_ < kMaxFinisherGauge)
		{
			finisherGauge_ += value * attackData_.takeFinisherGaugeIncreaseAmount;
		}

		if (finisherGaugePlayer > kPlayerMaxFinisherGauge)
		{
			finisherGaugePlayer -= value;
		}

		attackData_.isFinisherGaugeIncreased = true;
	}

	//最大値に設定
	if (finisherGauge_ > kMaxFinisherGauge)
	{
		finisherGauge_ = kMaxFinisherGauge;
	}

	if (finisherGaugePlayer < kPlayerMaxFinisherGauge)
	{
		finisherGaugePlayer = kPlayerMaxFinisherGauge;
	}

	player_->SetFinisherGauge(finisherGaugePlayer);
}

void Enemy::Reset()
{
	const int kMaxHp = 100;

	//リセット
	ICharacter::Reset();

	//HPの設定
	hp_ = kMaxHp;

	//パターンの初期化
	patternCount_ = RandomMove();

	//アニメーション
	const int kAnimationIdle = 5;
	animationIndex_ = kAnimationIdle;
	animationTime_ = 0.0f;
	model_->SetAnimationTime(animationTime_);

	//WorldTransformの設定
	const float kDefaultTranslationX = 1.5f;
	worldTransform_.translation = { kDefaultTranslationX,0.0f,0.0f };
	worldTransform_.rotation = { 0.0f,characterState_.leftDirectionRotation,0.0f };
	characterState_.direction = Direction::Left;

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();

	//フラグのリセット
	isCancel_ = false;
	isHitAudio_ = false;
	isKO_ = false;
}

void Enemy::DownAnimation()
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
		if (!player_->GetIsLightPunch() && hp_ > 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitLightPunch);

			patternCount_ = RandomMove();
			isHitAudio_ = false;
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
		const int kAnimationHeavyDown = 6;
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
		const AABB kDownAABB = (characterState_.direction == Direction::Right) ? AABB{ {-1.1f, 0.0f, -0.3f}, {-0.1f, 0.2f, 0.3f} } :
			AABB{ {0.1f, 0.0f, -0.3f}, {1.1f, 0.2f, 0.3f} };

		aabb_ = kDownAABB;
		SetAABB(aabb_);

		//移動処理
		const int kJumpTime = 55;
		const int kFallTime = -30;
		const float kJumpFirstSpeed_ = 0.15f;
		const float kMoveSpeed = 0.025f;
		float moveX = (player_->GetDirection() == Direction::Right) ? kMoveSpeed : -kMoveSpeed;

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
		if (!player_->GetIsHighPunch() && worldTransform_.translation.y <= 0.0f && hp_ > 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitHighPunch);
			ResetCollision();
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
		if (!player_->GetIsTCMiddlePunch() && hp_ > 0)
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
		const int kAnimationHeavyDown = 6;
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
		SetAABB(aabb_);

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
		if (!player_->GetIsTCHighPunch() && hp_ > 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitTCHighPunch);
			ResetCollision();
			isKO_ = false;
		}
	}

	//ジャンプ攻撃
	if (characterState_.isHitJumpAttack)
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
		const int kJumpAttaackEndTime = 35;
		if (timerData_.downAnimationTimer < kJumpAttaackEndTime && hp_ > 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitJumpAttack);
			isKO_ = false;
		}
	}

	//弾攻撃(地上)
	if (characterState_.isHitBullet)
	{
		//ダウン状態に設定
		const int kDownTime = 20;
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
		if (timerData_.downAnimationTimer < kDownTime && hp_ > 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitBullet);
			ResetCollision();

			patternCount_ = RandomMove();
			isParticle_ = false;
			isKO_ = false;
		}
	}
	//弾攻撃(空中)
	else if (characterState_.isHitAirBullet)
	{
		//ダウン状態に設定
		const int kDownTime = 20;
		characterState_.isDown = true;
		timerData_.downAnimationTimer--;

		//アニメーション
		const int kAnimationHeavyDown = 6;
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
		const AABB kDownAABB = (player_->GetDirection() == Direction::Right) ? AABB{ {0.1f, 0.0f, -0.3f}, {1.1f, 0.2f, 0.3f} } :
			AABB{ {-1.1f, 0.0f, -0.3f}, {-0.1f, 0.2f, 0.3f} };

		aabb_ = kDownAABB;
		SetAABB(aabb_);

		//移動処理
		const int kMoveTime = 35;
		const float kMoveSpeed = player_->GetDirection() == Direction::Right ? -0.08f : 0.08f;
		const float kFallSpeed = 0.03f;
		float moveX = kMoveSpeed;
		float rotationY = player_->GetDirection() == Direction::Right ? characterState_.leftDirectionRotation : characterState_.rightDirectionRotation;

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
		if (timerData_.downAnimationTimer < kDownTime && hp_ > 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitAirBullet);
			ResetCollision();

			isParticle_ = false;
			isKO_ = false;
		}
	}

	//タックル攻撃
	if (characterState_.isHitTackle)
	{
		//ダウン状態に設定
		characterState_.isDown = true;
		timerData_.downAnimationTimer -= static_cast<int>(GameTimer::GetDeltaTime() * kScaleFacter_);
		timerData_.effectTimer--;

		//アニメーション
		const int kAnimationHeavyDown = 6;
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
		const AABB kDownAABB = (player_->GetDirection() == Direction::Right) ? AABB{ {0.1f, 0.0f, -0.3f}, {1.1f, 0.2f, 0.3f} } :
			AABB{ {-1.1f, 0.0f, -0.3f}, {-0.1f, 0.2f, 0.3f} };

		aabb_ = kDownAABB;
		SetAABB(aabb_);

		//移動処理
		const int kMoveTime = 35;
		const float kComboMoveSpeed = 6.0f;
		const float kDefaultMoveSpeed = 4.8f;
		const float kTradeMoveSpeed = 2.0f;
		const float kFallSpeed = 1.8f;
		float moveX = 0.0f;

		//相手かダウン状態かどうかで速さを変化
		if (!player_->GetIsDown())
		{
			if (comboCount_ >= kComboCount_[5])
			{
				moveX = (player_->GetDirection() == Direction::Right) ? kComboMoveSpeed : -kComboMoveSpeed;
			}
			else
			{
				moveX = (player_->GetDirection() == Direction::Right) ? kDefaultMoveSpeed : -kDefaultMoveSpeed;
			}
		}
		else
		{
			moveX = (player_->GetDirection() == Direction::Right) ? kTradeMoveSpeed : -kTradeMoveSpeed;
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
		if (timerData_.downAnimationTimer < 0 && hp_ > 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitTackle);
			isKO_ = false;
			isCancel_ = false;
			ResetCollision();
		}
	}

	//アッパー攻撃
	if (characterState_.isHitUppercut)
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
		if (!player_->GetIsUppercut() && hp_ > 0)
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitUppercut);
			isKO_ = false;
		}
	}

	//必殺技(1段目)
	if (characterState_.isHitFinisherFirstAttack)
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
		if (!player_->GetIsFinisherFirstAttack())
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitFinisherFirstAttack);
			isKO_ = false;
		}
	}

	//必殺技(2段目)
	if (characterState_.isHitFinisherSecondAttack)
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
		const int kParticleTime[5] = { 55, 50, 45, 40, 35 };
		const float kParticleMoveSpeed = 0.1f;
		float particlePositionX = (characterState_.direction == Direction::Right) ? -kParticleMoveSpeed : kParticleMoveSpeed;
		const float particlePositionY = 0.5f;

		//最初の攻撃時
		if (timerData_.downAnimationTimer > kParticleTime[0])
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePositionX,
				 worldTransform_.translation.y + particlePositionY,worldTransform_.translation.z });
		}
		else if(hp_ <= 0)
		{
			isKO_ = true;
		}

		//2発目の攻撃時
		if (timerData_.downAnimationTimer < kParticleTime[1] && timerData_.downAnimationTimer > kParticleTime[2])
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePositionX,
				 worldTransform_.translation.y + particlePositionY,worldTransform_.translation.z });


			//サウンド再生
			audio_->PlaySoundMP3(damageSoundHandle_, false, 1.0f);

			//体力を減らす
			const int dmage = 1;
			hp_ -= dmage;
		}

		//3発目の攻撃時
		if (timerData_.downAnimationTimer < kParticleTime[3] && timerData_.downAnimationTimer > kParticleTime[4])
		{
			particleEffectPlayer_->PlayParticle("Hit", { worldTransform_.translation.x + particlePositionX,
				 worldTransform_.translation.y + particlePositionY,worldTransform_.translation.z });

			//サウンド再生
			audio_->PlaySoundMP3(damageSoundHandle_, false, 1.0f);

			//体力を減らす
			const int dmage = 1;
			hp_ -= dmage;
		}

		//終了処理
		if (!player_->GetIsFinisherSecondAttack())
		{
			//アニメーションの設定
			const int kAnimationIdle = 5;
			EndDownAnimation(kAnimationIdle, characterState_.isHitFinisherSecondAttack);
			isKO_ = false;
		}
	}

	//KOの場合
	const int kKOTime = 50;
	if (timerData_.downAnimationTimer < kKOTime && hp_ <= 0)
	{
		isKO_ = true;
	}
}

void Enemy::EndDownAnimation(int animationIndex, bool& isHitAttackType)
{
	//ダウンアニメーションの終了処理
	ICharacter::EndDownAnimation(animationIndex, isHitAttackType);
}

void Enemy::UpdateComboNumberSprite()
{
	//コンボ表示の更新
	int comboNum = comboCount_;

	comboNumTextureHandle_ = TextureManager::LoadTexture("resource/number/" + std::to_string(comboNum) + ".png");

	comboNumSprite_->SetTexture(comboNumTextureHandle_);
}

int Enemy::Random(int min_value, int max_value)
{
	//乱数を生成
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(min_value, max_value);

	return dis(gen);
}

int Enemy::RandomMove()
{
	std::vector<int> actions;

	//前歩きか後ろ歩きをランダムで設定
	actions = { kPatternCount_[1], kPatternCount_[2], kPatternCount_[2] };

	//乱数を生成
	const int kIndexOffset = 1;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, static_cast<int>(actions.size()) - kIndexOffset);

	return actions[dis(gen)]; 
}

int Enemy::RandomAttackOrMove()
{
	std::vector<int> actions;

	//体力の半分
	const int kDivisionFactor = 2;
	const int kHalfHP = maxHp_ / kDivisionFactor;

	//体力に応じて行動をランダムで設定
	if (hp_ >= kHalfHP)
	{
		//前歩きか突進攻撃
		actions = { kPatternCount_[2], kPatternCount_[2], kPatternCount_[3] };
	}
	else
	{
		//前歩きか後ろ歩きか弾攻撃
		actions = { kPatternCount_[1], kPatternCount_[2], kPatternCount_[4] };
	}

	//乱数を生成
	const int kIndexOffset = 1;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, static_cast<int>(actions.size()) - kIndexOffset);

	return actions[dis(gen)];
}

int Enemy::RandomBulletOrMove()
{
	std::vector<int> actions;

	//前歩きか後ろ歩きか弾攻撃
	actions = { kPatternCount_[1], kPatternCount_[2], kPatternCount_[4] };

	//乱数を生成
	const int kIndexOffset = 1;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, static_cast<int>(actions.size()) - kIndexOffset);

	return actions[dis(gen)];
}

void Enemy::ShootBullet(const Vector3& startPosition, const Vector3& velocity)
{
	//弾を生成してリストに追加する
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(bulletModel_.get(), startPosition, velocity);
	bullets_.push_back(newBullet);
}

void Enemy::UpdateBullets()
{
	//弾の更新と衝突判定などを行う
	for (auto it = bullets_.begin(); it != bullets_.end();)
	{
		(*it)->Update();
		if ((*it)->GetIsDead())
		{
			delete* it;
			it = bullets_.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void Enemy::HitCombo()
{
	//攻撃ごとの硬直
	//弱パンチ
	const int kLightPunchRecoveryTime = 15;

	//強パンチ
	const int kHighPunchRecoveryTime = 60;

	//TC中パンチ
	const int kTCMiddlePunchRecoveryTime = 30;

	//TC強パンチ
	const int kTCHighPunchRecoveryTime = 45;

	//ジャンプ攻撃
	const int kJumpAttackRecoveryTime = 40;

	//アッパー
	const int kUpperCutRecoveryTime = 40;

	//タックル
	const int kTackleRecoveryTime = 40;

	//必殺技
	const int kFinisherFirstAttackRecoveryTime = 120;
	const int kFinisherSecondAttackRecoveryTime = 240;
	const int kFinisherThreeAttackRecoveryTime = 230;
	const int kFinisherFourAttackRecoveryTime = 220;
	const int kFinisherFiveAttackRecoveryTime = 210;

	//TODO:冗長的なコードなので改善する
	//コンボを食らっているとき
	//始動技がジャンプ攻撃の場合
	if (characterState_.isHitJumpAttack && comboCount_ == 0)
	{
		firstAttack_ = "JumpAttack";
		comboCount_ = kComboCount_[1];
		timerData_.comboTimer = kJumpAttackRecoveryTime;
		timerData_.comboTimer--;
	}

	if (firstAttack_ == "JumpAttack")
	{
		//弱パンチ
		if (characterState_.isHitLightPunch && comboCount_ == kComboCount_[1])
		{
			comboCount_ = kComboCount_[2];
			timerData_.comboTimer = kLightPunchRecoveryTime;
			timerData_.comboTimer--;
		}

		//TC中パンチ
		if (characterState_.isHitTCMiddlePunch && comboCount_ == kComboCount_[2])
		{
			comboCount_ = kComboCount_[3];
			timerData_.comboTimer = kTCMiddlePunchRecoveryTime;
			timerData_.comboTimer--;
		}

		//アッパー
		if (characterState_.isHitUppercut && comboCount_ == kComboCount_[3])
		{
			comboCount_ = kComboCount_[4];
			timerData_.comboTimer = kUpperCutRecoveryTime;
			timerData_.comboTimer--;
		}

		//必殺技(1段目)
		if (characterState_.isHitFinisherFirstAttack && comboCount_ == kComboCount_[4])
		{
			comboCount_ = kComboCount_[5];
			timerData_.comboTimer = kFinisherFirstAttackRecoveryTime;
			timerData_.comboTimer--;
		}

		//必殺技(2段目)
		if (characterState_.isHitFinisherSecondAttack)
		{
			if (comboCount_ == kComboCount_[5])
			{
				timerData_.comboTimer = kFinisherSecondAttackRecoveryTime;
			}

			if (timerData_.comboTimer > 0)
			{
				timerData_.comboTimer--;

				if (timerData_.comboTimer > kFinisherThreeAttackRecoveryTime)
				{
					comboCount_ = kComboCount_[6];
				}
				else if (timerData_.comboTimer > kFinisherFourAttackRecoveryTime)
				{
					comboCount_ = kComboCount_[7];
				}
				else if (timerData_.comboTimer > kFinisherFiveAttackRecoveryTime)
				{
					comboCount_ = kComboCount_[8];
				}
			}
		}

		//タックル
		if (characterState_.isHitTackle && comboCount_ == kComboCount_[8])
		{
			comboCount_ = kComboCount_[9];
			timerData_.comboTimer = kTackleRecoveryTime;
			timerData_.comboTimer--;
		}

		//TC強パンチ
		if (characterState_.isHitTCHighPunch && comboCount_ == kComboCount_[3])
		{
			comboCount_ = kComboCount_[4];
			timerData_.comboTimer = kTCHighPunchRecoveryTime;
			timerData_.comboTimer--;
		}

		//強パンチ
		if (characterState_.isHitHighPunch && comboCount_ == kComboCount_[3])
		{
			comboCount_ = kComboCount_[4];
			timerData_.comboTimer = kHighPunchRecoveryTime;
			timerData_.comboTimer--;
		}

		//TCタックル
		if (characterState_.isHitTackle && comboCount_ == kComboCount_[4])
		{
			comboCount_ = kComboCount_[5];
			timerData_.comboTimer = kHighPunchRecoveryTime;
			timerData_.comboTimer--;
		}
	}

	//始動技が弱パンチの場合
	if (characterState_.isHitLightPunch && comboCount_ == 0)
	{
		firstAttack_ = "LightPunch";
		comboCount_ = kComboCount_[1];
		timerData_.comboTimer = kLightPunchRecoveryTime;
		timerData_.comboTimer--;
	}

	if (firstAttack_ == "LightPunch")
	{
		//TC中パンチ
		if (characterState_.isHitTCMiddlePunch && comboCount_ == kComboCount_[1])
		{
			comboCount_ = kComboCount_[2];
			timerData_.comboTimer = kTCMiddlePunchRecoveryTime;
			timerData_.comboTimer--;
		}

		//TC強パンチ
		if (characterState_.isHitTCHighPunch && comboCount_ == kComboCount_[2])
		{
			comboCount_ = kComboCount_[3];
			timerData_.comboTimer = kTCHighPunchRecoveryTime;
			timerData_.comboTimer--;
		}

		//アッパー
		if (characterState_.isHitUppercut && comboCount_ == kComboCount_[2])
		{
			comboCount_ = kComboCount_[3];
			timerData_.comboTimer = kUpperCutRecoveryTime;
			timerData_.comboTimer--;
		}

		//必殺技(1段目)
		if (characterState_.isHitFinisherFirstAttack && comboCount_ == kComboCount_[3])
		{
			comboCount_ = kComboCount_[4];
			timerData_.comboTimer = kFinisherFirstAttackRecoveryTime;
			timerData_.comboTimer--;
		}

		//必殺技(2段目)
		if (characterState_.isHitFinisherSecondAttack)
		{
			if (comboCount_ == kComboCount_[4])
			{
				timerData_.comboTimer = kFinisherSecondAttackRecoveryTime;
			}

			if (timerData_.comboTimer > 0)
			{	
				timerData_.comboTimer--;

				if (timerData_.comboTimer > kFinisherThreeAttackRecoveryTime)
				{
					comboCount_ = kComboCount_[5];
				}
				else if (timerData_.comboTimer > kFinisherFourAttackRecoveryTime)
				{
					comboCount_ = kComboCount_[6];
				}
				else if (timerData_.comboTimer > kFinisherFiveAttackRecoveryTime)
				{
					comboCount_ = kComboCount_[7];
				}
			}
		}

		//タックル
		if (characterState_.isHitTackle && comboCount_ == kComboCount_[7])
		{
			comboCount_ = kComboCount_[8];
			timerData_.comboTimer = kTackleRecoveryTime;
			timerData_.comboTimer--;
		}

		//強攻撃
		if (characterState_.isHitHighPunch && comboCount_ == kComboCount_[2])
		{
			comboCount_ = kComboCount_[3];
			timerData_.comboTimer = kHighPunchRecoveryTime;
			timerData_.comboTimer--;
		}

		//タックル
		if (characterState_.isHitTackle && comboCount_ == kComboCount_[3])
		{
			comboCount_ = kComboCount_[4];
			timerData_.comboTimer = kTackleRecoveryTime;
			timerData_.comboTimer--;
		}
	}

	//始動技が必殺技の場合
	if (characterState_.isHitFinisherFirstAttack && comboCount_ == 0)
	{
		firstAttack_ = "FinisherFirstAttack";
		comboCount_ = kComboCount_[1];
		timerData_.comboTimer = kFinisherSecondAttackRecoveryTime;
		timerData_.comboTimer--;
	}

	//必殺技(2段目)
	if (firstAttack_ == "FinisherFirstAttack")
	{
		if (characterState_.isHitFinisherSecondAttack)
		{
			if (comboCount_ == kComboCount_[1])
			{
				timerData_.comboTimer = kFinisherSecondAttackRecoveryTime;
			}

			if (timerData_.comboTimer > 0)
			{
				timerData_.comboTimer--;

				if (timerData_.comboTimer > kFinisherThreeAttackRecoveryTime)
				{
					comboCount_ = kComboCount_[2];
				}
				else if (timerData_.comboTimer > kFinisherFourAttackRecoveryTime)
				{
					comboCount_ = kComboCount_[3];
				}
				else if (timerData_.comboTimer > kFinisherFiveAttackRecoveryTime)
				{
					comboCount_ = kComboCount_[4];
				}
			}
		}

		//タックル
		if (characterState_.isHitTackle && comboCount_ == kComboCount_[4])
		{
			comboCount_ = kComboCount_[5];
			timerData_.comboTimer = kTackleRecoveryTime;
			timerData_.comboTimer--;
		}
	}

	//相手の硬直中に書道の攻撃を当てた場合
	if (comboCount_ >= kComboCount_[3])
	{
		if (characterState_.isHitJumpAttack)
		{
			firstAttack_ = "JumpAttack";
			comboCount_ = kComboCount_[1];
			timerData_.comboTimer = kJumpAttackRecoveryTime;
			timerData_.comboTimer--;
		}

		if (characterState_.isHitLightPunch)
		{
			firstAttack_ = "LightPunch";
			comboCount_ = kComboCount_[1];
			timerData_.comboTimer = kLightPunchRecoveryTime;
			timerData_.comboTimer--;
		}
	}

	//コンボタイマーを減らす
	if (timerData_.comboTimer >= 0 && !player_->GetIsFinisher())
	{
		timerData_.comboTimer--;
	}

	//終了処理
	if (timerData_.comboTimer < 0 || player_->GetIsDown())
	{
		timerData_.comboTimer = 0;
		comboCount_ = 0;
		firstAttack_ = "";
	}
}

Vector3 Enemy::GetWorldPosition()
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld.m[3][0];
	pos.y = worldTransform_.matWorld.m[3][1];
	pos.z = worldTransform_.matWorld.m[3][2];
	return pos;
}
