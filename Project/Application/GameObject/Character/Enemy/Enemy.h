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

	void DrawBone(const Camera& camera)override;

	void DrawSprite()override;

	void DrawParticle(const Camera& camera)override;

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

	//UIの更新関数
	void HPBarUpdate()override;

	void GuardGaugeBarUpdate()override;

	void FinisherGaugeBarUpdate()override;

	void ComboNumberSpriteUpdate()override;

	//弾関係の関数
	void ShootBullet(const Vector3& startPosition, const Vector3& velocity);

	void UpdateBullets();

	void DrawBullet(const Camera& camera);

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
		std::unique_ptr<Model>& modelFighterBody);

	void DownAnimation();

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

	float animationTime_ = 0.0f;

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

	bool isParticle_ = false;
};

