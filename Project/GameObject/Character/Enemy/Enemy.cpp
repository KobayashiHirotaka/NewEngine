#include "Enemy.h"
#include <cassert>
#include <numbers>
#include "Project/GameObject/Character/Player/Player.h"

void Enemy::Initialize(const std::vector<Model*>& models)
{
	input_ = Input::GetInstance();

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

	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Enemy::Update()
{
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

	DownAnimation();

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
	models_[kModelIndexBody]->Draw(worldTransformBody_, camera);
	models_[kModelIndexHead]->Draw(worldTransformHead_, camera);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, camera);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, camera);
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
			damage = 3.0f;
			HP_ -= damage;
			isHitThrow_ = true;
		}
		ImGui::Begin("Aaa");

		ImGui::End();
	}

	if (collider->GetCollisionAttribute() & kCollisionAttributeWeapon)
	{
		if (player_->GetIsAttack() == true && player_->GetIsSwingDown() == true && isDown_ == false)
		{
			damage = 7.0f;
			HP_ -= damage;
			isHitSwingDown_ = true;
		}

		if (player_->GetIsAttack() == true && player_->GetIsSwingDown() == true && isDown_ == false)
		{
			damage = 7.0f;
			HP_ -= damage;
			isHitSwingDown_ = true;
		}

		if (player_->GetIsAttack() == true && player_->GetIsPoke() == true && isDown_ == false)
		{
			damage = 10.0f;
			HP_ -= damage;
			isHitPoke_ = true;
		}

		if (player_->GetIsAttack() == true && player_->GetIsMowDown() == true && isDown_ == false)
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

		if (downAnimationTimer_[0] <= 0)
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

		if (downAnimationTimer_[0] <= 0)
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

		if (downAnimationTimer_[1] <= 0)
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

		if (downAnimationTimer_[1] <= 0)
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

		if (downAnimationTimer_[2] <= 0)
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

		if (downAnimationTimer_[2] <= 0)
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
		else if (player_->GetThrowTimer() > 20)
		{
			worldTransformBody_.rotation.x -= 0.2f;
		}
		else if (player_->GetThrowTimer() <= 20)
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


