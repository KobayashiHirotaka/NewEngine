/**
 * @file Enemy.h
 * @brief 敵の管理(移動、攻撃など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Application/GameObject/Character/BaseCharacter.h"
#include "EnemyBullet.h"

//前方宣言
class Player;

class Enemy : public BaseCharacter, public Collider
{
public:
	/// <summary>デストラクタ</summary>
	~Enemy();

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


	/// <summary>WorldPositionの取得</summary>
	Vector3 GetWorldPosition() override;


	//Getter
	//AnimationIndex
	uint32_t GetAnimationIndex() { return animationIndex_; };

	//WorldTransform
	WorldTransform& GetWorldTransform()override { return worldTransform_; }

	//AABB
	AABB GetAABB() { return aabb_; };

	//Bullets
	const std::vector<EnemyBullet*>& GetBullets() const{ return bullets_; };

	//Setter
	//Player
	void SetPlayer(Player* player) { player_ = player; };

	//IsKO
	void SetIsKO(bool isKO) { isKO_ = isKO; };

private:
	//アニメーション
	/// <summary>アニメーションの更新</summary>
	virtual void UpdateAnimationTime(float animationTime, bool isLoop, float frameRate,
		int animationIndex, std::unique_ptr<Model>& modelFighterBody)override;

	/// <summary>ダウンアニメーション</summary>
	virtual void DownAnimation()override;

	/// <summary>ダウンアニメーションの終了</summary>
	virtual void EndDownAnimation(int animationIndex, bool& isHitAttackType)override;

	/// <summary>指定した範囲内のランダムな整数を生成</summary>
	int Random(int min_value, int max_value);

	/// <summary>ランダムに移動方向を選択</summary>
	int RandomMove();

	/// <summary>ランダムに攻撃または移動を選択</summary>
	int RandomAttackOrMove();

	/// <summary>ランダムに弾攻撃または移動を選択</summary>
	int RandomBulletOrMove();
	
private:
	//行動のパターン
	int patternCount_ = 1;
	int moveTimer_ = 60;

	//プレイヤー
	Player* player_ = nullptr;

	//当たり判定
	AABB aabb_ = { {-0.3f,0.0f,-0.3f},{0.3f,1.0f,0.3f} };

	//弾
	std::unique_ptr<Model> bulletModel_;
	std::vector<EnemyBullet*> bullets_;

	//弾攻撃のクールダウン
	int shotCooldownTimer_ = 0;

	//弾を打ったか
	bool hasShot_ = false;

	//キャンセルかどうか
	bool isCancel_ = false;

	//ヒット時の音
	bool isHitAudio_ = false;

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
	std::unique_ptr<Sprite>enemyIconSprite_ = nullptr;
	uint32_t enemyIconTextureHandle_ = 0;

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

	std::unique_ptr<LineBox> lineBox_ = nullptr;

	//ガード状態か
	bool isGuardMode_ = false;

	//ガードタイマー(確定反撃用)
	int guardTimer_ = 4;

	//パターンカウントの定数
	const int kPatternCount_[8] = { 0,1,2,3,4,5,6,7 };
};

