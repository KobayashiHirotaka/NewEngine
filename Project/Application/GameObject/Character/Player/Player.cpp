#include "Player.h"
#include <cassert>
#include <numbers>
#include "Application/GameObject/Character/Enemy/Enemy.h"
#include "Application/Game/Scenes/GamePlayScene.h"

Player::~Player()
{
	delete hpBar_.sprite_;
	delete guardGaugeBar_.sprite_;
	delete finisherGaugeBar_.sprite_;
}

void Player::Initialize()
{
	//Inputのインスタンス
	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	AABB aabb = { {-1.0f,-1.0f,-10.0f},{1.0f,1.0f,10.0f} };
	SetAABB(aabb);

	modelFighterBody_.reset(Model::CreateFromOBJ("resource/float_Body", "float_Body.obj"));
	modelFighterPHead_.reset(Model::CreateFromOBJ("resource/float_PHead", "playerHead.obj"));
	modelFighterL_arm_.reset(Model::CreateFromOBJ("resource/float_L_arm", "float_L_arm.obj"));
	modelFighterR_arm_.reset(Model::CreateFromOBJ("resource/float_R_arm", "float_R_arm.obj"));

	modelFighterPAHead_.reset(Model::CreateFromOBJ("resource/float_PHead_A", "float_PHead.gltf"));
	modelFighterLA_arm_.reset(Model::CreateFromOBJ("resource/float_PL_arm", "float_PL_arm.gltf"));
	modelFighterRA_arm_.reset(Model::CreateFromOBJ("resource/float_PR_arm", "float_PR_arm.gltf"));

	playerCursol_.reset(Model::CreateFromOBJ("resource/playerCursol", "playerCursol.obj"));

	hpBar_ = {
		true,
		TextureManager::LoadTexture("resource/HP.png"),
		{60.0f, barSpace},
		0.0f,
		{-barSize  ,7.0f},
		nullptr,
	};

	hpBar_.sprite_ = Sprite::Create(hpBar_.textureHandle_, hpBar_.position_);

	guardGaugeBar_ = {
		true,
		TextureManager::LoadTexture("resource/guardGauge.png"),
		{60.0f, guardGaugeBarSpace},
		0.0f,
		{-guardGaugeBarSize  ,7.0f},
		nullptr,
	};

	guardGaugeBar_.sprite_ = Sprite::Create(guardGaugeBar_.textureHandle_, guardGaugeBar_.position_);

	finisherGaugeBar_ = {
		true,
		TextureManager::LoadTexture("resource/guardGauge.png"),
		{60.0f, finisherGaugeBarSpace},
		0.0f,
		{-finisherGaugeBarSize  ,20.0f},
		nullptr,
	};

	finisherGaugeBar_.sprite_ = Sprite::Create(finisherGaugeBar_.textureHandle_, finisherGaugeBar_.position_);

	//WorldTransform(Player)の初期化
	worldTransform_.Initialize();
	worldTransform_.translation = { -7.0f,0.0f,6.5f };

	worldTransformHead_.Initialize();
	worldTransform_.rotation.y = 1.7f;

	worldTransformBody_.Initialize();
	worldTransformBody_.translation = { 0.0f,1.0f,0.0f };

	worldTransformL_arm_.Initialize();
	worldTransformL_arm_.translation.x = 0.5f;

	worldTransformR_arm_.Initialize();
	worldTransformR_arm_.translation.x = -0.5f;

	worldTransformAHead_.Initialize();

	worldTransformAL_arm_.Initialize();
	worldTransformAL_arm_.translation.x = 0.5f;

	worldTransformAR_arm_.Initialize();
	worldTransformAR_arm_.translation.x = -0.5f;

	//親子付け
	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;

	worldTransformAHead_.parent_ = &worldTransformBody_;
	worldTransformAL_arm_.parent_ = &worldTransformBody_;
	worldTransformAR_arm_.parent_ = &worldTransformBody_;

	//Weaponの生成
	playerWeapon_ = std::make_unique<PlayerWeapon>();
	playerWeapon_->Initialize();
	playerWeapon_->SetParent(&worldTransform_);

	//当たり判定の設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	particleModel_.reset(ParticleModel::CreateFromOBJ("resource/Particle", "Particle.obj"));
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();

	//WorldTransform(Player)の更新
	worldTransform_.UpdateMatrixEuler();

	worldTransformBody_.UpdateMatrixEuler();
	worldTransformHead_.UpdateMatrixEuler();
	worldTransformL_arm_.UpdateMatrixEuler();
	worldTransformR_arm_.UpdateMatrixEuler();

	worldTransformAHead_.UpdateMatrixEuler();
	worldTransformAL_arm_.UpdateMatrixEuler();
	worldTransformAR_arm_.UpdateMatrixEuler();

	attackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Attack.mp3");
	weaponAttackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/WeaponAttack.mp3");
	damageSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Damage.mp3");
	guardSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Guard.mp3");
}

