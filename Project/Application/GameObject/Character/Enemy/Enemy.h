/**
 * @file Enemy.h
 * @brief 敵の管理(移動、攻撃など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Application/GameObject/Character/BaseCharacter.h"
#include "EnemyBullet.h"
#include "EnemyUI.h"

//前方宣言
class Player;

class Enemy : public BaseCharacter
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


	/// <summary>WorldPositionの取得</summary>
	Vector3 GetWorldPosition();


	//Getter
	//AnimationIndex
	uint32_t GetAnimationIndex() const { return animationIndex_; };

	//WorldTransform
	WorldTransform& GetWorldTransform() { return worldTransform_; };

	//Collider
	Collider* GetCollider() { return collider_.get(); };

	//AABB
	const AABB& GetAABB() const { return aabb_; };

	//Bullets
	const std::vector<std::unique_ptr<EnemyBullet>>& GetBullets() const { return bullets_; };

	//Setter
	//Player
	void SetPlayer(Player* player) { player_ = player; };

	//IsKO
	void SetIsKO(bool isKO) { isKO_ = isKO; };

private:
	//攻撃
	/// <summary>ダメージの適応</summary>
	virtual void ApplyDamage()override;

	/// <summary>コンボされているとき</summary>
	virtual void HitCombo()override;

	/// <summary>コンボカウントの更新</summary>
	virtual void ComboCountUpdate(const int kRecoveryTime)override;


	/// <summary>弾を発射</summary>
	void ShootBullet(const Vector3& startPosition, const Vector3& velocity);

	/// <summary>弾の更新</summary>
	void UpdateBullets();


	/// <summary>当たり判定</summary>
	void OnCollision(Collider* collider)override;

	/// <summary>当たり判定の初期化</summary>
	void ResetCollision()override;
	
private:
	//プレイヤー
	Player* player_ = nullptr;

	//当たり判定
	std::unique_ptr<Collider>collider_ = nullptr;
	AABB aabb_ = { {-0.3f,0.0f,-0.3f},{0.3f,1.0f,0.3f} };

	//UI
	std::unique_ptr<EnemyUI> enemyUI_;

	//弾
	std::unique_ptr<Model> bulletModel_;
	std::vector<std::unique_ptr<EnemyBullet>> bullets_;

	//弾攻撃のクールダウン
	int shotCooldownTimer_ = 0;

	//弾を打ったか
	bool hasShot_ = false;

	//キャンセルかどうか
	bool isCancel_ = false;

	//ヒット時の音
	bool isHitAudio_ = false;

	//サウンド
	uint32_t attackSoundHandle_ = 0u;
	uint32_t weaponAttackSoundHandle_ = 0u;
	uint32_t damageSoundHandle_ = 0u;
	uint32_t guardSoundHandle_ = 0u;
	float volume_ = 1.0f;

	//当たり判定描画用のLineBox
	std::unique_ptr<LineBox> lineBox_ = nullptr;


	//行動のパターン
	int patternCount_ = 1;
	int moveTimer_ = 60;


	//足の速さ
	const float kMaxFrontSpeed_ = 0.03f;
	const float kMaxBackSpeed_ = 0.02f;

	//ガード状態か
	bool isGuardMode_ = false;

	//ガードタイマー(確定反撃用)
	const int kGuardTime_ = 20;
	int guardTimer_ = kGuardTime_;

	//パターンカウントの定数
	const int kPatternCount_[8] = { 0,1,2,3,4,5,6,7 };

	//テスト
	bool isHitSecondAttack = false;
	bool isHitThirdAttack = false;
};

