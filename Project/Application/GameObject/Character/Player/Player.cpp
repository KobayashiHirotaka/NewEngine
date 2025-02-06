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
#include <cassert>

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
	currentState_ = std::make_unique<PlayerRootState>();
	currentState_->SetPlayer(this);
	currentState_->Initialize();

	//UI生成、初期化
	playerUI_ = std::make_unique<PlayerUI>();
	playerUI_->SetPlayer(this);
	playerUI_->Initialize();

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

#endif

	//更新
	BaseCharacter::Update();

	//エディターで設定したパラメータをセット
	AttackEditor::GetInstance()->SetAttackParameters(attackType_, attackData_.attackStartTime, attackData_.attackEndTime, attackData_.recoveryTime,
		attackData_.cancelStartTime, attackData_.cancelEndTime,attackData_.damage, attackData_.hitRecoveryTime, attackData_.guardGaugeIncreaseAmount,
		attackData_.finisherGaugeIncreaseAmount, attackData_.hitStop, aabb_, true, characterState_.direction);

	if (nextState_)
	{
		//Stateの生成
		nextState_->SetPlayer(this);
		nextState_->Initialize();

		//State切り替え
		currentState_ = std::move(nextState_);
	}

	//Stateの更新
	currentState_->Update();

	//振り向きの処理
	Vector3 playerWorldPosition = worldTransform_.translation;
	Vector3 enemyWorldPosition = enemy_->GetWorldPosition();

	if (enemyWorldPosition.x > playerWorldPosition.x && !characterState_.isDown)
	{
		characterState_.direction = Direction::Right;
		worldTransform_.rotation.y = characterState_.rightDirectionRotation;
		isDirectionRight_ = true;
	}

	if (enemyWorldPosition.x < playerWorldPosition.x && !characterState_.isDown)
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

	//UIの更新
	playerUI_->Update();

	//コンボ関連の処理
	HitCombo();

	//ガードアニメーションタイマーのリセット
	if (!enemy_->GetAttackData().isGuarded)
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
	//UIの描画
	playerUI_->Draw();
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

void Player::Move(const Vector3 velocity)
{
	//移動
	worldTransform_.translation = Add(worldTransform_.translation, velocity);

	//WorldTransformの更新
	worldTransform_.UpdateMatrixEuler();
}

void Player::ChangeState(std::unique_ptr<PlayerBaseState> state)
{
	nextState_ = std::move(state);
}

void Player::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemy)
	{
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
	}
}

void Player::ShootBullet(const Vector3& startPosition, const Vector3& velocity)
{
	//弾を生成し、リストに追加する
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

void Player::ApplyDamage()
{
	//ダメージの適応
	if (!attackData_.isDamaged)
	{
		attackData_.isDamaged = true;
		baseData_.hp_ += enemy_->GetAttackData().damage;
	}
}

void Player::ResetCollision()
{
	//当たり判定のリセット
	aabb_ = defaultCollsiion_;
	collider_->SetAABB(aabb_);
}

void Player::Reset()
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