void Player::Update()
{
	isShake_ = false;

	if (guardGauge_ > 0 && guardGauge_ < maxGuardGauge_)
	{
		guardGauge_ -= 0.03f;
	}

	if (finisherGauge_ >= maxFinisherGauge_)
	{
		finisherGauge_ = 50.0f;
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

	//PlayerのBehavior
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

		if (!isDown_)
		{
			FloatingGimmickUpdate();
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

	if (worldTransform_.translation.x >= 13.4f)
	{
		worldTransform_.translation.x = 13.4f;
	}

	if (worldTransform_.translation.x <= -13.4f)
	{
		worldTransform_.translation.x = -13.4f;
	}

	if (behaviorRequest_ == Behavior::kJump && isHit_)
	{
		worldTransform_.translation.y = 0.0f;
	}

	float animationTime[3] = { 0.0f,0.0f,0.0f };

	if (enemy_->GetHP() > 0 && isFinisherEffect)
	{
		//Animation
		animationTime[0] = modelFighterPAHead_->GetAnimationTime();
		animationTime[0] += 1.0f / 60.0f;
		//animationTime[0] = std::fmod(animationTime[0], modelFighterPAHead_->GetAnimation().duration);

		modelFighterPAHead_->SetAnimationTime(animationTime[0]);

		//Animation
		animationTime[1] = modelFighterLA_arm_->GetAnimationTime();
		animationTime[1] += 1.0f / 60.0f;
		//animationTime[1] = std::fmod(animationTime[1], modelFighterLA_arm_->GetAnimation().duration);

		modelFighterLA_arm_->SetAnimationTime(animationTime[1]);

		//Animation
		animationTime[2] = modelFighterRA_arm_->GetAnimationTime();
		animationTime[2] += 1.0f / 60.0f;
		//animationTime[2] = std::fmod(animationTime[2], modelFighterRA_arm_->GetAnimation().duration);

		modelFighterRA_arm_->SetAnimationTime(animationTime[2]);
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			animationTime[i] = 0.0f;

			modelFighterPAHead_->SetAnimationTime(animationTime[0]);
			modelFighterLA_arm_->SetAnimationTime(animationTime[1]);
			modelFighterRA_arm_->SetAnimationTime(animationTime[2]);
		}
	}

	DownAnimation();

	//パーティクルの更新
	particleSystem_->Update();

	//WorldTransform(Player)の更新
	worldTransform_.UpdateMatrixEuler();

	worldTransformBody_.UpdateMatrixEuler();
	worldTransformHead_.UpdateMatrixEuler();
	worldTransformL_arm_.UpdateMatrixEuler();
	worldTransformR_arm_.UpdateMatrixEuler();

	worldTransformAHead_.UpdateMatrixEuler();
	worldTransformAL_arm_.UpdateMatrixEuler();
	worldTransformAR_arm_.UpdateMatrixEuler();

	//Weaponの更新
	playerWeapon_->Update();

	//isEnemyHit_ = false;

	isHit_ = false;

	HPBarUpdate();

	if (guardGauge_ <= 50.0f)
	{
		GuardGaugeBarUpdate();
	}

	FinisherGaugeBarUpdate();

	if (workAttack_.isCPunch == true)
	{
		cancelCount_ = 1;
	}

	if (cancelCount_ == 1)
	{
		cancelTimer_--;

		if (cancelTimer_ < 0)
		{
			cancelCount_ = 0;
			cancelTimer_ = 60;
		}
	}

	ImGui::Begin("FinisherGauge");
	ImGui::DragFloat("FinisherGauge", &finisherGauge_, 1.0f);
	ImGui::SliderFloat3("WTFT", &worldTransform_.translation.x, -10.0f, 16.0f);
	ImGui::SliderFloat3("WTFR", &worldTransformBody_.rotation.x, 0.0f, 16.0f);
	ImGui::End();
}

void Player::Draw(const Camera& camera)
{
	//Playerの描画
	modelFighterBody_->Draw(worldTransformBody_, camera, 0);

	if (enemy_->GetHP() > 0 && isFinisherEffect)
	{
		modelFighterPAHead_->Draw(worldTransformAHead_, camera, 0);
		modelFighterLA_arm_->Draw(worldTransformAL_arm_, camera, 0);
		modelFighterRA_arm_->Draw(worldTransformAR_arm_, camera, 0);
	}
	else
	{
		modelFighterPHead_->Draw(worldTransformHead_, camera, 0);
		modelFighterL_arm_->Draw(worldTransformL_arm_, camera, 0);
		modelFighterR_arm_->Draw(worldTransformR_arm_, camera, 0);
	}

	if (!isDown_)
	{
		playerCursol_->Draw(worldTransform_, camera, 0);
	}

	//Weaponの描画
	if (workAttack_.isSwingDown || workAttack_.isMowDown || workAttack_.isPoke || workAttack_.isFinisher && finisherEffectTimer <= 0
		&& !isHitSwingDown_ && !isHitPoke_ && !isHitMowDown_ && !isDown_ && behaviorRequest_ != Behavior::kRoot)
	{
		playerWeapon_->Draw(camera);
	}
}

void Player::DrawSprite()
{
	if (HP_ >= 0)
	{
		hpBar_.sprite_->Draw();
	}

	guardGaugeBar_.sprite_->Draw();

	finisherGaugeBar_.sprite_->Draw();
}

void Player::HPBarUpdate()
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

void Player::GuardGaugeBarUpdate()
{
	guardGaugeBar_.size_ = { (guardGauge_ / maxGuardGauge_) * guardGaugeBarSize,7.0f };

	guardGaugeBar_.sprite_->SetSize(guardGaugeBar_.size_);

	guardGaugeBar_.sprite_->SetColor({ 0.0f, 0.5f, 1.0f, 1.0f });
}

void Player::FinisherGaugeBarUpdate()
{

	if (enemy_->GetIsDown() == false && enemy_->GetIsHitPunch())
	{
		finisherGauge_ += 3.0f;
	}

	if (enemy_->GetIsDown() == false && enemy_->GetIsHitSwingDown())
	{
		finisherGauge_ += 8.0f;
	}

	if (enemy_->GetIsDown() == false && enemy_->GetIsHitPoke())
	{
		finisherGauge_ += 6.0f;
	}

	if (enemy_->GetIsDown() == false && enemy_->GetIsHitMowDown())
	{
		finisherGauge_ += 8.0f;
	}

	if (enemy_->GetIsDown() == false && enemy_->GetIsHitThrow())
	{
		finisherGauge_ += 5.0f;
	}

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


void Player::HitStop(int milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Player::Reset()
{
	HP_ = maxHP_;

	guardGauge_ = 0.0f;

	for (int i = 0; i < 6; i++)
	{
		downAnimationTimer_[i] = 60;
	}

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
	workAttack_.isCPunch = false;
	workAttack_.isSwingDown = false;
	workAttack_.isPoke = false;
	workAttack_.isPokeRight = false;
	workAttack_.isPokeLeft = false;
	workAttack_.isMowDown = false;
	workAttack_.isFinisher = false;
	workAttack_.isJumpAttack = false;

	isThrow_ = false;

	finisherEffectTimer = 90;
	isFinisherEffect = false;
	finisherCount_ = 0;

	behavior_ = Behavior::kRoot;

	worldTransform_.Initialize();
	worldTransform_.translation = { -7.0f,0.0f,6.5f };

	worldTransformHead_.Initialize();
	worldTransformHead_.rotation.y = 0.0f;
	worldTransform_.rotation.y = 1.7f;

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

	workAttack_.translation = { 0.0f,2.5f,0.0f };
	workAttack_.rotation = { 0.0f,0.0f,0.0f };

	playerWeapon_->SetTranslation(workAttack_.translation);
	playerWeapon_->SetRotation(workAttack_.rotation);

	worldTransform_.UpdateMatrixEuler();

	worldTransformBody_.UpdateMatrixEuler();
	worldTransformHead_.UpdateMatrixEuler();
	worldTransformL_arm_.UpdateMatrixEuler();
	worldTransformR_arm_.UpdateMatrixEuler();
}

void Player::DrawParticle(const Camera& camera) 
{
	if (!isReset_)
	{
		particleModel_->Draw(particleSystem_.get(), camera);
	}
}


void Player::OnCollision(Collider* collider, float damage)
{
	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemy)
	{
		isHit_ = true;

		if (enemy_->GetIsThrow() == true && isDown_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 50.0f;
			HP_ -= damage;
			isEnemyHit_ = true;
			isHitThrow_ = true;
		}
	}

	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemyWeapon)
	{
		if (isGuard_ && worldTransform_.rotation.y == 1.7f)
		{
			audio_->SoundPlayMP3(guardSoundHandle_, false, 1.0f);
			worldTransform_.translation.x -= 0.3f;
			guardGauge_ += 2.0f;

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
			guardGauge_ += 2.0f;

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

		if (enemy_->GetIsAttack() == true && enemy_->GetIsSwingDown() == true && isDown_ == false 
			&& isGuard_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 30.0f;
			HP_ -= damage;
			isHitSwingDown_ = true;
			isShake_ = true;

			HitStop(100);
		}

		if (enemy_->GetIsAttack() == true && enemy_->GetIsPoke() == true && isDown_ == false
			&& isGuard_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 20.0f;
			HP_ -= damage;
			isHitPoke_ = true;
			isShake_ = true;

			HitStop(100);
		}

		if (enemy_->GetIsAttack() == true && enemy_->GetIsMowDown() == true && isDown_ == false
			&& isGuard_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 30.0f;
			HP_ -= damage;
			isHitMowDown_ = true;
			isShake_ = true;

			HitStop(100);
		}
	}
}

Vector3 Player::GetWorldPosition()
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld.m[3][0];
	pos.y = worldTransform_.matWorld.m[3][1];
	pos.z = worldTransform_.matWorld.m[3][2];
	return pos;
}

void Player::BehaviorRootInitialize()
{
	
}

void Player::BehaviorRootUpdate()
{
	//コントローラーの移動処理
	if (input_->GetJoystickState())
	{
		if (GamePlayScene::migrationTimer >= 150)
		{
			const float deadZone = 0.7f;
			bool isMove_ = false;
			float kCharacterSpeed = 0.1f;
			velocity_ = { 0.0f, 0.0f, 0.0f };

			//移動処理
			if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && worldTransform_.rotation.y == 4.6f && isDown_ == false && !isHit_)
			{
				kCharacterSpeed = 0.1f;
				velocity_.x = -0.3f;
				isMove_ = true;
				isGuard_ = false;
			}

			if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && worldTransform_.rotation.y == 1.7f && isDown_ == false && !isHit_)
			{
				kCharacterSpeed = 0.1f;
				velocity_.x = 0.3f;
				isMove_ = true;
				isGuard_ = false;
			}

			if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && worldTransform_.rotation.y == 1.7f && isDown_ == false)
			{
				isGuard_ = true;

				if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
				{
					kCharacterSpeed = 0.05f;
					velocity_.x = -0.3f;
					isMove_ = true;
				}

				if (isGuard_ && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
				{
					kCharacterSpeed = 0.0f;
					velocity_.x = 0.0f;
					isMove_ = false;
				}
			}

			if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && worldTransform_.rotation.y == 4.6f && isDown_ == false)
			{

				isGuard_ = true;

				if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
				{
					kCharacterSpeed = 0.05f;
					velocity_.x = 0.3f;
					isMove_ = true;
				}

				if (isGuard_ && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
				{
					kCharacterSpeed = 0.0f;
					velocity_.x = 0.0f;
					isMove_ = false;
				}
			}

			if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT))
			{
				isGuard_ = false;
			}

			if (isMove_)
			{
				velocity_ = Normalize(velocity_);
				velocity_ = Multiply(kCharacterSpeed, velocity_);

				// 平行移動
				worldTransform_.translation = Add(worldTransform_.translation, velocity_);

				worldTransform_.UpdateMatrixEuler();
			}

			Vector3 playerWorldPosition = GetWorldPosition();

			Vector3 enemyWorldPosition = enemy_->GetWorldPosition();

			if (enemyWorldPosition.x > playerWorldPosition.x)
			{
				worldTransform_.rotation.y = 1.7f;
			}

			if (enemyWorldPosition.x < playerWorldPosition.x)
			{
				worldTransform_.rotation.y = 4.6f;
			}
		}

		//ジャンプ
		if (input_->GetJoystickState())
		{
			if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && !input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && isDown_ == false)
			{
				behaviorRequest_ = Behavior::kJump;
			}
		}

		//投げ
		if (input_->GetJoystickState())
		{
			if (input_->IsPressButton(XINPUT_GAMEPAD_X) && input_->IsPressButton(XINPUT_GAMEPAD_Y) && isDown_ == false)
			{
				behaviorRequest_ = Behavior::kThrow;
				isThrow_ = true;
			}
		}

		//攻撃
		//通常攻撃
		if (input_->GetJoystickState())
		{
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && !input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && isDown_ == false)
			{
				audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
				behaviorRequest_ = Behavior::kAttack;
				workAttack_.isPunch = true;
			}
		}

		//振り下ろし攻撃
		if (input_->GetJoystickState())
		{
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && isDown_ == false)
			{
				audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
				behaviorRequest_ = Behavior::kAttack;
				workAttack_.isSwingDown = true;
			}
		}

		//突き攻撃
		if (input_->GetJoystickState())
		{
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && worldTransform_.rotation.y == 1.7f && isDown_ == false)
			{
				audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
				behaviorRequest_ = Behavior::kAttack;
				workAttack_.isPoke = true;
				workAttack_.isPokeRight = true;
			}

			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && worldTransform_.rotation.y == 4.6f && isDown_ == false)
			{
				audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
				behaviorRequest_ = Behavior::kAttack;
				workAttack_.isPoke = true;
				workAttack_.isPokeLeft = true;
			}
		}

		//薙ぎ払う攻撃
		if (input_->GetJoystickState())
		{
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && worldTransform_.rotation.y == 4.6f ||
				input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && worldTransform_.rotation.y == 1.7f && isDown_ == false)
			{
				audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
				behaviorRequest_ = Behavior::kAttack;
				workAttack_.isMowDown = true;
			}
		}

		//finisher
		if (input_->GetJoystickState())
		{
			if (finisherGauge_ >= maxFinisherGauge_ && input_->IsPressButtonEnter(XINPUT_GAMEPAD_LEFT_SHOULDER) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && worldTransform_.rotation.y == 4.6f ||
				finisherGauge_ >= maxFinisherGauge_ && input_->IsPressButtonEnter(XINPUT_GAMEPAD_LEFT_SHOULDER) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
				&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && worldTransform_.rotation.y == 1.7f && isDown_ == false)
			{
				audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
				behaviorRequest_ = Behavior::kAttack;
				workAttack_.isFinisher = true;
			}
		}
	}
}

