#include "Enemy.h"
#include <cassert>
#include <numbers>
#include "Application/GameObject/Character/Player/Player.h"
#include "Application/Game/Scenes/GamePlayScene.h"

Enemy::~Enemy()
{
	delete hpBar_.sprite_;
	delete guardGaugeBar_.sprite_;
	delete finisherGaugeBar_.sprite_;
}

void Enemy::Initialize()
{
	audio_ = Audio::GetInstance();

	IGame3dObject::SetTag("Enemy");

	AABB aabb= { {-0.3f,-1.0f,-10.0f},{3.0f,1.0f,10.0f} };
	SetAABB(aabb);

	modelFighterBody_.reset(Model::CreateFromOBJ("resource/newEnemy", "newEnemy.gltf"));

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
		TextureManager::LoadTexture("resource/guardGauge.png"),
		{720.0f, guardGaugeBarSpace},
		0.0f,
		{guardGaugeBarSize  ,7.0f},
		nullptr,
	};

	guardGaugeBar_.sprite_ = Sprite::Create(guardGaugeBar_.textureHandle_, guardGaugeBar_.position_);

	finisherGaugeBar_ = {
		true,
		TextureManager::LoadTexture("resource/guardGauge.png"),
		{980.0f, finisherGaugeBarSpace},
		0.0f,
		{-finisherGaugeBarSize  ,20.0f},
		nullptr,
	};

	finisherGaugeBar_.sprite_ = Sprite::Create(finisherGaugeBar_.textureHandle_, finisherGaugeBar_.position_);

	hitTextureHandle_ = TextureManager::LoadTexture("resource/Hit.png");
	hitSprite_.reset(Sprite::Create(hitTextureHandle_, { 40.0f, 180.0f }));

	worldTransform_.Initialize();
	worldTransform_.translation = { 7.0f,0.0f,0.0f };

	worldTransformHead_.Initialize();
	worldTransform_.rotation.y = 4.6f;

	worldTransformBody_.Initialize();
	worldTransformBody_.translation = { 0.0f,0.0f,0.0f };
	worldTransformBody_.rotation = { 7.75f,0.0f,0.0f };
	worldTransformBody_.scale = { 0.007f,0.007f,0.007f };

	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformHead_.parent_ = &worldTransformBody_;

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
	worldTransform_.UpdateMatrixEuler();

	worldTransformBody_.UpdateMatrixEuler();
	worldTransformHead_.UpdateMatrixEuler();

	attackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Attack.mp3");
	weaponAttackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/WeaponAttack.mp3");
	damageSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Damage.mp3");
	guardSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Guard.mp3");
}

