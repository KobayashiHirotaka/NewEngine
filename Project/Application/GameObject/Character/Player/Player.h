#pragma once
#include "Engine/Base/WindowsApp/WindowsApp.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Utility/Collision/Collider.h"
#include "Engine/Utility/Collision/CollisionConfig.h"
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/3D/Particle/ParticleSystem.h"
#include "Engine/2D/Sprite/UI.h"
#include "Engine/3D/Model/IGame3dObject.h"

#include "Application/GameObject/Character/Player/PlayerWeapon.h"

class Enemy;

class Player : public Collider, public IGame3dObject
{
public:
	enum class Behavior
	{
		kRoot,
		kAttack,
		kJump,
		kThrow,
		kStan
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
		bool isCPunch = false;

		//振り下ろす
		bool isSwingDown = false;

		//突く
		bool isPoke = false;
		bool isPokeRight = false;
		bool isPokeLeft = false;

		//薙ぎ払う
		bool isMowDown = false;

		//finisher
		bool isFinisher = false;

		//ジャンプ攻撃
		bool isJumpAttack = false;
	};

	~Player();

	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void BoneDraw(const Camera& camera);

	WorldTransform& GetWorldTransform()override { return worldTransform_; }

	Vector3 GetWorldPosition() override;

	void OnCollision(Collider* collider, float damage)override;

	PlayerWeapon* GetPlayerWeapon() { return playerWeapon_.get(); };

	bool GetIsAttack() { return workAttack_.isAttack; };

	bool GetIsPunch() { return workAttack_.isPunch; };

	bool GetIsCPunch() { return workAttack_.isCPunch; };

	bool GetIsSwingDown() { return workAttack_.isSwingDown; };

	bool GetIsPoke() { return workAttack_.isPoke; };

	bool GetIsMowDown() { return workAttack_.isMowDown; };

	bool GetIsFinisher() { return workAttack_.isFinisher; };

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

	void BehaviorStanInitialize();

	void BehaviorStanUpdate();

	float GetHP() { return HP_; };

	void SetHP(float HP) { HP_ = HP; };

	void SetEnemy(Enemy* enemy) { enemy_ = enemy; };

	bool GetIsEnemyHit() { return isEnemyHit_; };

	void DownAnimation();

	void SetTransform(Vector3 transform) { worldTransform_.translation = transform; };

	void SetRotation(Vector3 rotation) { worldTransform_.rotation = rotation; };

	bool GetIsDown() { return isDown_; };

	Vector3 GetRotation() { return worldTransform_.rotation; };

	void DrawParticle(const Camera& camera);

	void DrawSprite();

	void HPBarUpdate();

	void GuardGaugeBarUpdate();

	void FinisherGaugeBarUpdate();

	void HitStop(int milliseconds);

	bool GetIsShake() { return isShake_; };

	void Reset();

	int GetFinisherEffectTimer() { return finisherEffectTimer; };

	bool GetIsFinisherEffect() { return isFinisherEffect; };

	int GetFinisherCount() { return finisherCount_; };

	int GetIsCancelCount() { return cancelCount_; };

	uint32_t GetANimationIndex() { return animationIndex; };

private:
	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	const WorldTransform* parent_ = nullptr;

	const Camera* camera_ = nullptr;

	WorldTransform worldTransform_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;

	WorldTransform worldTransformCursol_;

	std::unique_ptr<Model> modelFighterBody_;

	std::unique_ptr<Model> playerCursol_;

	Vector3 velocity_ = {};

	float speed_ = 0.3f;

	const uint16_t kMaxModelParts = 2;

	float maxHP_ = 10.0f;

	float HP_ = maxHP_;

	float maxGuardGauge_ = 50.0f;

	float guardGauge_ = 0.0f;

	float maxFinisherGauge_ = 50.0f;

	float finisherGauge_ = 50.0f;

	std::unique_ptr<PlayerWeapon> playerWeapon_ = nullptr;

	WorkAttack workAttack_;

	int attackTimer = 30;

	int jumpAttackTimer_ = 15;

	bool isAttack_[5];

	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	int attackAnimationFrame;

	Enemy* enemy_ = nullptr;

	int throwTimer_ = 100;

	bool isGuard_ = false;

	int pokeTimer_ = 30;

	bool isThrow_ = false;

	bool isEnemyHit_ = false;

	int downAnimationTimer_[6] = { 60,60,60,60,60,60 };
	bool isHitPunch_ = false;
	bool isHitCPunch_ = false;
	bool isHitSwingDown_ = false;
	bool isHitPoke_ = false;
	bool isHitMowDown_ = false;
	bool isHitThrow_ = false;
	bool isDown_ = false;

	bool isHit_ = false;

	Vector3 currentPosition_;  // 現在のフレームでの位置
	Vector3 previousPosition_; // 前のフレームでの位置

	//パーティクル
	std::unique_ptr<ParticleModel> particleModel_ = nullptr;
	std::unique_ptr<ParticleSystem> particleSystem_ = nullptr;

	UI hpBar_;
	const float barSpace = 16.0f;
	float barSize = 480.0f;

	UI guardGaugeBar_;
	const float guardGaugeBarSpace = 48.0f;
	float guardGaugeBarSize = 240.0f;

	UI finisherGaugeBar_;
	const float finisherGaugeBarSpace = 578.0f;
	float finisherGaugeBarSize = 240.0f;

	//サウンド
	uint32_t attackSoundHandle_ = 0u;
	uint32_t weaponAttackSoundHandle_ = 0u;
	uint32_t damageSoundHandle_ = 0u;
	uint32_t guardSoundHandle_ = 0u;

	bool isShake_ = false;

	int resetTimer_ = 60;
	bool isReset_ = false;

	int stanTimer_ = 200;

	int finisherEffectTimer = 90;

	bool isFinisherEffect = false;
	int finisherCount_ = 0;

	int cancelCount_ = 0;
	int cancelTimer_ = 60;

	uint32_t animationIndex = 0;

	//移動パラメーター
	//足の速さ(向いている方向に移動する場合)
	float characterFrontSpeed_ = 0.2f;

	//足の速さ(向いている方向とは逆に移動する場合)
	float characterBackSpeed_ = 0.15f;
};