void Player::BehaviorAttackInitialize()
{
	//通常攻撃
	if (workAttack_.isPunch)
	{
		worldTransformL_arm_.rotation.x = -1.3f;
		worldTransformR_arm_.rotation.x = 0.0f;
		worldTransformL_arm_.rotation.y = 0.0f;
		worldTransformR_arm_.rotation.y = 0.0f;
	}

	if (workAttack_.isCPunch)
	{
		worldTransformL_arm_.rotation.x = 0.0f;
		worldTransformR_arm_.rotation.x = -1.3f;
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

		playerWeapon_->SetTranslation(workAttack_.translation);
		playerWeapon_->SetRotation(workAttack_.rotation);
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

		playerWeapon_->SetTranslation(workAttack_.translation);
		playerWeapon_->SetRotation(workAttack_.rotation);
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

		playerWeapon_->SetTranslation(workAttack_.translation);
		playerWeapon_->SetRotation(workAttack_.rotation);
	}

	//Finisher
	if (workAttack_.isFinisher)
	{
		worldTransformL_arm_.rotation.x = 0.0f;
		worldTransformR_arm_.rotation.x = 0.0f;
		worldTransformL_arm_.rotation.y = 0.0f;
		worldTransformR_arm_.rotation.y = 0.0f;
		workAttack_.translation = { 0.0f,0.5f,0.0f };
		workAttack_.rotation = { 1.0f,0.0f,3.14f / 2.0f };

		playerWeapon_->SetTranslation(workAttack_.translation);
		playerWeapon_->SetRotation(workAttack_.rotation);
	}

	attackAnimationFrame = 0;
}

