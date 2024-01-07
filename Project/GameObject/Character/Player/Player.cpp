#include "Player.h"
#include <cassert>
#include <numbers>
#include "Project/GameObject/Character/Enemy/Enemy.h"

void Player::Initialize()
{
	//Inputのインスタンス
	input_ = Input::GetInstance();

	modelFighterBody_.reset(Model::CreateFromOBJ("resource/float_Body", "float_Body.obj"));
	modelFighterPHead_.reset(Model::CreateFromOBJ("resource/float_PHead", "playerHead.obj"));
	modelFighterL_arm_.reset(Model::CreateFromOBJ("resource/float_L_arm", "float_L_arm.obj"));
	modelFighterR_arm_.reset(Model::CreateFromOBJ("resource/float_R_arm", "float_R_arm.obj"));

	//WorldTransform(Player)の初期化
	worldTransform_.Initialize();
	worldTransform_.translation = { -7.0f,0.0f,0.0f };

	worldTransformHead_.Initialize();
	worldTransform_.rotation.y = 1.7f;

	worldTransformBody_.Initialize();
	worldTransformBody_.translation = { 0.0f,1.0f,0.0f };

	worldTransformL_arm_.Initialize();
	worldTransformL_arm_.translation.x = 0.5f;

	worldTransformR_arm_.Initialize();
	worldTransformR_arm_.translation.x = -0.5f;

	//親子付け
	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;

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
	worldTransform_.UpdateMatrix();

	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

void Player::Update()
{
	previousPosition_ = currentPosition_;
	// プレイヤーキャラクターの位置を更新
	currentPosition_ = worldTransform_.translation;

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
	playerWeapon_->Update();

	//パーティクルの更新
	particleSystem_->Update();

	//WorldTransform(Player)の更新
	worldTransform_.UpdateMatrix();

	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

	//isEnemyHit_ = false;

	isHit_ = false;

	ImGui::Begin("HP");
	ImGui::Text("%f", HP_);
	ImGui::End();

	ImGui::Begin("Player");
	ImGui::DragFloat3("rotation", &workAttack_.translation.x, 0.01f, -5.0f, 5.0f, "%.3f");
	ImGui::Text("timer %d", workAttack_.stiffnessTimer);
	ImGui::Text("count %d", workAttack_.count);
	ImGui::End();
}

void Player::Draw(const Camera& camera)
{
	//Playerの描画
	modelFighterBody_->Draw(worldTransformBody_, camera);
	modelFighterPHead_->Draw(worldTransformHead_, camera);
	modelFighterL_arm_->Draw(worldTransformL_arm_, camera);
	modelFighterR_arm_->Draw(worldTransformR_arm_, camera);

	//Weaponの描画
	if (workAttack_.isSwingDown || workAttack_.isMowDown || workAttack_.isPoke && !isHitSwingDown_
		&& !isHitPoke_ && !isHitMowDown_ && !isDown_ && behaviorRequest_ != Behavior::kRoot)
	{
		playerWeapon_->Draw(camera);
	}
}

void Player::DrawParticle(const Camera& camera) 
{
	particleModel_->Draw(particleSystem_.get(), camera);
}


void Player::OnCollision(Collider* collider, float damage)
{
	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemy)
	{
		isHit_ = true;

		if (enemy_->GetIsPunch() == true && isDown_ == false)
		{
			damage = 3.0f;
			HP_ -= damage;
			isHitPunch_ = true;
		}

		if (enemy_->GetIsThrow() == true && isDown_ == false)
		{
			damage = 5.0f;
			HP_ -= damage;
			isEnemyHit_ = true;
			isHitThrow_ = true;
		}

		ImGui::Begin("Aaa");

		ImGui::End();
	}

	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemyWeapon)
	{
		if (isGuard_ && worldTransform_.rotation.y == 1.7f)
		{
			worldTransform_.translation.x -= 0.3f;

			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kScale)
				.SetTranslation(worldTransform_.translation)
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.6f, 0.6f,0.6f }, { 1.0f ,1.0f ,1.0f })
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
			worldTransform_.translation.x += 0.3f;

			ParticleEmitter* newParticleEmitter = EmitterBuilder()
				.SetParticleType(ParticleEmitter::ParticleType::kScale)
				.SetTranslation(worldTransform_.translation)
				.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
				.SetScale({ 0.6f, 0.6f,0.6f }, { 1.0f ,1.0f ,1.0f })
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
			damage = 50.0f;
			HP_ -= damage;
			isHitSwingDown_ = true;
		}

		if (enemy_->GetIsAttack() == true && enemy_->GetIsPoke() == true && isDown_ == false
			&& isGuard_ == false)
		{
			damage = 50.0f;
			HP_ -= damage;
			isHitPoke_ = true;
		}

		if (enemy_->GetIsAttack() == true && enemy_->GetIsMowDown() == true && isDown_ == false
			&& isGuard_ == false)
		{
			damage = 50.0f;
			HP_ -= damage;
			isHitMowDown_ = true;
		}

		ImGui::Begin("A");

		ImGui::End();
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

		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && worldTransform_.rotation.y == 1.7f && isDown_ == false && !isHit_ )
		{
			kCharacterSpeed = 0.1f;
			velocity_.x = 0.3f;
			isMove_ = true;
			isGuard_ = false;
		}

		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT) && worldTransform_.rotation.y == 1.7f && isDown_ == false)
		{
			isGuard_ = true;

			if (!input_->IsPressButton(XINPUT_GAMEPAD_X))
			{
				kCharacterSpeed = 0.05f;
				velocity_.x = -0.3f;
				isMove_ = true;
			}

			if (isGuard_ && input_->IsPressButton(XINPUT_GAMEPAD_X))
			{
				kCharacterSpeed = 0.0f;
				velocity_.x = 0.0f;
				isMove_ = false;
			}
		}

		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT) && worldTransform_.rotation.y == 4.6f && isDown_ == false)
		{

			isGuard_ = true;

			if (!input_->IsPressButton(XINPUT_GAMEPAD_X))
			{
				kCharacterSpeed = 0.05f;
				velocity_.x = 0.3f;
				isMove_ = true;
			}

			if (isGuard_ && input_->IsPressButton(XINPUT_GAMEPAD_X))
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

			worldTransform_.UpdateMatrix();
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

		ImGui::Begin("Guard");
		ImGui::Text("%d", isGuard_);
		ImGui::End();
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
			behaviorRequest_ = Behavior::kAttack;
			workAttack_.isPoke = true;
			workAttack_.isPokeRight = true;
		}

		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
			&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
			&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP) && worldTransform_.rotation.y == 4.6f && isDown_ == false)
		{
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
			behaviorRequest_ = Behavior::kAttack;
			workAttack_.isMowDown = true;
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

	//振り下ろし攻撃
	if (workAttack_.isSwingDown)
	{
		worldTransformL_arm_.rotation.x = (float)std::numbers::pi;
		worldTransformR_arm_.rotation.x = (float)std::numbers::pi;
		workAttack_.translation = { 0.0f,2.5f,0.0f };
		workAttack_.rotation = { 0.0f,0.0f,0.0f };
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
	}

	attackAnimationFrame = 0;
}

