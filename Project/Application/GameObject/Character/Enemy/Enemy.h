#pragma once
#include "Application/GameObject/Character/ICharacter.h"
#include "EnemyBullet.h"

//前方宣言
class Player;

class Enemy : public ICharacter, public Collider
{
public:
	~Enemy();

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

	//攻撃
	void AttackStart(bool& isAttackType)override;

	void AttackEnd()override;

	//UIの更新関数
	void HPBarUpdate()override;

	void GuardGaugeBarUpdate()override;

	void FinisherGaugeBarUpdate()override;

	void ComboNumberSpriteUpdate()override;

	//弾関係の関数
	void BulletShoot(const Vector3& startPosition, const Vector3& velocity);

	void BulletsUpdate();

	void BulletDraw(const Camera& camera);

	//コンボ
	void HitCombo();

	//Getter
	uint32_t GetAnimationIndex() { return animationIndex_; };

	WorldTransform& GetWorldTransform()override { return worldTransform_; }

	Vector3 GetWorldPosition() override;

	const std::vector<EnemyBullet*>& GetBullets() const{ return bullets_; };

	//Setter
	void SetPlayer(Player* player) { player_ = player; };

private:
	void HitStop(int milliseconds);

	void UpdateAnimationTime(float animationTime, bool isLoop, float frameRate, int animationIndex,
		float animationDuration, std::unique_ptr<Model>& modelFighterBody)override;

	void DownAnimation()override;

	int Random(int min_value, int max_value);

private:
	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	//行動のパターン
	int patternCount_ = 1;
	int moveTimer_ = 60;

	//当たり判定
	AABB aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };

	//プレイヤー
	Player* player_ = nullptr;

	//敵の弾テスト用
	std::unique_ptr<Model> bulletModel_;
	std::vector<EnemyBullet*> bullets_;

	int shotCooldownTimer_ = 0;

	bool hasShot_ = false;

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
};