void Player::BehaviorAttackUpdate()
{
	//通常攻撃
	if (workAttack_.isPunch)
	{
		isGuard_ = false;
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
				behaviorRequest_ = Behavior::kRoot;
				workAttack_.stiffnessTimer = 60;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.isPunch = false;

				/*audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
				behaviorRequest_ = Behavior::kAttack;
				workAttack_.stiffnessTimer = 20;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.isPunch = false;
				workAttack_.isMowDown = true;*/
			}
		}

		if (isDown_)
		{
			behaviorRequest_ = Behavior::kRoot;
			workAttack_.stiffnessTimer = 60;
			worldTransformHead_.rotation.y = 0.0f;
			worldTransformBody_.rotation.y = 0.0f;
			worldTransformL_arm_.rotation.y = 0.0f;
			worldTransformR_arm_.rotation.y = 0.0f;
			workAttack_.isPunch = false;
		}

		if (input_->GetJoystickState())
		{
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && workAttack_.stiffnessTimer > 30)
			{
				audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
				behaviorRequest_ = Behavior::kAttack;
				attackAnimationFrame = 0;
				workAttack_.stiffnessTimer = 60;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.x = 0.0f;
				worldTransformR_arm_.rotation.x = -1.3f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;

				workAttack_.isPunch = false;
				workAttack_.isCPunch = true;
			}
		}

		attackAnimationFrame++;
	}

	if (workAttack_.isCPunch)
	{
		isGuard_ = false;

		if (attackAnimationFrame < 3.0f)
		{
			worldTransformBody_.rotation.y -= 0.1f;
		}
		else if (worldTransformBody_.rotation.y < 1.0f)
		{
			worldTransformBody_.rotation.y += 0.1f;
		}
		else
		{
			workAttack_.stiffnessTimer--;

			if (workAttack_.stiffnessTimer < 0)
			{
				behaviorRequest_ = Behavior::kRoot;
				workAttack_.stiffnessTimer = 60;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.isCPunch = false;

				/*audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
				behaviorRequest_ = Behavior::kAttack;
				workAttack_.stiffnessTimer = 20;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.isPunch = false;
				workAttack_.isMowDown = true;*/
			}
		}

		if (isDown_)
		{
			behaviorRequest_ = Behavior::kRoot;
			workAttack_.stiffnessTimer = 60;
			worldTransformHead_.rotation.y = 0.0f;
			worldTransformBody_.rotation.y = 0.0f;
			worldTransformL_arm_.rotation.y = 0.0f;
			worldTransformR_arm_.rotation.y = 0.0f;
			workAttack_.isCPunch = false;
		}

		if (input_->GetJoystickState())
		{
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && workAttack_.stiffnessTimer > 30)
			{
				audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
				behaviorRequest_ = Behavior::kAttack;
				attackAnimationFrame = 0;
				workAttack_.stiffnessTimer = 60;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.x = -1.3f;
				worldTransformR_arm_.rotation.x = -1.3f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.translation = { 0.0f,0.5f,0.0f };
				workAttack_.rotation = { 1.0f,0.0f,3.14f / 2.0f };

				playerWeapon_->SetTranslation(workAttack_.translation);
				playerWeapon_->SetRotation(workAttack_.rotation);

				workAttack_.isCPunch = false;
				workAttack_.isMowDown = true;
			}
		}

		attackAnimationFrame++;
	}

	//振り下ろし攻撃
	if (workAttack_.isSwingDown)
	{
		isGuard_ = false;
		if (attackAnimationFrame < 10)
		{
			worldTransformL_arm_.rotation.x -= 0.05f;
			worldTransformR_arm_.rotation.x -= 0.05f;

			workAttack_.rotation.x -= 0.05f;

			playerWeapon_->SetTranslation(workAttack_.translation);
			playerWeapon_->SetRotation(workAttack_.rotation);

		}
		else if (workAttack_.rotation.x < 2.0f)
		{
			worldTransformL_arm_.rotation.x += 0.1f;
			worldTransformR_arm_.rotation.x += 0.1f;

			workAttack_.translation.z += 0.05f;
			workAttack_.translation.y -= 0.05f;
			workAttack_.rotation.x += 0.1f;

			playerWeapon_->SetTranslation(workAttack_.translation);
			playerWeapon_->SetRotation(workAttack_.rotation);
			playerWeapon_->SetIsAttack(true);
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
				playerWeapon_->SetIsAttack(false);
				workAttack_.isSwingDown = false;
			}
		}
		else
		{
			workAttack_.stiffnessTimer--;
			workAttack_.isAttack = false;
			playerWeapon_->SetIsAttack(false);

			if (isDown_)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isAttack = false;
				playerWeapon_->SetIsAttack(false);
				workAttack_.isSwingDown = false;
			}
			
			if (workAttack_.stiffnessTimer <= 0)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isSwingDown = false;
			}
		}
		attackAnimationFrame++;
	}

	//突き攻撃
	if (workAttack_.isPoke)
	{
		isGuard_ = false;
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

			playerWeapon_->SetTranslation(workAttack_.translation);
			playerWeapon_->SetRotation(workAttack_.rotation);

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

			playerWeapon_->SetTranslation(workAttack_.translation);
			playerWeapon_->SetRotation(workAttack_.rotation);
			playerWeapon_->SetIsAttack(true);
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
				playerWeapon_->SetIsAttack(false);
				workAttack_.isPoke = false;
				workAttack_.isPokeRight = false;
				workAttack_.isPokeLeft = false;
			}
		}
		else
		{
			workAttack_.stiffnessTimer--;
			workAttack_.isAttack = false;
			playerWeapon_->SetIsAttack(false);

			if (isDown_)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isAttack = false;
				playerWeapon_->SetIsAttack(false);
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
		isGuard_ = false;
		if (attackAnimationFrame < 10)
		{
			worldTransformBody_.rotation.y -= 0.1f;

			workAttack_.rotation.x -= 0.05f;

			playerWeapon_->SetTranslation(workAttack_.translation);
			playerWeapon_->SetRotation(workAttack_.rotation);

		}
		else if (workAttack_.rotation.x <= 3.8f)
		{
			worldTransformBody_.rotation.y += 0.1f;

			workAttack_.rotation.x += 0.1f;

			playerWeapon_->SetTranslation(workAttack_.translation);
			playerWeapon_->SetRotation(workAttack_.rotation);
			playerWeapon_->SetIsAttack(true);
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
				playerWeapon_->SetIsAttack(false);
				workAttack_.isMowDown = false;
			}
		}
		else 
		{
			workAttack_.stiffnessTimer--;
			workAttack_.isAttack = false;
			playerWeapon_->SetIsAttack(false);

			if (isDown_)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isAttack = false;
				playerWeapon_->SetIsAttack(false);
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

	//Finisher
	if (workAttack_.isFinisher)
	{
		isGuard_ = false;

		if (finisherEffectTimer > 0)
		{
			finisherEffectTimer--;
			isFinisherEffect = true;
			worldTransform_.translation.z = 3.0f;
		}
		else if (attackAnimationFrame < 134)
		{
			isFinisherEffect = false;
			finisherCount_ = 1;
			worldTransform_.translation.z = 6.5f;

			worldTransformHead_.translation = { 0.0f,0.0f,0.0f };
			worldTransformHead_.rotation = { 0.0f,0.0f,0.0f };

			worldTransformL_arm_.rotation.x = -1.3f;
			worldTransformR_arm_.rotation.x = -1.3f;
			worldTransformL_arm_.rotation.y = 0.0f;
			worldTransformR_arm_.rotation.y = 0.0f;

			float rotationSpeed = 0.1f;
			rotationSpeed += 0.1f;
			worldTransformBody_.rotation.y += rotationSpeed;

			workAttack_.rotation.x += rotationSpeed;

			playerWeapon_->SetTranslation(workAttack_.translation);
			playerWeapon_->SetRotation(workAttack_.rotation);
			playerWeapon_->SetIsAttack(true);
			workAttack_.isAttack = true;

			if (isDown_)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				finisherEffectTimer = 90;
				workAttack_.isAttack = false;
				playerWeapon_->SetIsAttack(false);
				workAttack_.isFinisher = false;
				isFinisherEffect = false;
				finisherCount_ = 0;
				finisherGauge_ = 0.0f;
			}

		}
		else if (attackAnimationFrame >= 134 && attackAnimationFrame < 144)
		{
			worldTransformHead_.rotation.y = 0.0f;
			worldTransformBody_.rotation.y = 0.0f;

			worldTransformL_arm_.rotation.x = (float)std::numbers::pi;
			worldTransformR_arm_.rotation.x = (float)std::numbers::pi;
			workAttack_.translation = { 0.0f,2.5f,0.0f };
			workAttack_.rotation = { 0.0f,0.0f,0.0f };

			playerWeapon_->SetTranslation(workAttack_.translation);
			playerWeapon_->SetRotation(workAttack_.rotation);

			worldTransformL_arm_.rotation.x -= 0.05f;
			worldTransformR_arm_.rotation.x -= 0.05f;

			workAttack_.rotation.x -= 0.05f;

			playerWeapon_->SetTranslation(workAttack_.translation);
			playerWeapon_->SetRotation(workAttack_.rotation);
		}
		else if (workAttack_.rotation.x < 2.0f)
		{
			finisherCount_ = 2;

			worldTransformL_arm_.rotation.x += 0.1f;
			worldTransformR_arm_.rotation.x += 0.1f;

			workAttack_.translation.z += 0.05f;
			workAttack_.translation.y -= 0.05f;
			workAttack_.rotation.x += 0.1f;

			playerWeapon_->SetTranslation(workAttack_.translation);
			playerWeapon_->SetRotation(workAttack_.rotation);
			playerWeapon_->SetIsAttack(true);
			workAttack_.isAttack = true;

			if (isDown_)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				finisherEffectTimer = 90;
				workAttack_.isAttack = false;
				playerWeapon_->SetIsAttack(false);
				workAttack_.isFinisher = false;
				isFinisherEffect = false;
				finisherCount_ = 0;
				finisherGauge_ = 0.0f;
			}
		}
		else
		{
			workAttack_.stiffnessTimer--;
			workAttack_.isAttack = false;
			playerWeapon_->SetIsAttack(false);

			if (isDown_)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				finisherEffectTimer = 90;
				workAttack_.isAttack = false;
				playerWeapon_->SetIsAttack(false);
				workAttack_.isFinisher = false;
				isFinisherEffect = false;
				finisherCount_ = 0;
				finisherGauge_ = 0.0f;
			}

			if (workAttack_.stiffnessTimer <= 0)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				finisherEffectTimer = 90;
				workAttack_.isAttack = false;
				workAttack_.isFinisher = false;
				isFinisherEffect = false;
				finisherCount_ = 0;
				finisherGauge_ = 0.0f;
			}
		}
		attackAnimationFrame++;
	}
}

