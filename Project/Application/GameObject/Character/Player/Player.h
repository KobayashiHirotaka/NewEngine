/**
 * @file Player.h
 * @brief プレイヤーの管理(移動、攻撃など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Application/GameObject/Character/ICharacter.h"
#include "Engine/Utility/AttackEditor/AttackEditor.h"

//前方宣言
class Enemy;

class Player : public ICharacter, public Collider
{
public:
	~Player();

	void Initialize()override;

	void Update()override;

	void Draw(const Camera& camera)override;

	void ImGui(const char* title)override;

	void DrawBone(const Camera& camera)override;

	void DrawSprite()override;

	void DrawParticle(const Camera& camera)override;

	void DrawCollision(const Camera& camera)override;

	void OnCollision(Collider* collider)override;

	void Reset()override;

	//行動関数
	void InitializeBehaviorRoot()override;

	void UpdateBehaviorRoot()override;

	void InitializeBehaviorAttack()override;

	void UpdateBehaviorAttack()override;

	void InitializeBehaviorJump()override;

	void UpdateBehaviorJump()override;

	void InitializeBehaviorStan()override;

	void UpdateBehaviorStan()override;

	//移動
	void Move();

	//攻撃
	void StartAttack(bool& isAttackType)override;

	void EndAttack(bool& isAttackType)override;

	void EvaluateAttackTiming()override;

	void ApplyDamage()override;

	//当たり判定の初期化
	void ResetCollision()override;

	//当たり判定の設定
	void ConfigureCollision(Vector3 min, Vector3 max)override;

	//UIの更新関数
	void UpdateHPBar()override;

	void UpdateGuardGaugeBar()override;

	void AdjustGuardGauge()override;

	void UpdateFinisherGaugeBar()override;

	void AdjustFinisherGauge(float value)override;

	void UpdateComboNumberSprite()override;

	void HitCombo();

	//Getter
	uint32_t GetAnimationIndex() { return animationIndex_; };

	WorldTransform& GetWorldTransform()override { return worldTransform_; }

	Vector3 GetWorldPosition() override;

	Vector3 GetRightHandJointWorldPosition();

	bool GetIsDirectionRight() { return isDirectionRight_; };

	bool GetIsFinisherEffect() { return isFinisherEffect_; };

	AABB GetAABB() { return aabb_; };

	//Setter
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; };

private:
	void PushEnemy(Vector3& enemyPosition, float pushSpeed);

	void UpdateAnimationTime(float animationTime, bool isLoop, float frameRate,
		int animationIndex, std::unique_ptr<Model>& modelFighterBody)override;

	void DownAnimation()override;

	void EndDownAnimation(int animationIndex, bool& isHitAttackType);

private:
	//敵
	Enemy* enemy_ = nullptr;

	//カーソル
	std::unique_ptr<Model> playerCursol_;
	WorldTransform worldTransformCursol_;

	//当たり判定
	AABB aabb_ = { {-0.3f,0.0f,-0.3f},{0.3f,1.0f,0.3f} };

	//向き
	bool isDirectionRight_ = false;

	bool isCancel_ = false;

	//Sprite(hp)
	UI hpBar_;
	const float barSpace_ = 15.6f;
	float barSize_ = 480.0f;

	//Sprite(ガードゲージ)
	UI guardGaugeBar_;
	const float guardGaugeBarSpace_ = 48.5f;
	float guardGaugeBarSize_ = 240.0f;

	//Sprite(必殺技ゲージ)
	UI finisherGaugeBar_;
	const float finisherGaugeBarSpace_ = 627.0f;
	float finisherGaugeBarSize_ = 240.0f;

	//キャラクターアイコンのSprite
	std::unique_ptr<Sprite>playerIconSprite_ = nullptr;
	uint32_t playerIconTextureHandle_ = 0;

	//Sprite(コンボ表示)
	std::unique_ptr<Sprite>hitSprite_ = nullptr;
	uint32_t hitTextureHandle_;

	std::unique_ptr<Sprite>comboNumSprite_ = nullptr;
	uint32_t comboNumTextureHandle_;

	//Sounds
	uint32_t attackSoundHandle_ = 0u;
	uint32_t weaponAttackSoundHandle_ = 0u;
	uint32_t damageSoundHandle_ = 0u;
	uint32_t guardSoundHandle_ = 0u;

	bool isFinisherInvincible_ = false;

	std::unique_ptr<LineBox> lineBox_ = nullptr;

	const float value_ = 0.7f;

	bool isFinisherEffect_ = false;
};

