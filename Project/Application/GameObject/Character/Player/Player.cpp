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

	IGame3dObject::SetTag("Player");

	AABB aabb = { {-1.0f,-1.0f,-10.0f},{1.0f,1.0f,10.0f} };
	SetAABB(aabb);

	modelFighterBody_.reset(Model::CreateFromOBJ("resource/models", "newPlayer.gltf"));

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
	worldTransform_.translation = { -7.0f,0.0f,0.0f };

	worldTransformHead_.Initialize();
	worldTransform_.rotation.y = 1.7f;

	worldTransformBody_.Initialize();
	worldTransformBody_.translation = { 0.0f,0.0f,0.0f };
	worldTransformBody_.rotation = { 7.75f,0.0f,0.0f };
	worldTransformBody_.scale = { 0.007f,0.007f,0.007f };

	worldTransformCursol_.Initialize();
	worldTransformCursol_.translation.x = 0.9f;
	worldTransformCursol_.translation.y = worldTransform_.translation.y + 2.0f;

	//親子付け
	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformHead_.parent_ = &worldTransformBody_;

	worldTransformCursol_.parent_ = &worldTransform_;
	
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

	attackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Attack.mp3");
	weaponAttackSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/WeaponAttack.mp3");
	damageSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Damage.mp3");
	guardSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Guard.mp3");

	animationIndex = 2;
}

void Player::Update()
{
	isShake_ = false;

	if (behaviorRequest_ == Behavior::kRoot && HP_ > 0)
	{
		animationIndex = 4;
	}
	else if (behaviorRequest_ == Behavior::kAttack && workAttack_.isSwingDown)
	{
		animationIndex = 3;
	}
	else if (behaviorRequest_ == Behavior::kAttack && workAttack_.isMowDown)
	{
		animationIndex = 1;
	}
	else if (behaviorRequest_ == Behavior::kJump)
	{
		animationIndex = 0;
	}
	else if (HP_ <= 0 || isDown_ || GamePlayScene::roundStartTimer_ <= 0)
	{
		animationIndex = 2;
	}

	modelFighterBody_->ApplyAnimation(animationIndex);

	modelFighterBody_->Update();


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

	//float animationTime[3] = { 0.0f,0.0f,0.0f };

	//if (enemy_->GetHP() > 0 && isFinisherEffect)
	//{
	//	//Animation
	//	animationTime[0] = modelFighterPAHead_->GetAnimationTime();
	//	animationTime[0] += 1.0f / 60.0f;
	//	//animationTime[0] = std::fmod(animationTime[0], modelFighterPAHead_->GetAnimation().duration);

	//	modelFighterPAHead_->SetAnimationTime(animationTime[0]);

	//	//Animation
	//	animationTime[1] = modelFighterLA_arm_->GetAnimationTime();
	//	animationTime[1] += 1.0f / 60.0f;
	//	//animationTime[1] = std::fmod(animationTime[1], modelFighterLA_arm_->GetAnimation().duration);

	//	modelFighterLA_arm_->SetAnimationTime(animationTime[1]);

	//	//Animation
	//	animationTime[2] = modelFighterRA_arm_->GetAnimationTime();
	//	animationTime[2] += 1.0f / 60.0f;
	//	//animationTime[2] = std::fmod(animationTime[2], modelFighterRA_arm_->GetAnimation().duration);

	//	modelFighterRA_arm_->SetAnimationTime(animationTime[2]);
	//}
	//else
	//{
	//	for (int i = 0; i < 3; i++)
	//	{
	//		animationTime[i] = 0.0f;

	//		modelFighterPAHead_->SetAnimationTime(animationTime[0]);
	//		modelFighterLA_arm_->SetAnimationTime(animationTime[1]);
	//		modelFighterRA_arm_->SetAnimationTime(animationTime[2]);
	//	}
	//}

	DownAnimation();

	//パーティクルの更新
	particleSystem_->Update();

	//WorldTransform(Player)の更新
	worldTransform_.UpdateMatrixEuler();

	worldTransformBody_.UpdateMatrixEuler();
	worldTransformHead_.UpdateMatrixEuler();

	worldTransformCursol_.UpdateMatrixEuler();

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

	ImGui::Begin("PlayerSpeed");
	ImGui::DragFloat("characterFrontSpeed", &characterFrontSpeed_, 0.1f);
	ImGui::DragFloat("characterBackSpeed", &characterBackSpeed_, 0.1f);
	ImGui::End();

	ImGui::Begin("FinisherGauge");
	ImGui::DragFloat("FinisherGauge", &finisherGauge_, 1.0f);
	ImGui::SliderFloat3("WTFT", &worldTransformBody_.translation.x, -10.0f, 16.0f);
	ImGui::SliderFloat3("WTFR", &worldTransformBody_.rotation.x, 0.0f, 16.0f);
	ImGui::End();
}