void Player::BehaviorJumpInitialize()
{
	worldTransform_.translation.y = 0.0f;

	const float kJumpFirstSpeed_ = 0.6f;

	velocity_.y = kJumpFirstSpeed_;
}

void Player::BehaviorJumpUpdate()
{
	worldTransform_.translation = Add(worldTransform_.translation, velocity_);

	const float kGravityAcceleration_ = 0.03f;

	Vector3 accelerationVector_ = { 0.0f,-kGravityAcceleration_,0.0f };

	velocity_ = Add(velocity_, accelerationVector_);

	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
		{
			workAttack_.isJumpAttack = true;
			worldTransformL_arm_.rotation.x = -1.3f;
			worldTransformR_arm_.rotation.x = -1.3f;
			worldTransformL_arm_.rotation.y = 0.0f;
			worldTransformR_arm_.rotation.y = 0.0f;

			attackAnimationFrame = 0;
		}
	}

	if (workAttack_.isJumpAttack)
	{
		if (attackAnimationFrame < 10)
		{
			worldTransformL_arm_.rotation.x -= 0.1f;
			worldTransformR_arm_.rotation.x -= 0.1f;
		}
		else if (worldTransformL_arm_.rotation.x > -0.8f && worldTransformR_arm_.rotation.x > -0.8f)
		{
			worldTransformL_arm_.rotation.x += 0.1f;
			worldTransformR_arm_.rotation.x += 0.1f;
		}
		attackAnimationFrame++;
		
	}

	if (worldTransform_.translation.y <= 0.0f)
	{
		behaviorRequest_ = Behavior::kRoot;
		workAttack_.isJumpAttack = false;
		worldTransformL_arm_.rotation.x = 0.0f;
		worldTransformR_arm_.rotation.x = 0.0f;
		worldTransform_.translation.y = 0.0f;
	}
}

