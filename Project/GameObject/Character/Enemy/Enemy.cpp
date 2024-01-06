#include "Enemy.h"
#include <cassert>
#include <numbers>
#include "Project/GameObject/Character/Player/Player.h"

void Enemy::Initialize(const std::vector<Model*>& models)
{
	ICharacter::Initialize(models);
	worldTransform_.translation = { 3.0f,0.0f,0.0f };

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
	enemyWeapon_->Initialize(models_[4]);
	enemyWeapon_->SetParent(&worldTransform_);

	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Enemy::Update()
{
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

	DownAnimation();

	//Weaponの更新
	enemyWeapon_->Update();

	worldTransform_.UpdateMatrix();

	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

	isPlayerHit_ = false;

	ImGui::Begin("HP");
	ImGui::Text("%f", HP_);
	ImGui::End();
}

void Enemy::Draw(const Camera& camera)
{
	//Enemyの描画
	models_[kModelIndexBody]->Draw(worldTransformBody_, camera);
	models_[kModelIndexHead]->Draw(worldTransformHead_, camera);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, camera);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, camera);

	//Weaponの描画
	if (workAttack_.isSwingDown || workAttack_.isMowDown || workAttack_.isPoke && !isHitSwingDown_
		&& !isHitPoke_ && !isHitMowDown_ && !isDown_)
	{
		enemyWeapon_->Draw(camera);
	}
}

