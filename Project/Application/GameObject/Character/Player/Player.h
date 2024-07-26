#pragma once
#include "Application/GameObject/Character/ICharacter.h"

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

	void BoneDraw(const Camera& camera)override;

	void SpriteDraw()override;

	void ParticleDraw(const Camera& camera)override;

	void OnCollision(Collider* collider, float damage)override;

	void Reset()override;

	//行動関数
	void BehaviorRootInitialize()override;

	void BehaviorRootUpdate()override;

	void BehaviorAttackInitialize()override;

	void BehaviorAttackUpdate()override;

	void BehaviorJumpInitialize()override;

	void BehaviorJumpUpdate()override;

	void BehaviorStanInitialize()override;

	void BehaviorStanUpdate()override;

	//移動
	void Move();

	//攻撃
	void AttackStart(bool& isAttackType)override;

	void AttackEnd(bool& isAttackType)override;

	//当たり判定の初期化
	void ResetCollision()override;

	//当たり判定の設定
	void ConfigureCollision(Vector3 min, Vector3 max)override;

	//UIの更新関数
	void HPBarUpdate()override;

	void GuardGaugeBarUpdate()override;

	void FinisherGaugeBarUpdate()override;

	void ComboNumberSpriteUpdate()override;

	//Getter
	uint32_t GetAnimationIndex() { return animationIndex_; };

	WorldTransform& GetWorldTransform()override { return worldTransform_; }

	Vector3 GetWorldPosition() override;

	//Setter
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; };

private:
	void HitStop(int milliseconds);

	void PushEnemy(Vector3& enemyPosition, float pushSpeed);

	void UpdateAnimationTime(float animationTime, bool isLoop, float frameRate, 
		int animationIndex, std::unique_ptr<Model>& modelFighterBody)override;

	void DownAnimation()override;

	void DownAnimationEnd(int animationIndex, bool& isHitAttackType);

private:
	//敵
	Enemy* enemy_ = nullptr;

	//カーソル
	std::unique_ptr<Model> playerCursol_;
	WorldTransform worldTransformCursol_;

	//当たり判定
	AABB aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };

	//リソース
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

	//エディターのテスト
	std::vector<std::string> tabs;
	int tabCounter = 0;
};

