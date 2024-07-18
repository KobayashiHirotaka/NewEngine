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
#include <random>
#include <numbers>

#include "Direction.h"
#include "EnemyBullet.h"

//前方宣言
class Player;

class Enemy : public IGame3dObject, public Collider
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

		//弾攻撃
		bool isShot = false;

		//finisher
		bool isFinisher = false;

		//ジャンプ攻撃
		bool isJumpAttack = false;
	};

	~Enemy();

	void Initialize()override;

	void Update()override;

	void Draw(const Camera& camera)override;

	void BoneDraw(const Camera& camera);

	void DrawSprite();

	void DrawBullet(const Camera& camera);

	void DrawParticle(const Camera& camera);

	void OnCollision(Collider* collider, float damage)override;

	//EnemyWeapon* GetEnemyWeapon() { return enemyWeapon_.get(); };

	void Reset();

	void ShootBullet(const Vector3& startPosition, const Vector3& velocity);

	void UpdateBullets();
	
	void DrawBullets(const Camera& camera);

#pragma region Getter

	//EnemyWeapon* GetEnemyWeapon() { return enemyWeapon_.get(); };

	uint32_t GetAnimationIndex() { return animationIndex_; };

	WorldTransform& GetWorldTransform()override { return worldTransform_; }

	Vector3 GetWorldPosition() override;

	Vector3 GetRotation() { return worldTransform_.rotation; };

	//bool GetIsPlayerHit() { return isPlayerHit_; };

	bool GetIsAttack() { return workAttack_.isAttack; };

	bool GetIsLightPunch() { return workAttack_.isLightPunch; };
	bool GetIsMiddlePunch() { return workAttack_.isMiddlePunch; };
	bool GetIsHighPunch() { return workAttack_.isHighPunch; };
	bool GetIsTCMiddlePunch() { return workAttack_.isTCMiddlePunch; };
	bool GetIsTCHighPunch() { return workAttack_.isTCHighPunch; };

	bool GetIsTackle() { return workAttack_.isTackle; };

	bool GetIsShot() { return workAttack_.isShot; };

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

	Direction GetDirection() { return enemyDirection_; };

	const std::vector<EnemyBullet*>& GetBullets() const{ return bullets_; };

#pragma endregion

#pragma region Setter

	void SetIsReset(bool isReset) { isReset_ = isReset; };

	//武器のSetter
	void SetTransform(Vector3 transform) { worldTransform_.translation = transform; };
	void SetRotation(Vector3 rotation) { worldTransform_.rotation = rotation; };

	void SetPlayer(Player* player) { player_ = player; };

#pragma endregion

private:
	void HitStop(int milliseconds);

	void UpdateAnimationTime(float animationTime, bool isLoop, float frameRate, int animationIndex,
		std::unique_ptr<Model>& modelFighterBody);

	void DownAnimation();

	int Random(int min_value, int max_value);

#pragma region UIの更新

	void HPBarUpdate();

	void GuardGaugeBarUpdate();

	void FinisherGaugeBarUpdate();

	void ComboNumberSpriteUpdate();

#pragma endregion

#pragma region 敵の行動

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

#pragma region 敵の基本パラメータ

	//behavior
	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	//現在のフレームでの位置
	Vector3 currentPosition_;

	//前のフレームでの位置
	Vector3 previousPosition_;

	//向いている方向
	Direction enemyDirection_ = Direction::Left;

	//再生するanimationの番号
	uint32_t animationIndex_ = 4;

	//行動のパターン
	int patternCount_ = 1;
	int moveTimer_ = 60;

	//ダウン演出の時間
	int downAnimationTimer_ = 60;

	//ガード演出の時間
	int guardAnimationTimer_ = 60;

	//リセットの時間
	int resetTimer_ = 60;

	//スタンの時間
	int stanTimer_ = 200;

	//必殺技
	int finisherEffectTimer_ = 90;
	int finisherCount_ = 0;

	//キャンセル
	int cancelCount_ = 0;
	int cancelTimer_ = 60;

	//コンボを食らっているとき
	int comboTimer_ = 60;
	int comboCount_ = 0;

	//当たり判定
	AABB aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };

#pragma endregion

#pragma region 敵の移動パラメータ

	//移動
	Vector3 velocity_ = {};

#pragma endregion

#pragma region 敵の攻撃パラメータ

	WorkAttack workAttack_;

	int attackTimer_ = 30;

	int jumpAttackTimer_ = 15;

	int throwTimer_ = 100;

	int attackAnimationFrame_;

#pragma endregion

#pragma region 敵のフラグ

	//当たっているかどうか
	bool isHit_ = false;

	//ダウンしているかどうか
	bool isDown_ = false;

	//攻撃しているかどうか
	bool isAttack_[5];

	//ガードしているかどうか
	bool isGuard_ = false;

	//プレイヤーと当たっているかどうか
	bool isPlayerHit_ = false;

	//各攻撃があたっているかどうか
	//通常攻撃
	bool isHitLightPunch_ = false;
	bool isHitMiddlePunch_ = false;
	bool isHitHighPunch_ = false;
	bool isHitTCMiddlePunch_ = false;
	bool isHitTCHighPunch_ = false;

	//技
	bool isHitTackle_ = false;

	bool isHitThrow_ = false;
	bool isThrow_ = false;

	//シェイクしているかどうか
	bool isShake_ = false;

	//リセットしているかどうか
	bool isReset_ = false;

	bool isFinisherEffect_ = false;

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

	//スプライト(コンボ表示)
	std::unique_ptr<Sprite>hitSprite_ = nullptr;
	uint32_t hitTextureHandle_;

	std::unique_ptr<Sprite>comboNumSprite_ = nullptr;
	uint32_t comboNumTextureHandle_;

	//サウンド
	uint32_t attackSoundHandle_ = 0u;
	uint32_t weaponAttackSoundHandle_ = 0u;
	uint32_t damageSoundHandle_ = 0u;
	uint32_t guardSoundHandle_ = 0u;

#pragma endregion

#pragma region その他

	//プレイヤー
	Player* player_ = nullptr;

	//パーティクル
	std::unique_ptr<ParticleModel> particleModel_ = nullptr;
	std::unique_ptr<ParticleSystem> particleSystem_ = nullptr;

	//敵の弾テスト用
	std::unique_ptr<Model> bulletModel_;
	std::vector<EnemyBullet*> bullets_;

	int shotTimer_ = 200;
#pragma endregion
};