void Enemy::OnCollision(Collider* collider, float damage)
{
	if (collider->GetCollisionAttribute() & kCollisionAttributePlayer)
	{
		isPlayerHit_ = true;

		if (player_->GetIsPunch() == true && isDown_ == false)
		{
			damage = 3.0f;
			HP_ -= damage;
			isHitPunch_ = true;
		}

		if (player_->GetIsThrow() == true && isDown_ == false)
		{
			damage = 5.0f;
			HP_ -= damage;
			isHitThrow_ = true;
		}
		/*ImGui::Begin("Aaa");

		ImGui::End();*/
	}

	if (collider->GetCollisionAttribute() & kCollisionAttributePlayerWeapon)
	{
		if (player_->GetIsAttack() == true && player_->GetIsSwingDown() == true && isDown_ == false
			&& isGuard_ == false)
		{
			damage = 7.0f;
			HP_ -= damage;
			isHitSwingDown_ = true;
		}

		if (player_->GetIsAttack() == true && player_->GetIsPoke() == true && isDown_ == false
			&& isGuard_ == false)
		{
			damage = 10.0f;
			HP_ -= damage;
			isHitPoke_ = true;
		}

		if (player_->GetIsAttack() == true && player_->GetIsMowDown() == true && isDown_ == false
			&& isGuard_ == false)
		{
			damage = 10.0f;
			HP_ -= damage;
			isHitMowDown_ = true;
		}

		ImGui::Begin("A");

		ImGui::End();
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
	/*velocity_ = { 0.0f,0.0f,0.0f };*/
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
		if (moveTimer_ > 30 && worldTransform_.rotation.y == 4.6f)
		{
			kCharacterSpeed = 0.1f;
			velocity_.x = -0.3f;
			/*worldTransform_.rotation.y = 4.6f;*/
			isMove_ = true;
			isGuard_ = false;
		}

		if (moveTimer_ > 30 && worldTransform_.rotation.y == 1.7f)
		{
			kCharacterSpeed = 0.1f;
			velocity_.x = 0.3f;
			/*worldTransform_.rotation.y = 1.7f;*/
			isMove_ = true;
			isGuard_ = false;
		}

		if (moveTimer_ <= 30 && worldTransform_.rotation.y == 1.7f)
		{
			kCharacterSpeed = 0.05f;
			velocity_.x = -0.3f;
			/*worldTransform_.rotation.y = 4.6f;*/
			isMove_ = true;
			isGuard_ = true;
		}

		if (moveTimer_ <= 30 && worldTransform_.rotation.y == 4.6f)
		{
			kCharacterSpeed = 0.05f;
			velocity_.x = 0.3f;
			/*worldTransform_.rotation.y = 4.6f;*/
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
			moveTimer_ = 60;
			patternCount_ = Random(5, 7);
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

		ImGui::Begin("Guard");
		ImGui::Text("%d", isGuard_);
		ImGui::End();
	}

	isGuard_ = false;

	//ジャンプ
	if (patternCount_ == 2 && isDown_ == false)
	{
		behaviorRequest_ = Behavior::kJump;
	}

	//投げ
	if (patternCount_ == 3 && isDown_ == false)
	{
		behaviorRequest_ = Behavior::kThrow;
		isThrow_ = true;
	}

	//攻撃
	//通常攻撃
	if (patternCount_ == 4 && isDown_ == false)
	{
		behaviorRequest_ = Behavior::kAttack;
		workAttack_.isPunch = true;
	}

	////対空攻撃
	//if (input_->GetJoystickState())
	//{
	//	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT)
	//		&& !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT)
	//		&& input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
	//	{
	//		behaviorRequest_ = Behavior::kAttack;
	//		workAttack_.isAntiAir = true;
	//	}
	//}

	//振り下ろし攻撃
	if (patternCount_ == 5 && isDown_ == false)
	{
		behaviorRequest_ = Behavior::kAttack;
		workAttack_.isSwingDown = true;
	}

	//突き攻撃
	if (patternCount_ == 6 && isDown_ == false)
	{
		if (worldTransform_.rotation.y == 1.7f)
		{
			behaviorRequest_ = Behavior::kAttack;
			workAttack_.isPoke = true;
			workAttack_.isPokeRight = true;
		}
		

		if (worldTransform_.rotation.y == 4.6f)
		{
			behaviorRequest_ = Behavior::kAttack;
			workAttack_.isPoke = true;
			workAttack_.isPokeLeft = true;
		}
	}

	//薙ぎ払う攻撃
	if (patternCount_ == 7 && isDown_ == false)
	{
		behaviorRequest_ = Behavior::kAttack;
		workAttack_.isMowDown = true;
	}

	//跳ね返す攻撃
	if (patternCount_ == 8 && isDown_ == false)
	{
		if (worldTransform_.rotation.y == 4.6f || worldTransform_.rotation.y == 1.7f)
		{
			behaviorRequest_ = Behavior::kAttack;
			workAttack_.isReject = true;
		}
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
	}

	//突き攻撃
	if (workAttack_.isPoke)
	{
		worldTransformL_arm_.rotation.x = -1.3f;
		worldTransformR_arm_.rotation.x = -1.3f;
		workAttack_.translation = { 0.0f,0.5f,0.0f };
		workAttack_.rotation = { 1.5f,0.0f,0.0f };
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

	if (workAttack_.isReject)
	{
		worldTransformL_arm_.rotation.x = -1.3f;
		worldTransformR_arm_.rotation.x = -1.3f;
		worldTransformL_arm_.rotation.y = 0.0f;
		worldTransformR_arm_.rotation.y = 0.0f;
	}

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

			ImGui::Begin("rotate");
			ImGui::DragFloat3("rotation", &worldTransformBody_.rotation.x, 0.01f, -5.0f, 5.0f, "%.3f");
			ImGui::End();
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

			SetDamage(10.0f);
		}
		else
		{
			workAttack_.stiffnessTimer--;
			workAttack_.isAttack = false;
			enemyWeapon_->SetIsAttack(false);


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

			enemyWeapon_->SetTranslation(workAttack_.translation);
			enemyWeapon_->SetRotation(workAttack_.rotation);

		}
		else if (pokeTimer_ > 0)
		{
			if (workAttack_.isPokeRight)
			{
				workAttack_.rotation.z += 0.3f;
				worldTransform_.translation.x += 0.3f;

				/*if (worldTransform_.translation.x >= 7.0f)
				{
					worldTransform_.translation.
				}*/
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
		}
		else
		{
			workAttack_.stiffnessTimer--;
			workAttack_.isAttack = false;
			enemyWeapon_->SetIsAttack(false);

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
		}
		else
		{
			workAttack_.stiffnessTimer--;
			workAttack_.isAttack = false;
			enemyWeapon_->SetIsAttack(false);

			////キャンセル用の処理
			//if (input_->GetJoystickState())
			//{
			//	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
			//	{
			//		behaviorRequest_ = Behavior::kAttack;
			//		worldTransformHead_.rotation.y = 0.0f;
			//		worldTransformBody_.rotation.y = 0.0f;
			//		worldTransformL_arm_.rotation.y = 0.0f;
			//		worldTransformR_arm_.rotation.y = 0.0f;
			//		workAttack_.isAttack = false;
			//		workAttack_.isSwingDown = true;
			//		workAttack_.isMowDown = false;
			//	}
			//}

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

	//跳ね返す攻撃
	if (workAttack_.isReject)
	{
		if (attackAnimationFrame < 30)
		{
			worldTransformL_arm_.rotation.y += 0.02f;
			worldTransformR_arm_.rotation.y -= 0.02f;
		}
		else if (attackAnimationFrame >= 30 && attackAnimationFrame < 60 && player_->GetIsEnemyHit() == true)
		{
			ImGui::Begin("reject");
			ImGui::End();

			behaviorRequest_ = Behavior::kRoot;
			worldTransformL_arm_.rotation.y = 0.0f;
			worldTransformR_arm_.rotation.y = 0.0f;
			workAttack_.stiffnessTimer = 60;
			workAttack_.isReject = false;
		}
		else
		{
			workAttack_.stiffnessTimer--;

			if (workAttack_.stiffnessTimer <= 0)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 60;
				workAttack_.isReject = false;
			}
		}
		attackAnimationFrame++;
	}
}

void Enemy::BehaviorJumpInitialize()
{
	worldTransform_.translation.y = 0.0f;

	const float kJumpFirstSpeed_ = 0.6f;

	velocity_.y = kJumpFirstSpeed_;
}

void Enemy::BehaviorJumpUpdate()
{
	worldTransform_.translation = Add(worldTransform_.translation, velocity_);

	const float kGravityAcceleration_ = 0.03f;

	Vector3 accelerationVector_ = { 0.0f,-kGravityAcceleration_,0.0f };

	velocity_ = Add(velocity_, accelerationVector_);

	/*if (input_->GetJoystickState())
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
	}*/

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

void Enemy::DownAnimation()
{
	//通常攻撃
	if (isHitPunch_ && worldTransform_.rotation.y == 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_[3]--;
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

	if (isHitPunch_ && worldTransform_.rotation.y == 1.7f)
	{
		isDown_ = true;
		downAnimationTimer_[3]--;
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
	if (isHitSwingDown_ && worldTransform_.rotation.y == 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_[0]--;
		if (downAnimationTimer_[0] > 0)
		{
			worldTransform_.translation.x += 0.1f;
			worldTransformBody_.rotation.x -= 0.03f;
		}

		if (player_->GetIsSwingDown() == false)
		{
			downAnimationTimer_[0] = 60;
			isHitSwingDown_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	if (isHitSwingDown_ && worldTransform_.rotation.y == 1.7f)
	{
		isDown_ = true;
		downAnimationTimer_[0]--;
		if (downAnimationTimer_[0] > 0)
		{
			worldTransform_.translation.x -= 0.1f;
			worldTransformBody_.rotation.x -= 0.03f;
		}

		if (player_->GetIsSwingDown() == false)
		{
			downAnimationTimer_[0] = 60;
			isHitSwingDown_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	//突き攻撃
	if (isHitPoke_ && worldTransform_.rotation.y == 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_[1]--;
		if (downAnimationTimer_[1] > 0)
		{
			worldTransform_.translation.x += 0.3f;
			worldTransformBody_.rotation.x -= 0.03f;
		}

		if (player_->GetIsPoke() == false)
		{
			downAnimationTimer_[1] = 60;
			isHitPoke_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	if (isHitPoke_ && worldTransform_.rotation.y == 1.7f)
	{
		isDown_ = true;
		downAnimationTimer_[1]--;
		if (downAnimationTimer_[1] > 0)
		{
			worldTransform_.translation.x -= 0.3f;
			worldTransformBody_.rotation.x -= 0.03f;
		}

		if (player_->GetIsPoke() == false)
		{
			downAnimationTimer_[1] = 60;
			isHitPoke_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	//薙ぎ払い攻撃
	if (isHitMowDown_ && worldTransform_.rotation.y == 4.6f)
	{
		isDown_ = true;
		downAnimationTimer_[2]--;
		if (downAnimationTimer_[2] > 0)
		{
			worldTransform_.translation.x += 0.1f;
			worldTransformBody_.rotation.x -= 0.03f;
		}

		if (player_->GetIsMowDown() == false)
		{
			downAnimationTimer_[2] = 60;
			isHitMowDown_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	if (isHitMowDown_ && worldTransform_.rotation.y == 1.7f)
	{
		isDown_ = true;
		downAnimationTimer_[2]--;
		if (downAnimationTimer_[2] > 0)
		{
			worldTransform_.translation.x += 0.1f;
			worldTransformBody_.rotation.x -= 0.03f;
		}

		if (player_->GetIsMowDown() == false)
		{
			downAnimationTimer_[2] = 60;
			isHitMowDown_ = false;
			isDown_ = false;
			worldTransformBody_.rotation.x = 0.0f;
		}
	}

	//投げ攻撃
	if (isHitThrow_ && worldTransform_.rotation.y == 4.6f)
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

	if (isHitThrow_ && worldTransform_.rotation.y == 1.7f)
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

	return dis(gen); // ランダムな浮動小数点数を生成して返す
}