void Player::BehaviorAttackUpdate()
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

			ImGui::Begin("rotate");
			ImGui::DragFloat3("rotation", &worldTransformBody_.rotation.x, 0.01f, -5.0f, 5.0f, "%.3f");
			ImGui::End();
		}
		else
		{
			workAttack_.stiffnessTimer--;

			if (workAttack_.stiffnessTimer < 0)
			{
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
				workAttack_.stiffnessTimer = 60;
				workAttack_.isSwingDown = false;
			}
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

			ImGui::Begin("rotate");
			ImGui::DragFloat3("rotation", &worldTransformBody_.rotation.x, 0.01f, -5.0f, 5.0f, "%.3f");
			ImGui::End();
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
	//通常攻撃
	if (isHitPunch_ && enemy_->GetRotation().y == 1.7f)
	{
		isDown_ = true;
		downAnimationTimer_[3]--;
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

	if (isHitSwingDown_ && enemy_->GetRotation().y == 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_[0]--;
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
	if (isHitPoke_ && enemy_->GetRotation().y == 1.7f)
	{
		isDown_ = true;
		downAnimationTimer_[1]--;
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

	if (isHitPoke_ && enemy_->GetRotation().y == 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_[1]--;
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
	if (isHitMowDown_ && enemy_->GetRotation().y >= 1.7f && enemy_->GetRotation().y < 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_[2]--;
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

	if (isHitMowDown_ && enemy_->GetRotation().y >= 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_[2]--;
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
}







