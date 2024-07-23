#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/Model/IGame3dObject.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Utility/Collision/Collider.h"
#include "Engine/Utility/Collision/CollisionConfig.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/2D/Sprite/UI.h"
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/3D/Particle/ParticleSystem.h"

//前方宣言
class Enemy;

class Player : public IGame3dObject, public Collider
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

		int stiffnessTimer = 60;

		bool comboNext = false;

		//攻撃しているか
		bool isAttack = false;

		//弱攻撃
		bool isLightPunch = false;

		//中攻撃
		bool isMiddlePunch = false;

		//強攻撃
		bool isHighPunch = false;

		//中攻撃(ターゲットコンボ用)
		bool isTCMiddlePunch = false;

		//強攻撃(ターゲットコンボ用)
		bool isTCHighPunch = false;

		//タックル攻撃
		bool isTackle = false;

		//finisher
		bool isFinisher = false;

		//ジャンプ攻撃
		bool isJumpAttack = false;
	};

	~Player();

	void Initialize()override;

	void Update()override;

	void Draw(const Camera& camera)override;

	void ImGui(const char* Title)override;

	void BoneDraw(const Camera& camera);

	void DrawSprite();

	void DrawParticle(const Camera& camera);

	void OnCollision(Collider* collider, float damage)override;

	void Reset();

#pragma region Getter

	//PlayerWeapon* GetPlayerWeapon() { return playerWeapon_.get(); };

	uint32_t GetAnimationIndex() { return animationIndex_; };

	WorldTransform& GetWorldTransform()override { return worldTransform_; }

	Vector3 GetWorldPosition() override;

	Vector3 GetRotation() { return worldTransform_.rotation; };

	//bool GetIsEnemyHit() { return isEnemyHit_; };

	bool GetIsAttack() { return workAttack_.isAttack; };

	bool GetIsLightPunch() { return workAttack_.isLightPunch; };
	bool GetIsMiddlePunch() { return workAttack_.isMiddlePunch; };
	bool GetIsHighPunch() { return workAttack_.isHighPunch; };
	bool GetIsTCMiddlePunch() { return workAttack_.isTCMiddlePunch; };
	bool GetIsTCHighPunch() { return workAttack_.isTCHighPunch; };

	bool GetIsTackle() { return workAttack_.isTackle; };

	bool GetIsFinisher() { return workAttack_.isFinisher; };

	bool GetIsThrow() { return isThrow_; };

	int GetAttackAnimationFrame() { return attackAnimationFrame_; };

	int GetThrowTimer() { return throwTimer_; };

	bool GetIsDown() { return isDown_; };

	int GetFinisherEffectTimer() { return finisherEffectTimer_; };

	bool GetIsFinisherEffect() { return isFinisherEffect_; };

	int GetFinisherCount() { return finisherCount_; };

	int GetIsCancelCount() { return cancelCount_; };

	bool GetIsShake() { return isShake_; };

	bool GetIsHSVFilter() { return isHSVFilter_; };

	Direction GetDirection() { return playerDirection_; };

#pragma endregion

#pragma region Setter

	void SetIsReset(bool isReset) { isReset_ = isReset; };

	//武器のSetter
	void SetTransform(Vector3 transform) { worldTransform_.translation = transform; };
	void SetRotation(Vector3 rotation) { worldTransform_.rotation = rotation; };

	void SetEnemy(Enemy* enemy) { enemy_ = enemy; };

#pragma endregion

private:

	void HitStop(int milliseconds);

	void PushEnemy(Vector3& enemyPosition, float pushSpeed);

#pragma region Animation

	void UpdateAnimationTime(float animationTime, bool isLoop, float frameRate, int animationIndex,
		std::unique_ptr<Model>& modelFighterBody);

	void DownAnimation();

#pragma endregion

#pragma region UIの更新

	void HPBarUpdate();

	void GuardGaugeBarUpdate();

	void FinisherGaugeBarUpdate();

#pragma endregion

#pragma region プレイヤーの行動

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

