#include "Player.h"
#include <cassert>
#include <numbers>

const std::array<Player::ConstAttack, Player::ComboNum>
Player::kConstAttacks_ = {
	{
		{0,0,20,1,0.0f,0.0f,0.14f},
		{15,10,15,1,0.04f,0.0f,0.2f},
		{15,10,15,30,-0.04f,0.0f,0.2f}
	}
};

void Player::Initialize(const std::vector<Model*>& models)
{
	input_ = Input::GetInstance();

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

	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;

	weapon_ = std::make_unique<Weapon>();
	weapon_->Initialize(models_[4]);
	weapon_->SetParent(&worldTransform_);

	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	FloatingGimmickInitialize();
}

void Player::Update()
{
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
		}

		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_)
	{
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;

	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;

	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;
	}

	weapon_->Update();

	worldTransform_.UpdateMatrix();

	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

	ImGui::Begin("Player");
	ImGui::DragFloat3("rotation", &worldTransform_.rotation.x, 0.01f, -5.0f, 5.0f, "%.3f");
	ImGui::End();
}

void Player::Draw(const Camera& camera)
{
	models_[kModelIndexBody]->Draw(worldTransformBody_, camera);
	models_[kModelIndexHead]->Draw(worldTransformHead_, camera);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, camera);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, camera);

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
	velocity_ = { 0.0f,0.0f,0.0f };
}

void Player::BehaviorRootUpdate()
{
	FloatingGimmickUpdate();

	if (input_->GetJoystickState())
	{
		//コントローラーの移動処理
		const float deadZone = 0.7f;
		bool isMove_ = false;
		const float kCharacterSpeed = 0.1f;
		velocity_ = { 0.0f, 0.0f, 0.0f };

		//移動処理
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT))
		{
			velocity_.x = -0.3f;
			worldTransform_.rotation.y = 4.6f;
			isMove_ = true;
		}

		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT))
		{
			velocity_.x = 0.3f;
			worldTransform_.rotation.y = 1.7f;
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

	if (input_->GetJoystickState())
	{
		if (input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
		{
			behaviorRequest_ = Behavior::kJump;
		}
	}

	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
		{
			behaviorRequest_ = Behavior::kAttack;
		}
	}
}

void Player::BehaviorAttackInitialize()
{
	workAttack_.attackParameter = 0;
	workAttack_.comboIndex = 0;
	workAttack_.inComboPhase = 0;
	workAttack_.comboNext = false;
	workAttack_.isAttack = true;
	workAttack_.translation = { 0.0f,0.8f,0.0f };
	workAttack_.rotation = { 0.0f,0.0f,0.0f };
}

void Player::BehaviorAttackUpdate()
{
	if (workAttack_.comboIndex < ComboNum - 1)
	{
		if (input_->GetJoystickState())
		{
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
			{
				workAttack_.comboNext = true;
			}
		}
	}

	if (++workAttack_.attackParameter >= 40)
	{
		if (workAttack_.comboNext)
		{
			workAttack_.comboNext = false;
			workAttack_.attackParameter = 0;
			workAttack_.comboIndex++;

			weapon_->SetIsAttack(false);

			switch (workAttack_.comboIndex)
			{
			case 0:
				workAttack_.translation = { 0.0f,0.8f,0.0f };
				workAttack_.rotation = { 0.0f,0.0f,0.0f };
				break;

			case 1:
				workAttack_.translation = { 0.0f,0.8f,0.0f };
				workAttack_.rotation = { 1.0f,0.0f,3.14f / 2.0f };
				break;

			case 2:
				workAttack_.translation = { 0.0f,0.8f,0.0f };
				workAttack_.rotation = { 0.0f,0.0f,0.0f };
				break;
			}
		}
		else {
			behaviorRequest_ = Behavior::kRoot;
			workAttack_.isAttack = false;
			weapon_->SetIsAttack(false);
		}
	}

	uint32_t anticipationTime = kConstAttacks_[workAttack_.comboIndex].anticipationTime;
	uint32_t chargeTime = kConstAttacks_[workAttack_.comboIndex].anticipationTime + kConstAttacks_[workAttack_.comboIndex].chargeTime;
	uint32_t swingTime = kConstAttacks_[workAttack_.comboIndex].anticipationTime + kConstAttacks_[workAttack_.comboIndex].chargeTime + kConstAttacks_[workAttack_.comboIndex].swingTime;

	switch (workAttack_.comboIndex)
	{
	case 0:
		if (workAttack_.attackParameter < anticipationTime)
		{
			workAttack_.rotation.x += kConstAttacks_[workAttack_.comboIndex].anticipationSpeed;
		}

		if (workAttack_.attackParameter >= anticipationTime && workAttack_.attackParameter < chargeTime)
		{
			workAttack_.rotation.x += kConstAttacks_[workAttack_.comboIndex].chargeSpeed;
		}

		if (workAttack_.attackParameter >= chargeTime && workAttack_.attackParameter < swingTime)
		{
			workAttack_.rotation.x += kConstAttacks_[workAttack_.comboIndex].swingSpeed;
			weapon_->SetIsAttack(true);
		}

		if (workAttack_.attackParameter >= swingTime && workAttack_.attackParameter < 40) {
			weapon_->SetIsAttack(false);
		}

		worldTransformL_arm_.rotation.x += 0.3f;
		worldTransformR_arm_.rotation.x += 0.3f;

		weapon_->SetTranslation(workAttack_.translation);
		weapon_->SetRotation(workAttack_.rotation);
		break;

	case 1:
		if (workAttack_.attackParameter < anticipationTime)
		{
			workAttack_.rotation.x += kConstAttacks_[workAttack_.comboIndex].anticipationSpeed;
		}

		if (workAttack_.attackParameter >= anticipationTime && workAttack_.attackParameter < chargeTime)
		{
			workAttack_.rotation.x += kConstAttacks_[workAttack_.comboIndex].chargeSpeed;
		}

		if (workAttack_.attackParameter >= chargeTime && workAttack_.attackParameter < swingTime)
		{
			workAttack_.rotation.x += kConstAttacks_[workAttack_.comboIndex].swingSpeed;
			weapon_->SetIsAttack(true);
		}

		if (workAttack_.attackParameter >= swingTime && workAttack_.attackParameter < 40) {
			weapon_->SetIsAttack(false);
		}

		weapon_->SetTranslation(workAttack_.translation);
		weapon_->SetRotation(workAttack_.rotation);
		break;

	case 2:
		if (workAttack_.attackParameter < anticipationTime)
		{
			workAttack_.rotation.x += kConstAttacks_[workAttack_.comboIndex].anticipationSpeed;
		}

		if (workAttack_.attackParameter >= anticipationTime && workAttack_.attackParameter < chargeTime)
		{
			workAttack_.rotation.x += kConstAttacks_[workAttack_.comboIndex].chargeSpeed;
		}

		if (workAttack_.attackParameter >= chargeTime && workAttack_.attackParameter < swingTime)
		{
			workAttack_.rotation.x += kConstAttacks_[workAttack_.comboIndex].swingSpeed;
			weapon_->SetIsAttack(true);
		}

		if (workAttack_.attackParameter >= swingTime && workAttack_.attackParameter < 40) {
			weapon_->SetIsAttack(false);
		}

		weapon_->SetTranslation(workAttack_.translation);
		weapon_->SetRotation(workAttack_.rotation);

		break;
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