void Player::Draw(const Camera& camera)
{
	//Playerの描画
	modelFighterBody_->Draw(worldTransformBody_, camera, animationIndex);

	/*if (enemy_->GetHP() > 0 && isFinisherEffect)
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
	}*/

	if (!isDown_)
	{
		playerCursol_->Draw(worldTransformCursol_, camera, 0);
	}

	//Weaponの描画
	if (workAttack_.isAttack && workAttack_.isSwingDown || workAttack_.isMowDown || workAttack_.isPoke && !isHitSwingDown_
		&& !isHitPoke_ && !isHitMowDown_ && !isDown_ && behaviorRequest_ != Behavior::kRoot
		&& workAttack_.isAttack)
	{
		playerWeapon_->Draw(camera);
	}
}

void Player::BoneDraw(const Camera& camera)
{
	modelFighterBody_->BoneDraw(worldTransformBody_, camera, animationIndex);
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

	animationIndex = 2;

	behavior_ = Behavior::kRoot;

	worldTransform_.translation = { -7.0f,0.0f,0.0f };

	worldTransform_.rotation.y = 1.7f;

	worldTransformBody_.translation = { 0.0f,0.0f,0.0f };
	worldTransformBody_.rotation = { 7.75f,0.0f,0.0f };
	worldTransformBody_.scale = { 0.007f,0.007f,0.007f };

	worldTransformCursol_.translation.x = 0.9f;
	worldTransformCursol_.translation.y = worldTransform_.translation.y + 2.0f;

	workAttack_.translation = { 0.0f,2.5f,0.0f };
	workAttack_.rotation = { 0.0f,0.0f,0.0f };

	playerWeapon_->SetTranslation(workAttack_.translation);
	playerWeapon_->SetRotation(workAttack_.rotation);

	worldTransform_.UpdateMatrixEuler();

	worldTransformBody_.UpdateMatrixEuler();
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

		if (enemy_->GetIsPunch() == true && isDown_ == false)
		{
			audio_->SoundPlayMP3(damageSoundHandle_, false, 1.0f);
			damage = 15.0f;
			HP_ -= damage;
			isHitPunch_ = true;

			HitStop(20);
		}

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
			bool isFrontMove_ = false;
			bool isBackMove_ = false;

			float animationTime;

			velocity_ = { 0.0f, 0.0f, 0.0f };

			//移動処理
			if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && worldTransform_.rotation.y == 4.6f && isDown_ == false && !isHit_)
			{
				velocity_.x = -0.3f;
				isFrontMove_ = true;
				isGuard_ = false;
			}

			if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && worldTransform_.rotation.y == 1.7f && isDown_ == false && !isHit_)
			{
				velocity_.x = 0.3f;
				isFrontMove_ = true;
				isGuard_ = false;
			}

			if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && worldTransform_.rotation.y == 1.7f && isDown_ == false)
			{
				isGuard_ = true;

				if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
				{
					velocity_.x = -0.3f;
					isBackMove_ = true;
				}

				if (isGuard_ && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
				{
					velocity_.x = 0.0f;
					isBackMove_ = false;
				}
			}

			if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && worldTransform_.rotation.y == 4.6f && isDown_ == false)
			{

				isGuard_ = true;

				if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
				{
					velocity_.x = 0.3f;
					isBackMove_ = true;
				}

				if (isGuard_ && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
				{
					velocity_.x = 0.0f;
					isBackMove_ = false;
				}
			}

			if (!input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT))
			{
				isGuard_ = false;
			}

			if (isFrontMove_)
			{
				animationTime = modelFighterBody_->GetAnimationTime();
				if (!isDown_)
				{
					animationTime += 1.0f / 30.0f;
					animationTime = std::fmod(animationTime, modelFighterBody_->GetAnimation()[4].duration);
				}

				modelFighterBody_->SetAnimationTime(animationTime);

				modelFighterBody_->ApplyAnimation(4);

				modelFighterBody_->Update();

				velocity_ = Normalize(velocity_);
				velocity_ = Multiply(characterFrontSpeed_, velocity_);

				// 平行移動
				worldTransform_.translation = Add(worldTransform_.translation, velocity_);

				worldTransform_.UpdateMatrixEuler();
			}
			else if (isBackMove_)
			{
				animationTime = modelFighterBody_->GetAnimationTime();
				if (!isDown_)
				{
					animationTime += 1.0f / 60.0f;
					animationTime = std::fmod(animationTime, modelFighterBody_->GetAnimation()[4].duration);
				}

				modelFighterBody_->SetAnimationTime(animationTime);

				modelFighterBody_->ApplyAnimation(4);

				modelFighterBody_->Update();

				velocity_ = Normalize(velocity_);
				velocity_ = Multiply(characterBackSpeed_, velocity_);

				// 平行移動
				worldTransform_.translation = Add(worldTransform_.translation, velocity_);

				worldTransform_.UpdateMatrixEuler();
			}
			else
			{
				animationTime = modelFighterBody_->GetAnimationTime();
				if (!isDown_)
				{
					animationTime += 1.0f / 60.0f;
					animationTime = std::fmod(animationTime, modelFighterBody_->GetAnimation()[2].duration);
				}

				modelFighterBody_->SetAnimationTime(animationTime);

				modelFighterBody_->ApplyAnimation(2);

				modelFighterBody_->Update();
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
			////通常攻撃
			//if (input_->GetJoystickState())
			//{
			//	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B) && !input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
			//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
			//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && isDown_ == false)
			//	{
			//		audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
			//		behaviorRequest_ = Behavior::kAttack;
			//		workAttack_.isPunch = true;
			//	}
			//}

			//振り下ろし攻撃
			if (input_->GetJoystickState())
			{
				if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
					&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
					&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && isDown_ == false)
				{
					audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
					behaviorRequest_ = Behavior::kAttack;
					animationTime = 0.0f;
					modelFighterBody_->SetAnimationTime(animationTime);
					workAttack_.isSwingDown = true;
				}
			}

			////突き攻撃
			//if (input_->GetJoystickState())
			//{
			//	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
			//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
			//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && worldTransform_.rotation.y == 1.7f && isDown_ == false)
			//	{
			//		audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
			//		behaviorRequest_ = Behavior::kAttack;
			//		workAttack_.isPoke = true;
			//		workAttack_.isPokeRight = true;
			//	}

			//	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
			//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
			//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && worldTransform_.rotation.y == 4.6f && isDown_ == false)
			//	{
			//		audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
			//		behaviorRequest_ = Behavior::kAttack;
			//		workAttack_.isPoke = true;
			//		workAttack_.isPokeLeft = true;
			//	}
			//}

			////薙ぎ払う攻撃
			//if (input_->GetJoystickState())
			//{
			//	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
			//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
			//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && worldTransform_.rotation.y == 4.6f ||
			//		input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
			//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
			//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && worldTransform_.rotation.y == 1.7f && isDown_ == false)
			//	{
			//		audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
			//		behaviorRequest_ = Behavior::kAttack;
			//		animationTime = 0.0f;
			//		modelFighterBody_->SetAnimationTime(animationTime);
			//		workAttack_.isMowDown = true;
			//	}
			//}

			////finisher
			//if (input_->GetJoystickState())
			//{
			//	if (finisherGauge_ >= maxFinisherGauge_ && input_->IsPressButtonEnter(XINPUT_GAMEPAD_LEFT_SHOULDER) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
			//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
			//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && worldTransform_.rotation.y == 4.6f ||
			//		finisherGauge_ >= maxFinisherGauge_ && input_->IsPressButtonEnter(XINPUT_GAMEPAD_LEFT_SHOULDER) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
			//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
			//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && worldTransform_.rotation.y == 1.7f && isDown_ == false)
			//	{
			//		audio_->SoundPlayMP3(attackSoundHandle_, false, 1.0f);
			//		behaviorRequest_ = Behavior::kAttack;
			//		workAttack_.isFinisher = true;
			//	}
			//}
		}
	}
}

