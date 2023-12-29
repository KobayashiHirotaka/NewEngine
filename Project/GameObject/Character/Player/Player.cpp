#include "Player.h"
#include <cassert>
#include <numbers>

void Player::Initialize(const std::vector<Model*>& models)
{
	input_ = Input::GetInstance();

	attackModel_.reset(Model::CreateFromOBJ("resource/float_PHead", "playerHead.obj"));

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

	worldTransformCollision_[0].Initialize();
	worldTransformCollision_[0].translation.y = 2.0f;
	worldTransformCollision_[0].scale = { 1.2f,1.2f,1.2f };

	worldTransformCollision_[1].Initialize();
	worldTransformCollision_[1].translation.z = 1.5f;
	worldTransformCollision_[1].scale = { 1.2f,1.2f,1.2f };

	worldTransformCollision_[2].Initialize();
	worldTransformCollision_[2].translation.z = -1.5f;
	worldTransformCollision_[2].scale = { 1.2f,1.2f,1.2f };

	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;

	worldTransformCollision_[0].parent_ = &worldTransform_;
	worldTransformCollision_[1].parent_ = &worldTransform_;
	worldTransformCollision_[2].parent_ = &worldTransform_;

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

	FloatingGimmickUpdate();

	//振り降ろし
	if (input_->PushKey(DIK_J))
	{
		isAttack_[0] = true;
	}

	if (isAttack_[0] == true)
	{
		attackTimer--;
		worldTransformCollision_[0].translation.y -= 0.08f;
		worldTransformCollision_[0].translation.z += 0.1f;
		if (attackTimer < 0)
		{
			attackTimer = 30;
			worldTransformCollision_[0].translation = {0.0f,2.0f,0.0f};
			isAttack_[0] = false;
		}
	}

	//突き
	if (input_->PushKey(DIK_H))
	{
		isAttack_[1] = true;
	}

	if (isAttack_[1] == true)
	{
		attackTimer--;
		worldTransformCollision_[1].translation.z += 0.1f;
		if (attackTimer < 0)
		{
			attackTimer = 30;
			worldTransformCollision_[1].translation = { 0.0f,0.0f,1.5f };
			isAttack_[1] = false;
		}
	}

	//振り回し
	if (input_->PushKey(DIK_G))
	{
		isAttack_[2] = true;
	}

	if (isAttack_[2] == true)
	{
		attackTimer--;
		if (attackTimer > 0 && attackTimer < 15)
		{
			worldTransformCollision_[2].translation = { 0.0f,0.0f,1.5f };
		}

		if (attackTimer < 0)
		{
			attackTimer = 30;
			worldTransformCollision_[2].translation = { 0.0f,0.0f,-1.5f };
			isAttack_[2] = false;
		}
	}

	worldTransform_.UpdateMatrix();

	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

	worldTransformCollision_[0].UpdateMatrix();
	worldTransformCollision_[1].UpdateMatrix();
	worldTransformCollision_[2].UpdateMatrix();

	weapon_->Update();

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

	weapon_->Draw(camera);

	if (isAttack_[0] == true)
	{
		attackModel_->Draw(worldTransformCollision_[0], camera);
	}

	if (isAttack_[1] == true)
	{
		attackModel_->Draw(worldTransformCollision_[1], camera);
	}

	if (isAttack_[2] == true)
	{
		attackModel_->Draw(worldTransformCollision_[2], camera);
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
	if (input_->GetJoystickState())
	{
		//コントローラーの移動処理
	}
	else {
		bool isMove_ = false;
		float kCharacterSpeed = 0.1f;
		velocity_ = { 0.0f, 0.0f, 0.0f };

		//移動処理
		if (input_->PressKey(DIK_A))
		{
			velocity_.x = -0.03f;
			worldTransform_.rotation.y = 4.6f;
		}
		else if (input_->PressKey(DIK_D)) {
			velocity_.x = 0.03f;
			worldTransform_.rotation.y = 1.7f;
		}

		if (input_->PressKey(DIK_A) || input_->PressKey(DIK_D))
		{
			isMove_ = true;
			velocity_ = Normalize(velocity_);
			velocity_ = Multiply(kCharacterSpeed, velocity_);
		}


		if (isMove_)
		{
			// 平行移動
			worldTransform_.translation = Add(worldTransform_.translation, velocity_);

			worldTransform_.UpdateMatrix();

		}
	}

	
	if (input_->PushKey(DIK_W))
	{
		behaviorRequest_ = Behavior::kJump;
	}
	
}

void Player::BehaviorAttackInitialize()
{
	
}

void Player::BehaviorAttackUpdate()
{
	
}

void Player::BehaviorJumpInitialize()
{
	worldTransform_.translation.y = 0.0f;

	const float kJumpFirstSpeed_ = 0.6f;

	velocity_.y = kJumpFirstSpeed_;

	worldTransform_.DeleteParent();
}

void Player::BehaviorJumpUpdate()
{
	worldTransform_.translation = Add(worldTransform_.translation, velocity_);

	const float kGravityAcceleration_ = 0.03f;

	Vector3 accelerationVector_ = { 0.0f,-kGravityAcceleration_,0.0f };

	velocity_.y += accelerationVector_.y;

	worldTransform_.UpdateMatrix();

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