void Enemy::Update()
{
	isShake_ = false;

	if (behaviorRequest_ == Behavior::kRoot && HP_ > 0)
	{
		animationIndex = 2;
	}
	else if (behaviorRequest_ == Behavior::kAttack && workAttack_.isMowDown)
	{
		animationIndex = 1;
	}
	else if (HP_ <= 0 || isDown_ || GamePlayScene::roundStartTimer_ <= 0)
	{
		animationIndex = 0;
	}


	modelFighterBody_->ApplyAnimation(animationIndex);

	modelFighterBody_->Update();


	if (guardGauge_ > 0 && guardGauge_ < maxGuardGauge_)
	{
		guardGauge_ -= 0.03f;
	}

	if (finisherGauge_ < maxFinisherGauge_ && Input::GetInstance()->PressKey(DIK_L))
	{
		finisherGauge_ += 0.08f;
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
		if (GamePlayScene::roundStartTimer_ <= 0)
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

	case Behavior::kThrow:
		BehaviorThrowUpdate();
		break;

	case Behavior::kStan:
		BehaviorStanUpdate();
		break;
	}

	if (worldTransform_.translation.x >= 12.4f)
	{
		worldTransform_.translation.x = 12.4f;
	}

	if (worldTransform_.translation.x <= -12.4f)
	{
		worldTransform_.translation.x = -12.4f;
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

	worldTransform_.UpdateMatrixEuler();

	worldTransformBody_.UpdateMatrixEuler();

	isHit_ = false;

	isPlayerHit_ = false;

	HPBarUpdate();

	if (guardGauge_ <= 50.0f)
	{
		GuardGaugeBarUpdate();
	}

	FinisherGaugeBarUpdate();

	if (isHitPunch_)
	{
		comboCount_ = 1;
		comboTimer_--;
	}

	if (isHitCPunch_)
	{
		comboCount_ = 2;
		comboTimer_ = 60;
		comboTimer_--;
	}

	if (comboCount_ == 2 && isHitMowDown_)
	{
		comboCount_ = 3;
		comboTimer_--;
	}

	if (comboTimer_ < 60)
	{
		comboTimer_--;
	}

	if (comboTimer_ < 0)
	{
		comboTimer_ = 60;
		comboCount_ = 0;
	}

	UpdateComboNumberSprite();

	ImGui::Begin("GuardGauge");
	ImGui::Text("GuardGauge %f", guardGauge_);
	ImGui::Text("ComboC %d", comboCount_);
	ImGui::Text("ComboT %d", comboTimer_);
	ImGui::SliderFloat3("WTFT", &worldTransform_.translation.x, -16.0f, 16.0f);
	ImGui::SliderFloat3("WTFR", &worldTransformBody_.rotation.x, 0.0f, 16.0f);
	ImGui::End();
}

void Enemy::Draw(const Camera& camera)
{
	//Enemyの描画
	modelFighterBody_->Draw(worldTransformBody_, camera, animationIndex);

	//Weaponの描画
	if (workAttack_.isAttack && workAttack_.isSwingDown || workAttack_.isMowDown || workAttack_.isPoke && !isHitSwingDown_
		&& !isHitPoke_ && !isHitMowDown_ && !isDown_ && behaviorRequest_ != Behavior::kRoot
		&& workAttack_.isAttack)
	{
		enemyWeapon_->Draw(camera);
	}
}

void Enemy::BoneDraw(const Camera& camera)
{
	//Enemyの描画
	modelFighterBody_->BoneDraw(worldTransformBody_, camera, animationIndex);
}

void Enemy::DrawSprite()
{
	if (HP_ >= 0)
	{
		hpBar_.sprite_->Draw();
	}

	guardGaugeBar_.sprite_->Draw();

	finisherGaugeBar_.sprite_->Draw();

	if (comboCount_ >= 2)
	{
		hitSprite_->Draw();
		comboNumSprite_->Draw();
	}
}

void Enemy::HPBarUpdate()
{
	hpBar_.size_ = { (HP_ / maxHP_) * barSize,7.0f };

	hpBar_.sprite_->SetSize(hpBar_.size_);

	if (HP_ > 50)
	{
		hpBar_.sprite_->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	}

	if (HP_ <= 50 && HP_ > 25)
	{
		hpBar_.sprite_->SetColor({ 1.0f, 0.8f, 0.0f, 1.0f });
	}
	else if (HP_ <= 25)
	{
		hpBar_.sprite_->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	}
}

void Enemy::GuardGaugeBarUpdate()
{
	guardGaugeBar_.size_ = { (guardGauge_ / maxGuardGauge_) * guardGaugeBarSize,7.0f };

	guardGaugeBar_.sprite_->SetSize(guardGaugeBar_.size_);

	guardGaugeBar_.sprite_->SetColor({ 0.0f, 0.5f, 1.0f, 1.0f });
}

void Enemy::FinisherGaugeBarUpdate()
{
	finisherGaugeBar_.size_ = { (finisherGauge_ / maxFinisherGauge_) * finisherGaugeBarSize,20.0f };

	finisherGaugeBar_.sprite_->SetSize(finisherGaugeBar_.size_);

	if (finisherGauge_ < maxFinisherGauge_)
	{
		finisherGaugeBar_.sprite_->SetColor({ 0.0f, 0.5f, 1.0f, 1.0f });
	}
	else
	{
		finisherGaugeBar_.sprite_->SetColor({ 1.0f, 0.5f, 0.0f, 1.0f });
	}
}

void Enemy::UpdateComboNumberSprite()
{
	int comboNum = comboCount_;

	comboNumTextureHandle_ = TextureManager::LoadTexture("resource/number/" + std::to_string(comboNum) + ".png");
	
	comboNumSprite_.reset(Sprite::Create(comboNumTextureHandle_, { 10.0f, 290.0f }));
}

void Enemy::HitStop(int milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Enemy::Reset()
{
	HP_ = maxHP_;

	guardGauge_ = 0.0f;

	for (int i = 0; i < 6; i++)
	{
		downAnimationTimer_[i] = 60;
	}

	downAnimationTimer_[6] = 30;

	isReset_ = true;

	isHitPunch_ = false;
	isHitCPunch_ = false;
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

	comboCount_ = 0;

	behavior_ = Behavior::kRoot;

	worldTransform_.Initialize();
	worldTransform_.translation = { 7.0f,0.0f,0.0f };
	worldTransform_.rotation.y = 4.6f;

	worldTransformBody_.Initialize();
	worldTransformBody_.translation = { 0.0f,0.0f,0.0f };
	worldTransformBody_.rotation = { 7.75f,0.0f,0.0f };
	worldTransformBody_.scale = { 0.007f,0.007f,0.007f };

	worldTransform_.UpdateMatrixEuler();
	worldTransformBody_.UpdateMatrixEuler();
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

		/*if (isGuard_ && worldTransform_.rotation.y == 1.7f && player_->GetIsPunch() == true)
		{
			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.3f;
			guardGauge_ += 1.0f;

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

		if (isGuard_ && worldTransform_.rotation.y == 4.6f && player_->GetIsPunch() == true)
		{
			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x += 0.3f;
			guardGauge_ += 1.0f;

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
		}*/

		if (player_->GetIsPunch() == true && isDown_ == false && isGuard_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 8.0f;
			HP_ -= damage;
			isHitPunch_ = true;

			HitStop(20);
		}

		if (player_->GetIsCPunch() == true && isDown_ == false && isGuard_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 8.0f;
			HP_ -= damage;
			isHitCPunch_ = true;
		}

		if (player_->GetIsThrow() == true && isDown_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 20.0f;
			HP_ -= damage;
			isHitThrow_ = true;
		}
	}

	if (collider->GetCollisionAttribute() & kCollisionAttributePlayerWeapon)
	{
		if (isGuard_ && worldTransform_.rotation.y == 1.7f)
		{
			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.3f;
			guardGauge_ += 1.0f;

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
			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x += 0.3f;
			guardGauge_ += 1.0f;

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
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 20.0f;
			HP_ -= damage;
			isHitSwingDown_ = true;
			isShake_ = true;

			HitStop(100);
		}

		if (player_->GetIsAttack() == true && player_->GetIsPoke() == true && isDown_ == false
			&& isGuard_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 15.0f;
			HP_ -= damage;
			isHitPoke_ = true;
			isShake_ = true;

			HitStop(100);
		}

		if (player_->GetIsAttack() == true && player_->GetIsMowDown() == true && isDown_ == false
			&& isGuard_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 20.0f;
			HP_ -= damage;
			isHitMowDown_ = true;
			isShake_ = true;

			HitStop(100);
		}

		if (player_->GetIsAttack() == true && player_->GetIsMowDown() == true && 
			player_->GetIsCancelCount() == 1)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 0.6f;
			HP_ -= damage;
			isHitMowDown_ = true;
			isShake_ = true;
		}

		if (player_->GetIsAttack() == true && player_->GetIsFinisher() == true
			&& isGuard_ == false && player_->GetFinisherCount() == 1)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 0.5f;
			HP_ -= damage;
			isHitFinisher_ = true;
			isShake_ = true;
		}

		if (player_->GetIsAttack() == true && player_->GetIsFinisher() == true
			&& isGuard_ == false && player_->GetFinisherCount() == 2)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 1.0f;
			HP_ -= damage;
			isHitFinisher_ = true;
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
	if (GamePlayScene::migrationTimer >= 150)
	{
		float animationTime;
		animationTime = modelFighterBody_->GetAnimationTime();
		if (!isDown_)
		{
			animationTime += 1.0f / 60.0f;
			animationTime = std::fmod(animationTime, modelFighterBody_->GetAnimation()[2].duration);
		}

		modelFighterBody_->SetAnimationTime(animationTime);

		modelFighterBody_->ApplyAnimation(2);

		modelFighterBody_->Update();

		patternCount_ = 1;

		//コントローラーの移動処理
		if (patternCount_ == 1 && isDown_ == false)
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

				worldTransform_.UpdateMatrixEuler();
			}

			if (moveTimer_ < 0)
			{
				if (!isHit_)
				{
					moveTimer_ = Random(30, 90);;
					patternCount_ = Random(5, 5);

				}
				else {
					moveTimer_ = Random(30, 90);
					patternCount_ = Random(5, 5);
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
			audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
			behaviorRequest_ = Behavior::kAttack;
			workAttack_.isPunch = true;
		}

		//振り下ろし攻撃
		if (patternCount_ == 5 && isDown_ == false)
		{
			audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
			behaviorRequest_ = Behavior::kAttack;
			workAttack_.isSwingDown = true;
			animationTime = 0.0f;
			modelFighterBody_->SetAnimationTime(animationTime);
		}

		//突き攻撃
		if (patternCount_ == 6 && isDown_ == false)
		{
			if (worldTransform_.rotation.y == 1.7f)
			{
				audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
				behaviorRequest_ = Behavior::kAttack;
				workAttack_.isPoke = true;
				workAttack_.isPokeRight = true;
			}


			if (worldTransform_.rotation.y == 4.6f)
			{
				audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
				behaviorRequest_ = Behavior::kAttack;
				workAttack_.isPoke = true;
				workAttack_.isPokeLeft = true;
			}
		}

		//薙ぎ払う攻撃
		if (patternCount_ == 7 && isDown_ == false)
		{
			audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
			behaviorRequest_ = Behavior::kAttack;
			workAttack_.isMowDown = true;
		}
	}
}

