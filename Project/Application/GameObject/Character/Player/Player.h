/**
 * @file Player.h
 * @brief プレイヤーの管理(移動、攻撃など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Application/GameObject/Character/BaseCharacter.h"
#include "Application/GameObject/Character/State/Player/PlayerRootState.h"
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

	/// <summary>Stateの切り替え</summary>
	void ChangeState(std::unique_ptr<PlayerBaseState> state);


	/// <summary>移動</summary>
	void Move(const Vector3 velocity);


	//攻撃
	/// <summary>攻撃の終了</summary>
	virtual void EndAttack(bool& isAttackType)override;

	/// <summary>ダメージの適応</summary>
	virtual void ApplyDamage()override;

	/// <summary>コンボされているとき</summary>
	virtual void HitCombo()override;

	/// <summary>コンボカウントの更新</summary>
	virtual void ComboCountUpdate(const int kRecoveryTime)override;


	//当たり判定
	/// <summary>当たり判定の初期化</summary>
	void ResetCollision()override;


	//弾攻撃
	/// <summary>弾を発射</summary>
	void ShootBullet(const Vector3& startPosition, const Vector3& velocity);

	/// <summary>弾の更新</summary>
	void UpdateBullets();


	/// <summary>WorldPositionの取得</summary>
	Vector3 GetWorldPosition();

	/// <summary>右手のJoint座標の取得</summary>
	Vector3 GetRightHandJointWorldPosition();

	//Getter
	//AnimationIndex
	uint32_t GetAnimationIndex() const { return animationIndex_; };

	//WorldTransform
	WorldTransform& GetWorldTransform() { return worldTransform_; };

	//Collider
	Collider* GetCollider() const { return collider_.get(); };

	//IsDirectionRight
	bool GetIsDirectionRight() const { return isDirectionRight_; };

	//IsFinisherEffect
	bool GetIsFinisherEffect() const { return isFinisherEffect_; };

	//Bullets
	const std::vector<std::unique_ptr<PlayerBullet>>& GetBullets() const { return bullets_; };

	//Velocity
	Vector3 GetVelocity() const { return moveData_.velocity; };

	//Enemy
	Enemy* GetEnemy() const { return enemy_; };

	//AABB
	const AABB& GetAABB() const { return aabb_; };

	//Setter
	//Enemy
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; };

	//Velocity
	void SetVelocity(Vector3 velocity) { moveData_.velocity = velocity; };

	//CharacterState
	void SetIsGuard(bool isGuard) { characterState_.isGuard = isGuard; };

private:
	/// <summary>当たり判定</summary>
	void OnCollision(Collider* collider)override;


private:
	//敵
	Enemy* enemy_ = nullptr;

	//当たり判定
	std::unique_ptr<Collider> collider_ = nullptr;
	AABB aabb_ = { {-0.3f,0.0f,-0.3f},{0.3f,1.0f,0.3f} };

	//カーソル
	std::unique_ptr<Model> playerCursol_;
	WorldTransform worldTransformCursol_;

	//弾
	std::unique_ptr<Model> bulletModel_;
	std::vector<std::unique_ptr<PlayerBullet>> bullets_;

	//State
	std::unique_ptr<PlayerBaseState> currentState_;
	std::unique_ptr<PlayerBaseState> nextState_;

	//弾攻撃のクールダウン
	int shotCooldownTimer_ = 0;

	//弾を打ったか
	bool hasShot_ = false;

	//向き
	bool isDirectionRight_ = false;

	bool isCancel_ = false;

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

	//足の速さ
	const float kMaxFrontSpeed_ = 0.04f;
	const float kMaxBackSpeed_ = 0.04f;

	const float kStickDeadZone_ = 0.7f;
};

