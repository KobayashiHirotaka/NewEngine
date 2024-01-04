#pragma once
#include "Project/GameObject/Character/ICharacter.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Utility/Collision/Collider.h"
#include "Engine/Utility/Collision/CollisionConfig.h"

#include "Project/GameObject/Weapon/Weapon.h"

class Player : public Collider, public ICharacter
{
public:
	static const int ComboNum = 3;

	enum class Behavior
	{
		kRoot,
		kAttack,
		kJump,
		kThrow,
		kGuard
	};

	struct WorkAttack
	{
		Vector3 translation;

		Vector3 rotation;

		uint32_t attackParameter = 0;

		int32_t comboIndex = 0;

		int32_t inComboPhase = 0;

		int count = 0;
		int pokeCount = 0;

		int stiffnessTimer = 20;

		bool comboNext = false;
		bool isAttack = false;
		bool isPanch = false;
		bool isPoke = false;
	};

	void Initialize(const std::vector<Model*>& models)override;

	void Update()override;

	void Draw(const Camera& camera)override;

	WorldTransform& GetWorldTransform()override { return worldTransform_; }

	Vector3 GetWorldPosition() override;

	void OnCollision(Collider* collider)override;

	Weapon* GetWeapon() { return weapon_.get(); };

	bool GetIsAttack() { return workAttack_.isAttack; };

	void BehaviorRootInitialize();

	void BehaviorRootUpdate();

	void BehaviorAttackInitialize();

	void BehaviorAttackUpdate();

	void BehaviorJumpInitialize();

	void BehaviorJumpUpdate();

	void FloatingGimmickInitialize();

	void FloatingGimmickUpdate();

	int GetHP() { return HP_; };

	void SetHP(int HP) { HP_ = HP; };

private:
	Input* input_ = nullptr;

	const WorldTransform* parent_ = nullptr;

	const Camera* camera_ = nullptr;

	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	Vector3 velocity_ = {};

	float speed_ = 0.3f;

	float destinationAngleY_ = 0.0f;

	const uint16_t kMaxModelParts = 2;

	float floatingParameter_[2];

	int floatingCycle_[2];

	float floatingAmplitude_;

	std::unique_ptr<Weapon> weapon_ = nullptr;

	WorkAttack workAttack_;

	int attackTimer = 30;

	int jumpAttackTimer_ = 15;

	bool isAttack_[4];

	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	int attackAnimationFrame;

	int HP_ = 100;
};

