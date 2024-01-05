#pragma once
#include "Project/GameObject/Character/ICharacter.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Utility/Collision/Collider.h"
#include "Engine/Utility/Collision/CollisionConfig.h"

class Player;

class Enemy : public Collider, public ICharacter
{
public:
	void Initialize(const std::vector<Model*>& models)override;

	void Update()override;

	void Draw(const Camera& camera)override;

	WorldTransform& GetWorldTransform()override { return worldTransform_; }

	Vector3 GetWorldPosition() override;

	void OnCollision(Collider* collider, float damage)override;

	float GetHP() { return HP_; };

	void SetHP(float HP) { HP_ = HP; };

	void SetPlayer(Player* player) { player_ = player; };

	bool GetIsPlayerHit() { return isPlayerHit_; };

	void DownAnimation();

	void SetTransform(Vector3 transform) { worldTransform_.translation = transform; };

	void SetRotation(Vector3 rotation) { worldTransform_.rotation = rotation; };

	bool GetIsDown() { return isDown_; };

private:
	Input* input_ = nullptr;

	const WorldTransform* parent_ = nullptr;

	const Camera* camera_ = nullptr;

	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	Vector3 velocity_ = {};

	float destinationAngleY_ = 0.0f;

	const uint16_t kMaxModelParts = 2;

	float floatingParameter_[2];

	int floatingCycle_[2];

	float floatingAmplitude_;

	float HP_ = 100.0f;

	Player* player_ = nullptr;

	bool isPlayerHit_ = false;

	int downAnimationTimer_[6] = { 60,60,60,60,60,60 };
	bool isHitPunch_ = false;
	bool isHitSwingDown_ = false;
	bool isHitPoke_ = false;
	bool isHitMowDown_ = false;
	bool isHitThrow_ = false;
	bool isDown_ = false;
};