void Player::BehaviorThrowInitialize()
{
	if (isThrow_)
	{
		worldTransformL_arm_.rotation.x = -1.3f;
		worldTransformR_arm_.rotation.x = -1.3f;
		worldTransformL_arm_.rotation.y = 0.0f;
		worldTransformR_arm_.rotation.y = 0.0f;
		attackAnimationFrame = 0;
	}
}

void Player::BehaviorThrowUpdate()
{
	//投げ
	if (isThrow_)
	{
		isGuard_ = false;
		if (attackAnimationFrame < 30)
		{
			worldTransformL_arm_.rotation.y -= 0.02f;
			worldTransformR_arm_.rotation.y += 0.02f;
		}
		else if (enemy_->GetIsPlayerHit() == true)
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

void Player::BehaviorStanInitialize()
{

}

void Player::BehaviorStanUpdate()
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

		worldTransformL_arm_.rotation.x = 0.0f;
		worldTransformL_arm_.rotation.y = 0.0f;

		worldTransformR_arm_.rotation.x = 0.0f;
		worldTransformR_arm_.rotation.y = 0.0f;
	}
}

void Player::FloatingGimmickInitialize()
{
	for (int i = 0; i < kMaxModelParts; i++)
	{
		floatingParameter_[i] = 0.0f;
	}
}

