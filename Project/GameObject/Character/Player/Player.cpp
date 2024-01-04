#include "Player.h"
#include <cassert>
#include <numbers>
#include "Project/GameObject/Character/Enemy/Enemy.h"

void Player::Initialize(const std::vector<Model*>& models)
{
	//Inputのインスタンス
	input_ = Input::GetInstance();

	//WorldTransform(Player)の初期化
	ICharacter::Initialize(models);
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
	weapon_ = std::make_unique<Weapon>();
	weapon_->Initialize(models_[4]);
	weapon_->SetParent(&worldTransform_);

	//当たり判定の設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Player::Update()
{
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

	//Weaponの更新
	weapon_->Update();

	//WorldTransform(Player)の更新
	worldTransform_.UpdateMatrix();

	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

	ImGui::Begin("Player");
	ImGui::DragFloat3("rotation", &workAttack_.translation.x, 0.01f, -5.0f, 5.0f, "%.3f");
	ImGui::Text("timer %d", workAttack_.stiffnessTimer);
	ImGui::Text("count %d", workAttack_.count);
	ImGui::End();
}

void Player::Draw(const Camera& camera)
{
	//Playerの描画
	models_[kModelIndexBody]->Draw(worldTransformBody_, camera);
	models_[kModelIndexHead]->Draw(worldTransformHead_, camera);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, camera);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, camera);

	//Weaponの描画
	if (behavior_ == Behavior::kAttack)
	{
		weapon_->Draw(camera);
	}
}

void Player::OnCollision(Collider* collider)
{

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
	/*velocity_ = { 0.0f,0.0f,0.0f };*/
}

void Player::BehaviorRootUpdate()
{
	//コントローラーの移動処理
	if (input_->GetJoystickState())
	{
		const float deadZone = 0.7f;
		bool isMove_ = false;
		const float kCharacterSpeed = 0.1f;
		velocity_ = { 0.0f, 0.0f, 0.0f };

		//移動処理
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT))
		{
			velocity_.x = -0.3f;
			/*worldTransform_.rotation.y = 4.6f;*/
			isMove_ = true;
		}

		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT))
		{
			velocity_.x = 0.3f;
			/*worldTransform_.rotation.y = 1.7f;*/
			isMove_ = true;
		}

		if (isMove_)
		{
			velocity_ = Normalize(velocity_);
			velocity_ = Multiply(kCharacterSpeed, velocity_);

			// 平行移動
			worldTransform_.translation = Add(worldTransform_.translation, velocity_);

			worldTransform_.UpdateMatrix();
		}
	}

	//ジャンプ
	if (input_->GetJoystickState())
	{
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
		{
			behaviorRequest_ = Behavior::kJump;
		}
	}

	//投げ
	if (input_->GetJoystickState())
	{
		if (input_->IsPressButton(XINPUT_GAMEPAD_X) && input_->IsPressButton(XINPUT_GAMEPAD_Y))
		{
			behaviorRequest_ = Behavior::kThrow;
			isThrow_ = true;
		}
	}

	//攻撃
	//振り下ろし攻撃
	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && !input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
		{
			behaviorRequest_ = Behavior::kAttack;
			workAttack_.isPanch = true;
		}
	}

	//振り回し攻撃
	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
		{
			behaviorRequest_ = Behavior::kAttack;
			workAttack_.isPoke = true;
		}
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

