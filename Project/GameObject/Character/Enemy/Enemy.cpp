#include "Enemy.h"
#include <cassert>
#include <numbers>
#include "Project/GameObject/Character/Player/Player.h"

Enemy::~Enemy()
{
	delete hpBar_.sprite_;
	delete guardGaugeBar_.sprite_;
}

void Enemy::Initialize()
{
	audio_ = Audio::GetInstance();

	modelFighterBody_.reset(Model::CreateFromOBJ("resource/float_Body", "float_Body.obj"));
	modelFighterPHead_.reset(Model::CreateFromOBJ("resource/float_PHead", "playerHead.obj"));
	modelFighterL_arm_.reset(Model::CreateFromOBJ("resource/float_L_arm", "float_L_arm.obj"));
	modelFighterR_arm_.reset(Model::CreateFromOBJ("resource/float_R_arm", "float_R_arm.obj"));

	hpBar_ = {
		true,
		TextureManager::LoadTexture("resource/HP.png"),
		{720.0f, barSpace},
		0.0f,
		{barSize  ,7.0f},
		nullptr,
	};

	hpBar_.sprite_ = Sprite::Create(hpBar_.textureHandle_, hpBar_.position_);

	guardGaugeBar_ = {
		true,
		TextureManager::LoadTexture("resource/HP.png"),
		{720.0f, guardGaugeBarSpace},
		0.0f,
		{guardGaugeBarSize  ,7.0f},
		nullptr,
	};

	guardGaugeBar_.sprite_ = Sprite::Create(guardGaugeBar_.textureHandle_, guardGaugeBar_.position_);

	worldTransform_.Initialize();
	worldTransform_.translation = { 7.0f,0.0f,0.0f };

	worldTransformHead_.Initialize();
	worldTransform_.rotation.y = 4.6f;

	worldTransformBody_.Initialize();
	worldTransformBody_.translation = { 0.0f,1.0f,0.0f };

	worldTransformL_arm_.Initialize();
	worldTransformL_arm_.translation.x = 0.5f;

	worldTransformR_arm_.Initialize();
	worldTransformR_arm_.translation.x = -0.5f;

	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;

	//Weaponの生成
	enemyWeapon_ = std::make_unique<EnemyWeapon>();
	enemyWeapon_->Initialize();
	enemyWeapon_->SetParent(&worldTransform_);

	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	particleModel_.reset(ParticleModel::CreateFromOBJ("resource/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();

	//WorldTransform(Player)の更新
	worldTransform_.UpdateMatrix();

	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

	attackSoundHandle_ = audio_->SoundLoadWave("resource/Sounds/Attack.wav");
	weaponAttackSoundHandle_ = audio_->SoundLoadWave("resource/Sounds/WeaponAttack.wav");
	damageSoundHandle_ = audio_->SoundLoadWave("resource/Sounds/Damage.wav");
	guardSoundHandle_ = audio_->SoundLoadWave("resource/Sounds/Guard.wav");
}

void Enemy::Update()
{
	isShake_ = false;

	if (Input::GetInstance()->PressKey(DIK_G) && guardGauge_ >= 0 && guardGauge_ < maxGuardGauge_)
	{
		guardGauge_ += 1.0f;
	}
	else if(guardGauge_ > 0)
	{
		guardGauge_ -= 1.0f;
	}

	if (guardGauge_ >= maxGuardGauge_)
	{
		behaviorRequest_ = Behavior::kStan;
	}

	if (isReset_)
	{
		resetTimer_--;

		if (resetTimer_ < 0)
		{
			isReset_ = false;
			resetTimer_ = 60;
		}
	}

	//EnemyのBehavior
	if (behaviorRequest_)
	{
		behavior_ = behaviorRequest_.value();

		switch (behavior_)
		{
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			FloatingGimmickInitialize();
			break;

		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;

		case Behavior::kJump:
			BehaviorJumpInitialize();
			break;

		case Behavior::kThrow:
			BehaviorThrowInitialize();
			break;

		case Behavior::kStan:
			BehaviorStanInitialize();
			break;
		}

		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_)
	{
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		FloatingGimmickUpdate();
		break;

	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;

	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;

	case Behavior::kThrow:
		BehaviorThrowUpdate();
		break;

	case Behavior::kStan:
		BehaviorStanUpdate();
		break;
	}

	if (worldTransform_.translation.x >= 12.0f)
	{
		worldTransform_.translation.x = 12.0f;
	}

	if (worldTransform_.translation.x <= -12.0f)
	{
		worldTransform_.translation.x = -12.0f;
	}

	if (behaviorRequest_ == Behavior::kJump && isHit_)
	{
		worldTransform_.translation.y = 0.0f;
	}

	DownAnimation();

	//Weaponの更新
	enemyWeapon_->Update();

	//パーティクルの更新
	particleSystem_->Update();

	worldTransform_.UpdateMatrix();

	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

	isHit_ = false;

	isPlayerHit_ = false;

	HPBarUpdate();

	GuardGaugeBarUpdate();

	ImGui::Begin("GuardGauge");
	ImGui::Text("GuardGauge %f", guardGauge_);
	ImGui::End();
}

void Enemy::Draw(const Camera& camera)
{
	//Enemyの描画
	modelFighterBody_->Draw(worldTransformBody_, camera);
	modelFighterPHead_->Draw(worldTransformHead_, camera);
	modelFighterL_arm_->Draw(worldTransformL_arm_, camera);
	modelFighterR_arm_->Draw(worldTransformR_arm_, camera);

	//Weaponの描画
	if (workAttack_.isSwingDown || workAttack_.isMowDown || workAttack_.isPoke && !isHitSwingDown_
		&& !isHitPoke_ && !isHitMowDown_ && !isDown_ && behaviorRequest_ != Behavior::kRoot)
	{
		enemyWeapon_->Draw(camera);
	}
}

void Enemy::DrawSprite()
{
	if (HP_ >= 0)
	{
		hpBar_.sprite_->Draw();
	}

	guardGaugeBar_.sprite_->Draw();
}

void Enemy::HPBarUpdate()
{
	hpBar_.size_ = { (HP_ / maxHP_) * barSize,7.0f };

	hpBar_.sprite_->SetSize(hpBar_.size_);
}

void Enemy::GuardGaugeBarUpdate()
{
	guardGaugeBar_.size_ = { (guardGauge_ / maxGuardGauge_) * guardGaugeBarSize,7.0f };

	guardGaugeBar_.sprite_->SetSize(guardGaugeBar_.size_);
}

void Enemy::Reset()
{
	HP_ = maxHP_;

	guardGauge_ = 0.0f;

	for (int i = 0; i < 6; i++)
	{
		downAnimationTimer_[i] = 60;
	}

	isReset_ = true;

	isHitPunch_ = false;
	isHitSwingDown_ = false;
	isHitPoke_ = false;
	isHitMowDown_ = false;
	isHitThrow_ = false;
	isDown_ = false;

	isHit_ = false;

	workAttack_.isAttack = false;
	workAttack_.isPunch = false;
	workAttack_.isSwingDown = false;
	workAttack_.isPoke = false;
	workAttack_.isPokeRight = false;
	workAttack_.isPokeLeft = false;
	workAttack_.isMowDown = false;
	workAttack_.isJumpAttack = false;

	isThrow_ = false;

	behavior_ = Behavior::kRoot;

	worldTransform_.Initialize();
	worldTransform_.translation = { 7.0f,0.0f,0.0f };

	worldTransformHead_.Initialize();
	worldTransformHead_.rotation.y = 0.0f;
	worldTransform_.rotation.y = 4.6f;

	worldTransformBody_.Initialize();
	worldTransformBody_.translation = { 0.0f,1.0f,0.0f };
	worldTransformBody_.rotation.x = 0.0f;
	worldTransformBody_.rotation.y = 0.0f;

	worldTransformL_arm_.Initialize();
	worldTransformL_arm_.translation.x = 0.5f;
	worldTransformL_arm_.rotation.x = 0.0f;
	worldTransformL_arm_.rotation.y = 0.0f;

	worldTransformR_arm_.Initialize();
	worldTransformR_arm_.translation.x = -0.5f;
	worldTransformR_arm_.rotation.x = 0.0f;
	worldTransformR_arm_.rotation.y = 0.0f;

	worldTransform_.UpdateMatrix();

	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

void Enemy::DrawParticle(const Camera& camera)
{
	if (!isReset_)
	{
		particleModel_->Draw(particleSystem_.get(), camera);
	}
}

void Enemy::OnCollision(Collider* collider, float damage)
{
	if (collider->GetCollisionAttribute() & kCollisionAttributePlayer)
	{
		isPlayerHit_ = true;

		isHit_ = true;

		if (player_->GetIsPunch() == true && isDown_ == false)
		{
			audio_->SoundPlayWave(damageSoundHandle_, false, 1.0f);
			damage = 8.0f;
			HP_ -= damage;
			isHitPunch_ = true;
		}

		if (player_->GetIsThrow() == true && isDown_ == false)
		{
			audio_->SoundPlayWave(damageSoundHandle_, false, 1.0f);
			damage = 20.0f;
			HP_ -= damage;
			isHitThrow_ = true;
		}
	}

	if (collider->GetCollisionAttribute() & kCollisionAttributePlayerWeapon)
	{
		if (isGuard_ && worldTransform_.rotation.y == 1.7f)
		{
			audio_->SoundPlayWave(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.3f;

			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation(worldTransform_.translation)
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.2f, 0.2f,0.2f }, { 0.6f ,0.6f ,0.6f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.06f ,0.06f ,0.06f }, { 0.1f ,0.1f ,0.1f })
				.SetColor({ 1.0f ,1.0f ,1.0f ,1.0f }, { 1.0f ,1.0f ,1.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(100)
				.SetFrequency(4.0f)
				.SetDeleteTime(2.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (isGuard_ && worldTransform_.rotation.y == 4.6f)
		{
			audio_->SoundPlayWave(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x += 0.3f;

			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation(worldTransform_.translation)
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.2f, 0.2f,0.2f }, { 0.6f ,0.6f ,0.6f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.06f ,0.06f ,0.06f }, { 0.1f ,0.1f ,0.1f })
				.SetColor({ 1.0f ,1.0f ,1.0f ,1.0f }, { 1.0f ,1.0f ,1.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(100)
				.SetFrequency(4.0f)
				.SetDeleteTime(2.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (player_->GetIsAttack() == true && player_->GetIsSwingDown() == true && isDown_ == false
			&& isGuard_ == false)
		{
			audio_->SoundPlayWave(damageSoundHandle_, false, 1.0f);
			damage = 20.0f;
			HP_ -= damage;
			isHitSwingDown_ = true;
			isShake_ = true;
		}

		if (player_->GetIsAttack() == true && player_->GetIsPoke() == true && isDown_ == false
			&& isGuard_ == false)
		{
			audio_->SoundPlayWave(damageSoundHandle_, false, 1.0f);
			damage = 15.0f;
			HP_ -= damage;
			isHitPoke_ = true;
			isShake_ = true;
		}

		if (player_->GetIsAttack() == true && player_->GetIsMowDown() == true && isDown_ == false
			&& isGuard_ == false)
		{
			audio_->SoundPlayWave(damageSoundHandle_, false, 1.0f);
			damage = 20.0f;
			HP_ -= damage;
			isHitMowDown_ = true;
			isShake_ = true;
		}
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

void Enemy::BehaviorRootInitialize()
{
	
}

void Enemy::BehaviorRootUpdate()
{
	patternCount_ = 1;
	//コントローラーの移動処理
	if (patternCount_ == 1  && isDown_ == false)
	{
		moveTimer_--;

		const float deadZone = 0.7f;
		bool isMove_ = false;
		float kCharacterSpeed = 0.1f;
		velocity_ = { 0.0f, 0.0f, 0.0f };

		//移動処理
		if (moveTimer_ > 30 && worldTransform_.rotation.y == 4.6f && !isHit_)
		{
			kCharacterSpeed = 0.1f;
			velocity_.x = -0.3f;
			isMove_ = true;
			isGuard_ = false;
		}

		if (moveTimer_ > 30 && worldTransform_.rotation.y == 1.7f && !isHit_)
		{
			kCharacterSpeed = 0.1f;
			velocity_.x = 0.3f;
			isMove_ = true;
			isGuard_ = false;
		}

		if (moveTimer_ <= 30 && worldTransform_.rotation.y == 1.7f)
		{
			kCharacterSpeed = 0.05f;
			velocity_.x = -0.3f;
			isMove_ = true;
			isGuard_ = true;
		}

		if (moveTimer_ <= 30 && worldTransform_.rotation.y == 4.6f)
		{
			kCharacterSpeed = 0.05f;
			velocity_.x = 0.3f;
			isMove_ = true;
			isGuard_ = true;
		}

		if (isMove_)
		{
			velocity_ = Normalize(velocity_);
			velocity_ = Multiply(kCharacterSpeed, velocity_);

			// 平行移動
			worldTransform_.translation = Add(worldTransform_.translation, velocity_);

			worldTransform_.UpdateMatrix();
		}

		if (moveTimer_ < 0)
		{
			if (!isHit_)
			{
				moveTimer_ = Random(30, 90);;
				patternCount_ = Random(5, 7);

			}
			else {
				moveTimer_ = Random(30,90);
				patternCount_ = Random(4, 7);
			}
		}

		Vector3 playerWorldPosition = player_->GetWorldPosition();

		Vector3 enemyWorldPosition = GetWorldPosition();

		if (enemyWorldPosition.x > playerWorldPosition.x)
		{
			worldTransform_.rotation.y = 4.6f;
		}

		if (enemyWorldPosition.x < playerWorldPosition.x)
		{
			worldTransform_.rotation.y = 1.7f;
		}

		if (worldTransform_.translation.x >= 12.0f)
		{
			worldTransform_.translation.x = 12.0f;
		}

		if (worldTransform_.translation.x <= -12.0f)
		{
			worldTransform_.translation.x = -12.0f;
		}
	}

	//ジャンプ
	if (patternCount_ == 3 && isDown_ == false)
	{
		behaviorRequest_ = Behavior::kJump;
	}

	//投げ
	if (patternCount_ == 4 && isDown_ == false)
	{
		behaviorRequest_ = Behavior::kThrow;
		isThrow_ = true;
	}

	//攻撃
	//通常攻撃
	if (patternCount_ == 2 && isDown_ == false)
	{
		audio_->SoundPlayWave(attackSoundHandle_, false, 1.0f);
		behaviorRequest_ = Behavior::kAttack;
		workAttack_.isPunch = true;
	}

	//振り下ろし攻撃
	if (patternCount_ == 5 && isDown_ == false)
	{
		audio_->SoundPlayWave(attackSoundHandle_, false, 1.0f);
		behaviorRequest_ = Behavior::kAttack;
		workAttack_.isSwingDown = true;
	}

	//突き攻撃
	if (patternCount_ == 6 && isDown_ == false)
	{
		if (worldTransform_.rotation.y == 1.7f)
		{
			audio_->SoundPlayWave(attackSoundHandle_, false, 1.0f);
			behaviorRequest_ = Behavior::kAttack;
			workAttack_.isPoke = true;
			workAttack_.isPokeRight = true;
		}
		

		if (worldTransform_.rotation.y == 4.6f)
		{
			audio_->SoundPlayWave(attackSoundHandle_, false, 1.0f);
			behaviorRequest_ = Behavior::kAttack;
			workAttack_.isPoke = true;
			workAttack_.isPokeLeft = true;
		}
	}

	//薙ぎ払う攻撃
	if (patternCount_ == 7 && isDown_ == false)
	{
		audio_->SoundPlayWave(attackSoundHandle_, false, 1.0f);
		behaviorRequest_ = Behavior::kAttack;
		workAttack_.isMowDown = true;
	}
}

void Enemy::BehaviorAttackInitialize()
{
	//通常攻撃
	if (workAttack_.isPunch)
	{
		worldTransformL_arm_.rotation.x = -1.3f;
		worldTransformR_arm_.rotation.x = 0.0f;
		worldTransformL_arm_.rotation.y = 0.0f;
		worldTransformR_arm_.rotation.y = 0.0f;
	}

	//振り下ろし攻撃
	if (workAttack_.isSwingDown)
	{
		worldTransformL_arm_.rotation.x = (float)std::numbers::pi;
		worldTransformR_arm_.rotation.x = (float)std::numbers::pi;
		workAttack_.translation = { 0.0f,2.5f,0.0f };
		workAttack_.rotation = { 0.0f,0.0f,0.0f };
		workAttack_.stiffnessTimer = 60;
	}

	//突き攻撃
	if (workAttack_.isPoke)
	{
		worldTransformL_arm_.rotation.x = -1.3f;
		worldTransformR_arm_.rotation.x = -1.3f;
		workAttack_.translation = { 0.0f,0.5f,0.0f };
		workAttack_.rotation = { 1.5f,0.0f,0.0f };
		workAttack_.stiffnessTimer = 60;
		pokeTimer_ = 30;
	}

	//薙ぎ払う攻撃
	if (workAttack_.isMowDown)
	{
		worldTransformL_arm_.rotation.x = -1.3f;
		worldTransformR_arm_.rotation.x = -1.3f;
		worldTransformL_arm_.rotation.y = 0.0f;
		worldTransformR_arm_.rotation.y = 0.0f;
		workAttack_.translation = { 0.0f,0.5f,0.0f };
		workAttack_.rotation = { 1.0f,0.0f,3.14f / 2.0f };
		workAttack_.stiffnessTimer = 60;
	}

	isGuard_ = false;

	attackAnimationFrame = 0;
}

void Enemy::BehaviorAttackUpdate()
{
	//通常攻撃
	if (workAttack_.isPunch)
	{
		if (attackAnimationFrame < 3.0f)
		{
			worldTransformBody_.rotation.y += 0.1f;
		}
		else if (worldTransformBody_.rotation.y > -1.0f)
		{
			worldTransformBody_.rotation.y -= 0.1f;
		}
		else
		{
			workAttack_.stiffnessTimer--;

			if (workAttack_.stiffnessTimer < 0)
			{
				patternCount_ = 1;
				behaviorRequest_ = Behavior::kRoot;
				workAttack_.stiffnessTimer = 20;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.isPunch = false;
			}
		}
		attackAnimationFrame++;
	}

	//振り下ろし攻撃
	if (workAttack_.isSwingDown)
	{
		if (attackAnimationFrame < 10)
		{
			worldTransformL_arm_.rotation.x -= 0.05f;
			worldTransformR_arm_.rotation.x -= 0.05f;

			workAttack_.rotation.x -= 0.05f;

			enemyWeapon_->SetTranslation(workAttack_.translation);
			enemyWeapon_->SetRotation(workAttack_.rotation);

		}
		else if (workAttack_.rotation.x < 2.0f)
		{
			worldTransformL_arm_.rotation.x += 0.1f;
			worldTransformR_arm_.rotation.x += 0.1f;

			workAttack_.translation.z += 0.05f;
			workAttack_.translation.y -= 0.05f;
			workAttack_.rotation.x += 0.1f;

			enemyWeapon_->SetTranslation(workAttack_.translation);
			enemyWeapon_->SetRotation(workAttack_.rotation);
			enemyWeapon_->SetIsAttack(true);
			workAttack_.isAttack = true;

			if (isDown_)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isAttack = false;
				enemyWeapon_->SetIsAttack(false);
				workAttack_.isSwingDown = false;
			}
		}
		else
		{
			workAttack_.stiffnessTimer--;
			workAttack_.isAttack = false;
			enemyWeapon_->SetIsAttack(false);

			if (isDown_)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isAttack = false;
				enemyWeapon_->SetIsAttack(false);
				workAttack_.isSwingDown = false;
			}


			if (workAttack_.stiffnessTimer <= 0)
			{
				behaviorRequest_ = Behavior::kRoot;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isSwingDown = false;
			}
		}

		if (isHitSwingDown_ || isHitPoke_ || isHitMowDown_)
		{

		}
		attackAnimationFrame++;
	}

	//突き攻撃
	if (workAttack_.isPoke)
	{
		pokeTimer_--;

		if (attackAnimationFrame < 10)
		{
			if (workAttack_.isPokeRight)
			{
				workAttack_.rotation.z += 0.05f;
			}

			if (workAttack_.isPokeLeft)
			{
				workAttack_.rotation.z -= 0.05f;
			}

			enemyWeapon_->SetTranslation(workAttack_.translation);
			enemyWeapon_->SetRotation(workAttack_.rotation);

		}
		else if (pokeTimer_ > 0)
		{
			if (workAttack_.isPokeRight)
			{
				workAttack_.rotation.z += 0.3f;
				worldTransform_.translation.x += 0.3f;
			}

			if (workAttack_.isPokeLeft)
			{
				workAttack_.rotation.z -= 0.3f;
				worldTransform_.translation.x -= 0.3f;
			}

			enemyWeapon_->SetTranslation(workAttack_.translation);
			enemyWeapon_->SetRotation(workAttack_.rotation);
			enemyWeapon_->SetIsAttack(true);
			workAttack_.isAttack = true;

			if (isHit_ || isDown_)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isAttack = false;
				enemyWeapon_->SetIsAttack(false);
				workAttack_.isPoke = false;
				workAttack_.isPokeRight = false;
				workAttack_.isPokeLeft = false;
			}
		}
		else
		{
			workAttack_.stiffnessTimer--;
			workAttack_.isAttack = false;
			enemyWeapon_->SetIsAttack(false);

			if (isDown_)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isAttack = false;
				enemyWeapon_->SetIsAttack(false);
				workAttack_.isPoke = false;
				workAttack_.isPokeRight = false;
				workAttack_.isPokeLeft = false;
			}

			if (workAttack_.stiffnessTimer <= 0)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				pokeTimer_ = 30;
				workAttack_.isPoke = false;
				workAttack_.isPokeRight = false;
				workAttack_.isPokeLeft = false;
			}
		}
		attackAnimationFrame++;
	}

	//薙ぎ払う攻撃
	if (workAttack_.isMowDown)
	{
		if (attackAnimationFrame < 10)
		{
			worldTransformBody_.rotation.y -= 0.1f;

			workAttack_.rotation.x -= 0.05f;

			enemyWeapon_->SetTranslation(workAttack_.translation);
			enemyWeapon_->SetRotation(workAttack_.rotation);

		}
		else if (workAttack_.rotation.x <= 3.8f)
		{
			worldTransformBody_.rotation.y += 0.1f;

			workAttack_.rotation.x += 0.1f;

			enemyWeapon_->SetTranslation(workAttack_.translation);
			enemyWeapon_->SetRotation(workAttack_.rotation);
			enemyWeapon_->SetIsAttack(true);
			workAttack_.isAttack = true;

			if (isDown_)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isAttack = false;
				enemyWeapon_->SetIsAttack(false);
				workAttack_.isMowDown = false;
			}
		}
		else
		{
			workAttack_.stiffnessTimer--;
			workAttack_.isAttack = false;
			enemyWeapon_->SetIsAttack(false);

			if (isDown_)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isAttack = false;
				enemyWeapon_->SetIsAttack(false);
				workAttack_.isMowDown = false;
			}

			if (workAttack_.stiffnessTimer <= 0)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isAttack = false;
				workAttack_.isMowDown = false;
			}
		}
		attackAnimationFrame++;
	}
}

void Enemy::BehaviorJumpInitialize()
{
	worldTransform_.translation.y = 0.0f;

	const float kJumpFirstSpeed_ = 0.9f;

	velocity_.y = kJumpFirstSpeed_;

	isGuard_ = false;
}

void Enemy::BehaviorJumpUpdate()
{
	worldTransform_.translation = Add(worldTransform_.translation, velocity_);

	const float kGravityAcceleration_ = 0.04f;

	Vector3 accelerationVector_ = { 0.0f,-kGravityAcceleration_,0.0f };

	velocity_ = Add(velocity_, accelerationVector_);

	if (worldTransform_.translation.y <= 0.0f)
	{
		patternCount_ = 1;
		behaviorRequest_ = Behavior::kRoot;
		workAttack_.isJumpAttack = false;
		worldTransformL_arm_.rotation.x = 0.0f;
		worldTransformR_arm_.rotation.x = 0.0f;
		worldTransform_.translation.y = 0.0f;
	}
}

void Enemy::BehaviorThrowInitialize()
{
	if (isThrow_)
	{
		worldTransformL_arm_.rotation.x = -1.3f;
		worldTransformR_arm_.rotation.x = -1.3f;
		worldTransformL_arm_.rotation.y = 0.0f;
		worldTransformR_arm_.rotation.y = 0.0f;
		attackAnimationFrame = 0;
	}

	isGuard_ = false;
}

void Enemy::BehaviorThrowUpdate()
{
	//投げ
	if (isThrow_)
	{
		if (attackAnimationFrame < 30)
		{
			worldTransformL_arm_.rotation.y -= 0.02f;
			worldTransformR_arm_.rotation.y += 0.02f;
		}
		else if (player_->GetIsEnemyHit() == true)
		{
			throwTimer_--;
			worldTransformL_arm_.rotation.x += 0.2f;
			worldTransformR_arm_.rotation.x += 0.2f;

			if (throwTimer_ <= 0)
			{
				behaviorRequest_ = Behavior::kRoot;
				throwTimer_ = 100;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				isThrow_ = false;
			}
		}
		else
		{
			workAttack_.stiffnessTimer--;

			if (workAttack_.stiffnessTimer <= 0)
			{
				patternCount_ = 1;
				behaviorRequest_ = Behavior::kRoot;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				throwTimer_ = 100;
				workAttack_.stiffnessTimer = 60;
				isThrow_ = false;
			}
		}
		attackAnimationFrame++;
	}
}

void Enemy::BehaviorStanInitialize()
{

}

void Enemy::BehaviorStanUpdate()
{
	stanTimer_--;

	if (stanTimer_ < 0)
	{
		stanTimer_ = 100;
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Enemy::DownAnimation()
{
	//通常攻撃
	if (isHitPunch_ && player_->GetRotation().y == 1.7f)
	{
		isDown_ = true;
		downAnimationTimer_[3]--;

		if (downAnimationTimer_[3] > 40)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation(worldTransform_.translation)
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.2f, 0.2f,0.2f }, { 0.4f ,0.4f ,0.4f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.06f ,0.06f ,0.06f }, { 0.1f ,0.1f ,0.1f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(50)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (downAnimationTimer_[3] > 0)
		{
			worldTransformBody_.rotation.x -= 0.01f;
		}

		if (player_->GetIsPunch() == false)
		{
			downAnimationTimer_[3] = 60;
			isHitPunch_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	if (isHitPunch_ && player_->GetRotation().y == 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_[3]--;

		if (downAnimationTimer_[3] > 40)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation(worldTransform_.translation)
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.2f, 0.2f,0.2f }, { 0.4f ,0.4f ,0.4f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.06f ,0.06f ,0.06f }, { 0.1f ,0.1f ,0.1f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(50)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (downAnimationTimer_[3] > 0)
		{
			worldTransformBody_.rotation.x -= 0.01f;
		}

		if (player_->GetIsPunch() == false)
		{
			downAnimationTimer_[3] = 60;
			isHitPunch_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	//振り下ろし攻撃
	if (isHitSwingDown_ && player_->GetRotation().y == 1.7f)
	{
		isDown_ = true;
		downAnimationTimer_[0]--;

		if (downAnimationTimer_[0] > 50)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation(worldTransform_.translation)
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.6f, 0.6f,0.6f }, { 1.0f ,1.0f ,1.0f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.06f ,0.06f ,0.06f }, { 0.1f ,0.1f ,0.1f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 1.0f)
				.SetCount(100)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (downAnimationTimer_[0] > 0)
		{
			worldTransform_.translation.x += 0.1f;
			worldTransformBody_.rotation.x -= 0.03f;
		}

		if (downAnimationTimer_[0] <= 0)
		{
			behaviorRequest_ = Behavior::kRoot;
			downAnimationTimer_[0] = 60;
			isHitSwingDown_ = false;
			workAttack_.isSwingDown = false;
			workAttack_.isPoke = false;
			workAttack_.isMowDown = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
			worldTransformBody_.rotation.y = 0.0f;
		}
	}

	if (isHitSwingDown_ && player_->GetRotation().y == 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_[0]--;

		if (downAnimationTimer_[0] > 50)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation(worldTransform_.translation)
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.6f, 0.6f,0.6f }, { 1.0f ,1.0f ,1.0f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.06f ,0.06f ,0.06f }, { 0.1f ,0.1f ,0.1f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 0.5f)
				.SetCount(100)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (downAnimationTimer_[0] > 0)
		{
			worldTransform_.translation.x -= 0.1f;
			worldTransformBody_.rotation.x -= 0.03f;
		}

		if (downAnimationTimer_[0] <= 0)
		{
			behaviorRequest_ = Behavior::kRoot;
			downAnimationTimer_[0] = 60;
			isHitSwingDown_ = false;
			workAttack_.isSwingDown = false;
			workAttack_.isPoke = false;
			workAttack_.isMowDown = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
			worldTransformBody_.rotation.y = 0.0f;
		}
	}

	//突き攻撃
	if (isHitPoke_ && player_->GetRotation().y == 1.7f)
	{
		isDown_ = true;
		downAnimationTimer_[1]--;

		if (downAnimationTimer_[1] > 50)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation(worldTransform_.translation)
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.6f, 0.6f,0.6f }, { 1.0f ,1.0f ,1.0f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.06f ,0.06f ,0.06f }, { 0.1f ,0.1f ,0.1f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 0.5f)
				.SetCount(100)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (downAnimationTimer_[1] > 0)
		{
			worldTransform_.translation.x += 0.3f;
			worldTransformBody_.rotation.x -= 0.03f;
		}

		if (downAnimationTimer_[1] <= 0)
		{
			behaviorRequest_ = Behavior::kRoot;
			downAnimationTimer_[1] = 60;
			isHitPoke_ = false;
			workAttack_.isSwingDown = false;
			workAttack_.isPoke = false;
			workAttack_.isMowDown = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
			worldTransformBody_.rotation.y = 0.0f;
		}
	}

	if (isHitPoke_ && player_->GetRotation().y == 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_[1]--;

		if (downAnimationTimer_[1] > 50)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation(worldTransform_.translation)
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.6f, 0.6f,0.6f }, { 1.0f ,1.0f ,1.0f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.06f ,0.06f ,0.06f }, { 0.1f ,0.1f ,0.1f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 0.5f)
				.SetCount(100)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (downAnimationTimer_[1] > 0)
		{
			worldTransform_.translation.x -= 0.3f;
			worldTransformBody_.rotation.x -= 0.03f;
		}

		if (downAnimationTimer_[1] <= 0)
		{
			behaviorRequest_ = Behavior::kRoot;
			downAnimationTimer_[1] = 60;
			isHitPoke_ = false;
			workAttack_.isSwingDown = false;
			workAttack_.isPoke = false;
			workAttack_.isMowDown = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
			worldTransformBody_.rotation.y = 0.0f;
		}
	}

	//薙ぎ払い攻撃
	if (isHitMowDown_ && player_->GetRotation().y >= 1.7f && player_->GetRotation().y < 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_[2]--;

		if (downAnimationTimer_[2] > 50)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation(worldTransform_.translation)
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.6f, 0.6f,0.6f }, { 1.0f ,1.0f ,1.0f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.06f ,0.06f ,0.06f }, { 0.1f ,0.1f ,0.1f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 0.5f)
				.SetCount(100)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (downAnimationTimer_[2] > 0)
		{
			worldTransform_.translation.x += 0.1f;
			worldTransformBody_.rotation.x -= 0.03f;
		}

		if (downAnimationTimer_[2] <= 0)
		{
			behaviorRequest_ = Behavior::kRoot;
			downAnimationTimer_[2] = 60;
			isHitMowDown_ = false;
			workAttack_.isSwingDown = false;
			workAttack_.isPoke = false;
			workAttack_.isMowDown = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
			worldTransformBody_.rotation.y = 0.0f;
		}
	}

	if (isHitMowDown_ && player_->GetRotation().y >= 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_[2]--;

		if (downAnimationTimer_[2] > 50)
		{
			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kNormal)
				.SetTranslation(worldTransform_.translation)
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.6f, 0.6f,0.6f }, { 1.0f ,1.0f ,1.0f })
				.SetAzimuth(0.0f, 360.0f)
				.SetElevation(0.0f, 0.0f)
				.SetVelocity({ 0.06f ,0.06f ,0.06f }, { 0.1f ,0.1f ,0.1f })
				.SetColor({ 1.0f ,0.5f ,0.0f ,1.0f }, { 1.0f ,0.5f ,0.0f ,1.0f })
				.SetLifeTime(0.1f, 0.5f)
				.SetCount(100)
				.SetFrequency(4.0f)
				.SetDeleteTime(1.0f)
				.Build();
			particleSystem_->AddParticleEmitter(newParticleEmitter);
		}

		if (downAnimationTimer_[2] > 0)
		{
			worldTransform_.translation.x -= 0.1f;
			worldTransformBody_.rotation.x -= 0.03f;
		}

		if (downAnimationTimer_[2] <= 0)
		{
			behaviorRequest_ = Behavior::kRoot;
			downAnimationTimer_[2] = 60;
			isHitMowDown_ = false;
			workAttack_.isSwingDown = false;
			workAttack_.isPoke = false;
			workAttack_.isMowDown = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
			worldTransformBody_.rotation.y = 0.0f;
		}
	}

	//投げ攻撃
	if (isHitThrow_ && player_->GetRotation().y == 1.7f)
	{
		isDown_ = true;
		if (player_->GetAttackAnimationFrame() < 30)
		{
			worldTransformBody_.rotation.x += 0.01f;

		}
		else if (player_->GetThrowTimer() > 5)
		{
			worldTransformBody_.rotation.x -= 0.2f;
		}
		else if (player_->GetThrowTimer() <= 5)
		{
			worldTransform_.translation.x += 0.3f;
			worldTransformBody_.rotation.x -= 0.2f;
		}

		if (player_->GetIsThrow() == false)
		{
			downAnimationTimer_[4] = 60;
			isHitThrow_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	if (isHitThrow_ && player_->GetRotation().y == 4.6f)
	{
		isDown_ = true;
		if (player_->GetAttackAnimationFrame() < 30)
		{
			worldTransformBody_.rotation.x -= 0.01f;

		}
		else if (player_->GetThrowTimer() > 10)
		{
			worldTransformBody_.rotation.x -= 0.2f;
		}
		else if (player_->GetThrowTimer() <= 10)
		{
			worldTransform_.translation.x -= 0.3f;
			worldTransformBody_.rotation.x -= 0.2f;
		}

		if (player_->GetIsThrow() == false)
		{
			downAnimationTimer_[4] = 60;
			isHitThrow_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}
}

void Enemy::FloatingGimmickInitialize()
{
	for (int i = 0; i < kMaxModelParts; i++)
	{
		floatingParameter_[i] = 0.0f;
	}
}

void Enemy::FloatingGimmickUpdate()
{
	floatingCycle_[0] = 120;
	floatingCycle_[1] = 120;

	float step[2]{};

	for (int i = 0; i < kMaxModelParts; i++)
	{
		step[i] = 2.0f * (float)std::numbers::pi / floatingCycle_[i];

		floatingParameter_[i] += step[i];

		floatingParameter_[i] = (float)std::fmod(floatingParameter_[i], 2.0f * (float)std::numbers::pi);
	}

	worldTransformL_arm_.rotation.x = std::sin(floatingParameter_[1]) * 0.35f;
	worldTransformR_arm_.rotation.x = -std::sin(floatingParameter_[1]) * 0.35f;
}

int Enemy::Random(int min_value, int max_value)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(min_value, max_value);

	return dis(gen);
}