void Enemy::BehaviorAttackInitialize()
{
	//通常攻撃
	if (workAttack_.isPunch)
	{
		
	}

	//振り下ろし攻撃
	if (workAttack_.isSwingDown)
	{
		workAttack_.translation = { 0.0f,2.5f,0.0f };
		workAttack_.rotation = { 0.0f,0.0f,0.0f };
		workAttack_.stiffnessTimer = 60;
	}

	//突き攻撃
	if (workAttack_.isPoke)
	{
		workAttack_.translation = { 0.0f,0.5f,0.0f };
		workAttack_.rotation = { 1.5f,0.0f,0.0f };
		workAttack_.stiffnessTimer = 60;
		pokeTimer_ = 30;
	}

	//薙ぎ払う攻撃
	if (workAttack_.isMowDown)
	{
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
				workAttack_.isPunch = false;
			}
		}
		attackAnimationFrame++;
	}

	//振り下ろし攻撃
	if (workAttack_.isSwingDown)
	{
		float animationTime, animationDuration;
		animationTime = modelFighterBody_->GetAnimationTime();

		if (!isDown_)
		{
			animationTime += 1.0f / 60.0f;
			/*animationTime = std::fmod(animationTime, modelFighterBody_->GetAnimation()[0].duration);*/
		}
	
		animationDuration = modelFighterBody_->GetAnimation()[1].duration;

		modelFighterBody_->SetAnimationTime(animationTime);

		modelFighterBody_->ApplyAnimation(1);

		modelFighterBody_->Update();

		if (attackAnimationFrame < 10)
		{
			workAttack_.rotation.x -= 0.05f;

			enemyWeapon_->SetTranslation(workAttack_.translation);
			enemyWeapon_->SetRotation(workAttack_.rotation);

		}
		else if (workAttack_.rotation.x < 2.0f)
		{
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
				workAttack_.stiffnessTimer = 60;
				workAttack_.isAttack = false;
				enemyWeapon_->SetIsAttack(false);
				workAttack_.isSwingDown = false;
			}


			if (animationTime >= animationDuration)
			{
				// アニメーションが終了した場合の処理
				behaviorRequest_ = Behavior::kRoot;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isSwingDown = false;
				animationTime = 0.0f;
				modelFighterBody_->SetAnimationTime(animationTime);
			}

			/*if (workAttack_.stiffnessTimer <= 0)
			{
				behaviorRequest_ = Behavior::kRoot;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isSwingDown = false;
			}*/
		}

		/*if (isHitSwingDown_ || isHitPoke_ || isHitMowDown_)
		{

		}*/
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
		worldTransform_.translation.y = 0.0f;
	}
}

