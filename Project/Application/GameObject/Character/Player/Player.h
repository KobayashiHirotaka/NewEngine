/**
 * @file Player.h
 * @brief プレイヤーの管理(移動、攻撃など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Application/GameObject/Character/BaseCharacter.h"
#include "Engine/Utility/AttackEditor/AttackEditor.h"
#include "PlayerBullet.h"

//前方宣言
class Enemy;

class Player : public BaseCharacter
{
public:
	/// <summary>デストラクタ</summary>
	~Player();

	//基本
	/// <summary>初期化</summary>
	virtual void Initialize()override;

	/// <summary>更新</summary>
	virtual void Update()override;

	/// <summary>描画</summary>
	virtual void Draw(const Camera& camera)override;

	/// <summary>骨の描画</summary>
	virtual void DrawBone(const Camera& camera)override;

	/// <summary>当たり判定の描画</summary>
	virtual void DrawCollision(const Camera& camera)override;

	/// <summary>スプライトの描画</summary>
	virtual void DrawSprite()override;

	/// <summary>パーティクルの描画</summary>
	virtual void DrawParticle(const Camera& camera)override;

	/// <summary>ImGui</summary>
	virtual void ImGui()override;

	/// <summary>リセット</summary>
	virtual void Reset()override;


	/// <summary>WorldPositionの取得</summary>
	Vector3 GetWorldPosition();

	/// <summary>右手のJoint座標の取得</summary>
	Vector3 GetRightHandJointWorldPosition();

	//Getter
	//AnimationIndex
	uint32_t GetAnimationIndex() { return animationIndex_; };

	//WorldTransform
	WorldTransform& GetWorldTransform() { return worldTransform_; }

	//Collider
	Collider* GetCollider() { return collider_.get(); }

	//IsDirectionRight
	bool GetIsDirectionRight() { return isDirectionRight_; };

	//IsFinisherEffect
	bool GetIsFinisherEffect() { return isFinisherEffect_; };

	//Bullets
	const std::vector<PlayerBullet*>& GetBullets() const { return bullets_; };

	//AABB
	AABB GetAABB() { return aabb_; };

	//Setter
	//Enemy
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; };

private:
	//キャラクターの状態
	/// <summary>移動状態の初期化</summary>
	virtual void InitializeBehaviorRoot()override;

	/// <summary>移動状態の更新</summary>
	virtual void UpdateBehaviorRoot()override;

	/// <summary>攻撃状態の初期化</summary>
	virtual void InitializeBehaviorAttack()override;

	/// <summary>攻撃状態の更新</summary>
	virtual void UpdateBehaviorAttack()override;

	/// <summary>ジャンプ状態の初期化</summary>
	virtual void InitializeBehaviorJump()override;

	/// <summary>ジャンプ状態の更新</summary>
	virtual void UpdateBehaviorJump()override;

	/// <summary>スタン状態の初期化</summary>
	virtual void InitializeBehaviorStan()override;

	/// <summary>ジャンプ状態の更新</summary>
	virtual void UpdateBehaviorStan()override;


	//移動
	/// <summary>移動</summary>
	virtual void Move()override;


	//攻撃
	/// <summary>攻撃の開始</summary>
	virtual void StartAttack(bool& isAttackType)override;

	/// <summary>攻撃の終了</summary>
	virtual void EndAttack(bool& isAttackType)override;

	/// <summary>攻撃タイミングを評価</summary>
	virtual void EvaluateAttackTiming()override;

	/// <summary>ダメージの適応</summary>
	virtual void ApplyDamage()override;

	/// <summary>コンボされているとき</summary>
	void HitCombo();


	/// <summary>弾を発射</summary>
	void ShootBullet(const Vector3& startPosition, const Vector3& velocity);

	/// <summary>弾の更新</summary>
	void UpdateBullets();


	/// <summary>当たり判定</summary>
	void OnCollision(Collider* collider)override;

	//当たり判定の初期化
	void ResetCollision()override;

	//当たり判定の設定
	void ConfigureCollision(Vector3 min, Vector3 max)override;


	//UIの更新
	/// <summary>HPのUIの更新</summary>
	virtual void UpdateHPBar()override;

	/// <summary>ガードゲージのUIの更新</summary>
	virtual void UpdateGuardGaugeBar()override;

	/// <summary>ガードゲージのUIの適応</summary>
	virtual void AdjustGuardGauge()override;

	/// <summary>必殺技ゲージのUIの適応</summary>
	virtual void UpdateFinisherGaugeBar()override;

	/// <summary>必殺技ゲージのUIの適応</summary>
	virtual void AdjustFinisherGauge(float value)override;

	/// <summary>コンボ表示のUIの更新</summary>
	virtual void UpdateComboNumberSprite()override;


	/// <summary>敵を押す</summary>
	void PushEnemy(Vector3& enemyPosition, float pushSpeed);

	//アニメーション
	/// <summary>アニメーションの更新</summary>
	virtual void UpdateAnimationTime(float animationTime, bool isLoop, float frameRate,
		int animationIndex, std::unique_ptr<Model>& modelFighterBody)override;

	/// <summary>ダウンアニメーション</summary>
	virtual void DownAnimation()override;

	/// <summary>ダウンアニメーションの終了</summary>
	virtual void EndDownAnimation(int animationIndex, bool& isHitAttackType)override;

private:
	//敵
	Enemy* enemy_ = nullptr;

	//当たり判定
	std::unique_ptr<Collider>collider_ = nullptr;
	AABB aabb_ = { {-0.3f,0.0f,-0.3f},{0.3f,1.0f,0.3f} };

	//カーソル
	std::unique_ptr<Model> playerCursol_;
	WorldTransform worldTransformCursol_;

	//弾
	std::unique_ptr<Model> bulletModel_;
	std::vector<PlayerBullet*> bullets_;

	//弾攻撃のクールダウン
	int shotCooldownTimer_ = 0;

	//弾を打ったか
	bool hasShot_ = false;

	//向き
	bool isDirectionRight_ = false;

	bool isCancel_ = false;

	//Sprite(hp)
	UI hpBar_;
	const float kBarSpace_ = 15.6f;
	float barSize_ = 480.0f;

	//Sprite(ガードゲージ)
	UI guardGaugeBar_;
	const float kGuardGaugeBarSpace_ = 48.5f;
	float guardGaugeBarSize_ = 240.0f;

	//Sprite(必殺技ゲージ)
	UI finisherGaugeBar_;
	const float kFinisherGaugeBarSpace_ = 627.0f;
	float finisherGaugeBarSize_ = 240.0f;

	//キャラクターアイコンのSprite
	std::unique_ptr<Sprite>playerIconSprite_ = nullptr;
	uint32_t playerIconTextureHandle_ = 0;

	//Sprite(コンボ表示)
	std::unique_ptr<Sprite>hitSprite_ = nullptr;
	uint32_t hitTextureHandle_;

	std::unique_ptr<Sprite>comboNumSprite_ = nullptr;
	uint32_t comboNumTextureHandle_;

	//サウンド
	uint32_t attackSoundHandle_ = 0u;
	uint32_t weaponAttackSoundHandle_ = 0u;
	uint32_t damageSoundHandle_ = 0u;
	uint32_t guardSoundHandle_ = 0u;
	float volume_ = 1.0f;

	//フィニッシャー中に表示するか
	bool isFinisherEffect_ = false;
	bool isFinisherInvincible_ = false;

	//LineBox
	std::unique_ptr<LineBox> lineBox_ = nullptr;

	//スティックの閾値
	const float kValue_ = 0.7f;
};

