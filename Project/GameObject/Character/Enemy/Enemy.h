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

	void OnCollision(Collider* collider)override;

	int GetHP() { return HP_; };

	void SetHP(int HP) { HP_ = HP; };

	void SetPlayer(Player* player) { player_ = player; };

	bool GetIsPlayerHit() { return isPlayerHit_; };

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

	int HP_ = 100;

	Player* player_ = nullptr;

	bool isPlayerHit_ = false;
};