void Player::BehaviorAttackInitialize()
{
	//通常攻撃
	if (workAttack_.isPunch)
	{
		
	}

	if (workAttack_.isCPunch)
	{
		
	}

	//振り下ろし攻撃
	if (workAttack_.isSwingDown)
	{
		workAttack_.translation = { 0.0f,2.5f,0.0f };
		workAttack_.rotation = { 0.0f,0.0f,0.0f };

		playerWeapon_->SetTranslation(workAttack_.translation);
		playerWeapon_->SetRotation(workAttack_.rotation);
	}

	//突き攻撃
	if (workAttack_.isPoke)
	{
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
		workAttack_.translation = { 0.5f,2.5f,0.0f };
		workAttack_.rotation = { 1.0f,0.0f,3.14f / 2.0f };

		playerWeapon_->SetTranslation(workAttack_.translation);
		playerWeapon_->SetRotation(workAttack_.rotation);
	}

	//Finisher
	if (workAttack_.isFinisher)
	{
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
		float animationTime, animationDuration;
		animationTime = modelFighterBody_->GetAnimationTime();

		if (!isDown_)
		{
			animationTime += 1.0f / 50.0f;
			/*animationTime = std::fmod(animationTime, modelFighterBody_->GetAnimation()[0].duration);*/
		}

		animationDuration = modelFighterBody_->GetAnimation()[3].duration;

		modelFighterBody_->SetAnimationTime(animationTime);

		modelFighterBody_->ApplyAnimation(3);

		modelFighterBody_->Update();

		if (attackAnimationFrame < 10)
		{
			workAttack_.rotation.x -= 0.05f;

			playerWeapon_->SetTranslation(workAttack_.translation);
			playerWeapon_->SetRotation(workAttack_.rotation);

		}
		else if (workAttack_.rotation.x < 2.0f)
		{
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
				workAttack_.stiffnessTimer = 60;
				workAttack_.isAttack = false;
				playerWeapon_->SetIsAttack(false);
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
				workAttack_.stiffnessTimer = 60;
				workAttack_.isAttack = false;
				playerWeapon_->SetIsAttack(false);
				workAttack_.isMowDown = false;
			}


			if (animationTime >= animationDuration)
			{
				// アニメーションが終了した場合の処理
				behaviorRequest_ = Behavior::kRoot;
				workAttack_.stiffnessTimer = 60;
				worldTransformBody_.rotation.y = 0.0f;
				workAttack_.isMowDown = false;
				animationTime = 0.0f;
				modelFighterBody_->SetAnimationTime(animationTime);
			}

			/*if (workAttack_.stiffnessTimer <= 0)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isAttack = false;
				workAttack_.isMowDown = false;
			}*/
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

			workAttack_.translation = { 0.0f,2.5f,0.0f };
			workAttack_.rotation = { 0.0f,0.0f,0.0f };

			playerWeapon_->SetTranslation(workAttack_.translation);
			playerWeapon_->SetRotation(workAttack_.rotation);

			workAttack_.rotation.x -= 0.05f;

			playerWeapon_->SetTranslation(workAttack_.translation);
			playerWeapon_->SetRotation(workAttack_.rotation);
		}
		else if (workAttack_.rotation.x < 2.0f)
		{
			finisherCount_ = 2;

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
	float animationTime, animationDuration;
	animationTime = modelFighterBody_->GetAnimationTime();

	if (!isDown_)
	{
		animationTime += 1.0f / 60.0f;
		/*animationTime = std::fmod(animationTime, modelFighterBody_->GetAnimation()[0].duration);*/
	}

	animationDuration = modelFighterBody_->GetAnimation()[0].duration;

	modelFighterBody_->SetAnimationTime(animationTime);

	modelFighterBody_->ApplyAnimation(0);

	modelFighterBody_->Update();

	worldTransform_.translation = Add(worldTransform_.translation, velocity_);

	const float kGravityAcceleration_ = 0.03f;

	Vector3 accelerationVector_ = { 0.0f,-kGravityAcceleration_,0.0f };

	velocity_ = Add(velocity_, accelerationVector_);

	/*if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
		{
			workAttack_.isJumpAttack = true;

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
		
	}*/

	if (worldTransform_.translation.y <= 0.0f)
	{
		behaviorRequest_ = Behavior::kRoot;
		workAttack_.isJumpAttack = false;
		worldTransform_.translation.y = 0.0f;
		animationTime = 0.0f;
		modelFighterBody_->SetAnimationTime(animationTime);
	}
}

void Player::BehaviorThrowInitialize()
{
	if (isThrow_)
	{
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
		
		}
		else if (enemy_->GetIsPlayerHit() == true)
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
				behaviorRequest_ = Behavior::kRoot;
			
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
	}
}

void Player::DownAnimation()
{
	//通常攻撃
	if (isHitPunch_ && enemy_->GetRotation().y == 1.7f)
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

		if (enemy_->GetIsPunch() == false)
		{
			downAnimationTimer_[3] = 60;
			isHitPunch_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	if (isHitPunch_ && enemy_->GetRotation().y == 4.6f)
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

		if (enemy_->GetIsPunch() == false)
		{
			downAnimationTimer_[3] = 60;
			isHitPunch_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

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