#pragma endregion

private:
#pragma region インスタンス

	//modelManager
	ModelManager* modelManager_ = nullptr;

	//input
	Input* input_ = nullptr;

	//audio
	Audio* audio_ = nullptr;

#pragma endregion

#pragma region プレイヤーの基本パラメータ

	//behavior
	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	//現在のフレームでの位置
	Vector3 currentPosition_;

	//前のフレームでの位置
	Vector3 previousPosition_;

	//向いている方向
	Direction playerDirection_ = Direction::Right;

	//再生するanimationの番号
	uint32_t animationIndex_ = 4;
	float animationTime_ = 0.0f;

	//ダウン演出の時間
	int downAnimationTimer_ = 60;

	//ガード演出の時間
	int guardAnimationTimer_ = 60;

	//リセットの時間
	int resetTimer_ = 60;

	//スタンの時間
	int stanTimer_ = 60;

	//必殺技
	int finisherEffectTimer_ = 90;
	int finisherCount_ = 0;

	//キャンセル
	int cancelCount_ = 0;
	int cancelTimer_ = 60;

	//当たり判定
	AABB aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };

#pragma endregion

#pragma region プレイヤーの移動パラメータ

	//移動
	Vector3 velocity_ = {};

#pragma endregion

#pragma region プレイヤーの攻撃パラメータ

	WorkAttack workAttack_;

	int attackTimer_ = 30;

	int jumpAttackTimer_ = 15;

	int throwTimer_ = 100;

	int attackAnimationFrame_;

#pragma endregion

#pragma region プレイヤーのフラグ

	//当たっているかどうか
	bool isHit_ = false;

	//ダウンしているかどうか
	bool isDown_ = false;

	//攻撃しているかどうか
	bool isAttack_[5];

	//ガードしているかどうか
	bool isGuard_ = false;

	//敵と当たっているかどうか
	bool isEnemyHit_ = false;

	//各攻撃があたっているかどうか
	//通常攻撃
	bool isHitLightPunch_ = false;
	bool isHitMiddlePunch_ = false;
	bool isHitHighPunch_ = false;
	bool isHitTCMiddlePunch_ = false;
	bool isHitTCHighPunch_ = false;

	//技
	bool isHitTackle_ = false;

	//弾
	bool isHitBullet_ = false;
	bool isHitAirBullet_ = false;

	bool isHitThrow_ = false;
	bool isThrow_ = false;

	//シェイクしているかどうか
	bool isShake_ = false;

	//リセットしているかどうか
	bool isReset_ = false;

	//必殺技を発動しているかどうか
	bool isFinisherEffect_ = false;

	//HSVFilterをかけるかどうか
	bool isHSVFilter_ = false;

#pragma endregion

#pragma region リソース

	//スプライト(hp)
	UI hpBar_;
	const float barSpace_ = 15.6f;
	float barSize_ = 480.0f;

	//スプライト(ガードゲージ)
	UI guardGaugeBar_;
	const float guardGaugeBarSpace_ = 48.5f;
	float guardGaugeBarSize_ = 240.0f;

	//スプライト(必殺技ゲージ)
	UI finisherGaugeBar_;
	const float finisherGaugeBarSpace_ = 627.0f;
	float finisherGaugeBarSize_ = 240.0f;

	//サウンド
	uint32_t attackSoundHandle_ = 0u;
	uint32_t weaponAttackSoundHandle_ = 0u;
	uint32_t damageSoundHandle_ = 0u;
	uint32_t guardSoundHandle_ = 0u;

#pragma endregion

#pragma region その他

	//敵
	Enemy* enemy_ = nullptr;

	//カーソル
	std::unique_ptr<Model> playerCursol_;
	WorldTransform worldTransformCursol_;

	//パーティクル
	std::unique_ptr<ParticleModel> particleModel_ = nullptr;
	std::unique_ptr<ParticleSystem> particleSystem_ = nullptr;
	bool isParticle_ = false;

#pragma endregion
};

