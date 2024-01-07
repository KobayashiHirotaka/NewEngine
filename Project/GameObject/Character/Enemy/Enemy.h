#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Utility/Collision/Collider.h"
#include "Engine/Utility/Collision/CollisionConfig.h"
#include <random>

#include "Project/GameObject/Character/Enemy/EnemyWeapon.h"

class Player;

class Enemy : public Collider
{
public:
	enum class Behavior
	{
		kRoot,
		kAttack,
		kJump,
		kThrow,
	};

	struct WorkAttack
	{
		Vector3 translation;

		Vector3 rotation;

		uint32_t attackParameter = 0;

		int count = 0;
		int pokeCount = 0;

		int stiffnessTimer = 60;

		bool comboNext = false;

		//攻撃しているか
		bool isAttack = false;

		//パンチ
		bool isPunch = false;

		//振り下ろす
		bool isSwingDown = false;

		//突く
		bool isPoke = false;
		bool isPokeRight = false;
		bool isPokeLeft = false;

		//薙ぎ払う
		bool isMowDown = false;

		//ジャンプ攻撃
		bool isJumpAttack = false;

		//跳ね返す
		bool isReject = false;
	};

	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	WorldTransform& GetWorldTransform()override { return worldTransform_; }

	Vector3 GetWorldPosition() override;

	void OnCollision(Collider* collider, float damage)override;

	EnemyWeapon* GetEnemyWeapon() { return enemyWeapon_.get(); };
	
	bool GetIsAttack() { return workAttack_.isAttack; };

	bool GetIsPunch() { return workAttack_.isPunch; };

	bool GetIsSwingDown() { return workAttack_.isSwingDown; };

	bool GetIsPoke() { return workAttack_.isPoke; };

	bool GetIsMowDown() { return workAttack_.isMowDown; };

	bool GetIsThrow() { return isThrow_; };

	int GetAttackAnimationFrame() { return attackAnimationFrame; };

	int GetThrowTimer() { return throwTimer_; };

	void BehaviorRootInitialize();

	void BehaviorRootUpdate();

	void BehaviorAttackInitialize();

	void BehaviorAttackUpdate();

	void BehaviorJumpInitialize();

	void BehaviorJumpUpdate();

	void BehaviorThrowInitialize();

	void BehaviorThrowUpdate();

	void FloatingGimmickInitialize();

	void FloatingGimmickUpdate();

	float GetHP() { return HP_; };

	void SetHP(float HP) { HP_ = HP; };

	void SetPlayer(Player* player) { player_ = player; };

	bool GetIsPlayerHit() { return isPlayerHit_; };

	void DownAnimation();

	void SetTransform(Vector3 transform) { worldTransform_.translation = transform; };

	void SetRotation(Vector3 rotation) { worldTransform_.rotation = rotation; };

	bool GetIsDown() { return isDown_; };

	int Random(int min_value, int max_value);

	Vector3 GetRotation() { return worldTransform_.rotation; };

private:
	const WorldTransform* parent_ = nullptr;

	const Camera* camera_ = nullptr;

	WorldTransform worldTransform_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	std::unique_ptr<Model> modelFighterBody_;
	std::unique_ptr<Model> modelFighterPHead_;
	std::unique_ptr<Model> modelFighterL_arm_;
	std::unique_ptr<Model> modelFighterR_arm_;

	std::unique_ptr<Model> enemyWeaponModel_;

	Vector3 velocity_ = {};

	float speed_ = 0.3f;

	const uint16_t kMaxModelParts = 2;

	float floatingParameter_[2];

	int floatingCycle_[2];

	float floatingAmplitude_;

	std::unique_ptr<EnemyWeapon> enemyWeapon_ = nullptr;

	float HP_ = 100.0f;

	WorkAttack workAttack_;

	int attackTimer = 30;

	int jumpAttackTimer_ = 15;

	bool isAttack_[4];

	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	int attackAnimationFrame;

	Player* player_ = nullptr;

	int throwTimer_ = 140;

	bool isGuard_ = false;

	int pokeTimer_ = 30;

	bool isThrow_ = false;

	bool isPlayerHit_ = false;

	int downAnimationTimer_[6] = { 60,60,60,60,60,60 };
	bool isHitPunch_ = false;
	bool isHitSwingDown_ = false;
	bool isHitPoke_ = false;
	bool isHitMowDown_ = false;
	bool isHitThrow_ = false;
	bool isDown_ = false;

	int patternCount_ = 1;
	int moveTimer_ = 60;

	bool isHit_ = false;

	Vector3 currentPosition_;  // 現在のフレームでの位置
	Vector3 previousPosition_; // 前のフレームでの位置
};