void Player::FloatingGimmickUpdate()
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

void Player::DownAnimation()
{
	//振り下ろし攻撃
	if (isHitSwingDown_ && enemy_->GetRotation().y == 1.7f)
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

	if (isHitSwingDown_ && enemy_->GetRotation().y == 4.6f)
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
	if (isHitPoke_ && enemy_->GetRotation().y == 1.7f)
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

	if (isHitPoke_ && enemy_->GetRotation().y == 4.6f)
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
	if (isHitMowDown_ && enemy_->GetRotation().y >= 1.7f && enemy_->GetRotation().y < 4.6f)
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

	if (isHitMowDown_ && enemy_->GetRotation().y >= 4.6f)
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
	if (isHitThrow_ && enemy_->GetRotation().y == 1.7f)
	{
		isDown_ = true;
		if (enemy_->GetAttackAnimationFrame() < 30)
		{
			worldTransformBody_.rotation.x += 0.01f;

		}
		else if (enemy_->GetThrowTimer() > 30)
		{
			worldTransformBody_.rotation.x -= 0.2f;
		}
		else if (enemy_->GetThrowTimer() <= 30)
		{
			worldTransform_.translation.x += 0.3f;
			worldTransformBody_.rotation.x -= 0.2f;
		}

		if (enemy_->GetIsThrow() == false)
		{
			downAnimationTimer_[4] = 60;
			isHitThrow_ = false;
			isDown_ = false;
			isEnemyHit_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	if (isHitThrow_ && enemy_->GetRotation().y == 4.6f)
	{
		isDown_ = true;
		if (enemy_->GetAttackAnimationFrame() < 30)
		{
			worldTransformBody_.rotation.x -= 0.01f;

		}
		else if (enemy_->GetThrowTimer() > 20)
		{
			worldTransformBody_.rotation.x -= 0.2f;
		}
		else if (enemy_->GetThrowTimer() <= 20)
		{
			worldTransform_.translation.x -= 0.3f;
			worldTransformBody_.rotation.x -= 0.2f;
		}

		if (enemy_->GetIsThrow() == false)
		{
			downAnimationTimer_[4] = 60;
			isHitThrow_ = false;
			isDown_ = false;
			isEnemyHit_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	for (int i = 0; i < 6; i++)
	{
		if (downAnimationTimer_[i] <= 0 && HP_ <= 0)
		{
			worldTransform_.translation.y -= 0.1f;

			worldTransformBody_.rotation.x = 4.7f;

			worldTransformL_arm_.rotation.x = 0.0f;
			worldTransformL_arm_.rotation.y = 0.0f;

			worldTransformR_arm_.rotation.x = 0.0f;
			worldTransformR_arm_.rotation.y = 0.0f;

			if (worldTransform_.translation.y <= -0.8f)
			{
				worldTransform_.translation.y = -0.8f;
			}
		}
	}
}







