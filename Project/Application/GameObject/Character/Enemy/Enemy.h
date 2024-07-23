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

	void ShootBullet(const Vector3& startPosition, const Vector3& velocity);

	void UpdateBullets();

	void DrawBullet(const Camera& camera);

#pragma region Getter

	uint32_t GetAnimationIndex() { return animationIndex_; };

	WorldTransform& GetWorldTransform()override { return worldTransform_; }

	Vector3 GetWorldPosition() override;

	Vector3 GetRotation() { return worldTransform_.rotation; };

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

	int attackTimer_ = 30;

	int attackAnimationFrame_;

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

	//敵の弾テスト用
	std::unique_ptr<Model> bulletModel_;
	std::vector<EnemyBullet*> bullets_;


	int shotCooldownTimer_ = 0;

	bool hasShot_ = false;

	//デバッグ用
	bool isDebug_ = false;

#pragma endregion
};