void Enemy::BehaviorThrowInitialize()
{
	if (isThrow_)
	{
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
		}
		else if (player_->GetIsEnemyHit() == true)
		{
			throwTimer_--;

			if (throwTimer_ <= 0)
			{
				behaviorRequest_ = Behavior::kRoot;
				throwTimer_ = 100;
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
	if (stanTimer_ == 200)
	{
		isShake_ = true;
	}

	if (stanTimer_ <= 170)
	{
		isGuard_ = false;
	}

	guardGauge_ = maxGuardGauge_;

	stanTimer_--;

	worldTransformBody_.rotation.y += 0.1f;

	if (stanTimer_ < 0 || isHitMowDown_ || isHitPoke_ || isHitPunch_ ||
		isHitSwingDown_ || isThrow_)
	{
		stanTimer_ = 200;
		guardGauge_ = 0.0f;
		behaviorRequest_ = Behavior::kRoot;

		worldTransformHead_.rotation.y = 0.0f;

		worldTransformBody_.rotation.x = 0.0f;
		worldTransformBody_.rotation.y = 0.0f;

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

	//C通常攻撃
	if (isHitCPunch_ && player_->GetRotation().y == 1.7f)
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

		if (player_->GetIsCPunch() == false)
		{
			downAnimationTimer_[3] = 60;
			isHitCPunch_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	if (isHitCPunch_ && player_->GetRotation().y == 4.6f)
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

		if (player_->GetIsCPunch() == false)
		{
			downAnimationTimer_[3] = 60;
			isHitCPunch_ = false;
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

		if (downAnimationTimer_[0] <= 0 && HP_ > 0)
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

		if (downAnimationTimer_[0] <= 0 && HP_ > 0)
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

		if (downAnimationTimer_[1] <= 0 && HP_ > 0)
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

		if (downAnimationTimer_[1] <= 0 && HP_ > 0)
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

		if (downAnimationTimer_[2] <= 0 && HP_ > 0)
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

		if (downAnimationTimer_[2] <= 0 && HP_ > 0)
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

	//Finisher
	if (isHitFinisher_ && player_->GetRotation().y == 1.7f && player_->GetFinisherCount() == 1)
	{
		isDown_ = true;
		downAnimationTimer_[6]--;

		if (downAnimationTimer_[6] > 20)
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

		if (downAnimationTimer_[6] > 0)
		{
			worldTransformBody_.rotation.x -= 0.01f;
		}

		if (downAnimationTimer_[6] <= 0 && HP_ > 0)
		{
			downAnimationTimer_[6] = 30;
			isHitFinisher_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	if (isHitFinisher_ && player_->GetRotation().y == 4.6f && player_->GetFinisherCount() == 1)
	{
		isDown_ = true;
		downAnimationTimer_[6]--;

		if (downAnimationTimer_[6] > 20)
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

		if (downAnimationTimer_[6] > 0)
		{
			worldTransformBody_.rotation.x -= 0.01f;
		}

		if (downAnimationTimer_[6] <= 0 && HP_ > 0)
		{
			downAnimationTimer_[6] = 30;
			isHitFinisher_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	if (isHitFinisher_ && player_->GetRotation().y == 1.7f && player_->GetFinisherCount() == 2)
	{
		isDown_ = true;
		downAnimationTimer_[2]--;

		if (downAnimationTimer_[2] > 40)
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

		if (downAnimationTimer_[2] > 0)
		{
			worldTransform_.translation.x += 0.1f;
			worldTransformBody_.rotation.x -= 0.03f;
		}

		if (downAnimationTimer_[2] <= 0 && HP_ > 0)
		{
			behaviorRequest_ = Behavior::kRoot;
			downAnimationTimer_[2] = 60;
			isHitFinisher_ = false;
			workAttack_.isSwingDown = false;
			workAttack_.isPoke = false;
			workAttack_.isMowDown = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
			worldTransformBody_.rotation.y = 0.0f;
		}
	}

	if (isHitFinisher_ && player_->GetRotation().y == 4.6f && player_->GetFinisherCount() == 2)
	{
		isDown_ = true;
		downAnimationTimer_[2]--;

		if (downAnimationTimer_[2] > 40)
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

		if (downAnimationTimer_[2] > 0)
		{
			worldTransform_.translation.x -= 0.1f;
			worldTransformBody_.rotation.x -= 0.03f;
		}

		if (downAnimationTimer_[2] <= 0 && HP_ > 0)
		{
			behaviorRequest_ = Behavior::kRoot;
			downAnimationTimer_[2] = 60;
			isHitFinisher_ = false;
			workAttack_.isSwingDown = false;
			workAttack_.isPoke = false;
			workAttack_.isMowDown = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
			worldTransformBody_.rotation.y = 0.0f;
		}
	}

	for (int i = 0; i < 7; i++)
	{
		if (isDown_ && worldTransform_.translation.x >= 9.5f)
		{
			worldTransform_.translation.x = 9.5f;
		}

		if (isDown_ && worldTransform_.translation.x <= -9.5f)
		{
			worldTransform_.translation.x = -9.5f;
		}

		if (worldTransformBody_.rotation.x <= 6.3f)
		{
			worldTransformBody_.rotation.x = 6.3f;
		}

		if (downAnimationTimer_[i] <= 0 && HP_ <= 0)
		{	
			if (worldTransform_.translation.y <= 0.2f)
			{
				worldTransform_.translation.y = 0.2f;
			}
		}
		else if (isDown_ && worldTransform_.translation.y <= 0.2f)
		{
			worldTransform_.translation.y = 0.2f;
		}
	}
}

int Enemy::Random(int min_value, int max_value)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(min_value, max_value);

	return dis(gen);
}