void Player::BehaviorAttackInitialize()
{
	//振り下ろし攻撃
	if (workAttack_.isPanch)
	{
		worldTransformL_arm_.rotation.x = (float)std::numbers::pi;
		worldTransformR_arm_.rotation.x = (float)std::numbers::pi;
		workAttack_.translation = { 0.0f,2.5f,0.0f };
		workAttack_.rotation = { 0.0f,0.0f,0.0f };
	}

	//振り回し攻撃
	if (workAttack_.isPoke)
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
	//振り下ろし攻撃
	if (workAttack_.isPanch)
	{
		if (attackAnimationFrame < 10)
		{
			worldTransformL_arm_.rotation.x -= 0.05f;
			worldTransformR_arm_.rotation.x -= 0.05f;

			workAttack_.rotation.x -= 0.05f;

			weapon_->SetTranslation(workAttack_.translation);
			weapon_->SetRotation(workAttack_.rotation);

		}
		else if (workAttack_.rotation.x < 2.0f)
		{
			worldTransformL_arm_.rotation.x += 0.1f;
			worldTransformR_arm_.rotation.x += 0.1f;

			workAttack_.translation.z += 0.05f;
			workAttack_.translation.y -= 0.05f;
			workAttack_.rotation.x += 0.1f;

			weapon_->SetTranslation(workAttack_.translation);
			weapon_->SetRotation(workAttack_.rotation);
			weapon_->SetIsAttack(true);
		}
		else
		{
			workAttack_.stiffnessTimer--;
			workAttack_.isAttack = false;
			weapon_->SetIsAttack(false);

			if (workAttack_.stiffnessTimer <= 0)
			{
				behaviorRequest_ = Behavior::kRoot;
				workAttack_.stiffnessTimer = 20;
				workAttack_.isPanch = false;
			}
		}
		attackAnimationFrame++;
	}

	//振り回し攻撃
	if (workAttack_.isPoke)
	{
		if (attackAnimationFrame < 10)
		{
			worldTransformBody_.rotation.y -= 0.1f;

			workAttack_.rotation.x -= 0.05f;

			weapon_->SetTranslation(workAttack_.translation);
			weapon_->SetRotation(workAttack_.rotation);

		}
		else if (workAttack_.rotation.x <= 7.8f)
		{
			worldTransformBody_.rotation.y += 0.1f;

			workAttack_.rotation.x += 0.1f;

			weapon_->SetTranslation(workAttack_.translation);
			weapon_->SetRotation(workAttack_.rotation);
			weapon_->SetIsAttack(true);

			//キャンセル用の処理
			/*if (input_->GetJoystickState())
			{
				if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
				{
					behaviorRequest_ = Behavior::kAttack;
					worldTransformHead_.rotation.y = 0.0f;
					worldTransformBody_.rotation.y = 0.0f;
					worldTransformL_arm_.rotation.y = 0.0f;
					worldTransformR_arm_.rotation.y = 0.0f;
					workAttack_.isPanch = true;
					workAttack_.isPoke = false;
				}
			}*/

		}
		else 
		{
			workAttack_.stiffnessTimer--;
			workAttack_.isAttack = false;
			weapon_->SetIsAttack(false);

			if (workAttack_.stiffnessTimer <= 0)
			{
				behaviorRequest_ = Behavior::kRoot;
				worldTransformHead_.rotation.y = 0.0f;
				worldTransformBody_.rotation.y = 0.0f;
				worldTransformL_arm_.rotation.y = 0.0f;
				worldTransformR_arm_.rotation.y = 0.0f;
				workAttack_.stiffnessTimer = 20;
				workAttack_.isPoke = false;
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

	if (worldTransform_.translation.y <= 0.0f)
	{
		behaviorRequest_ = Behavior::kRoot;
		worldTransform_.translation.y = 0.0f;
	}
}

void Player::BehaviorThrowInitialize()
{
	worldTransformL_arm_.rotation.x = -1.3f;
	worldTransformR_arm_.rotation.x = -1.3f;
	worldTransformL_arm_.rotation.y = 0.0f;
	worldTransformR_arm_.rotation.y = 0.0f;
	attackAnimationFrame = 0;
}

void Player::BehaviorThrowUpdate()
{
	//投げ
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
			workAttack_.stiffnessTimer = 20;
			isThrow_ = false;
		}
	}
	attackAnimationFrame++;
}

void Player::BehaviorGuardInitialize()
{

}

void Player::BehaviorGuardUpdate()
{

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




