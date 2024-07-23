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

	void DrawBone(const Camera& camera)override;

	void DrawSprite()override;

	void DrawParticle(const Camera& camera)override;

	void OnCollision(Collider* collider, float damage)override;

	void Reset()override;

	uint32_t GetAnimationIndex() { return animationIndex_; };

	WorldTransform& GetWorldTransform()override { return worldTransform_; }

	Vector3 GetWorldPosition() override;

	Vector3 GetRotation() { return worldTransform_.rotation; };

#pragma region Setter

	void SetIsReset(bool isReset) { isReset_ = isReset; };

	//武器のSetter
	void SetTransform(Vector3 transform) { worldTransform_.translation = transform; };
	void SetRotation(Vector3 rotation) { worldTransform_.rotation = rotation; };

	void SetEnemy(Enemy* enemy) { enemy_ = enemy; };

#pragma endregion

private:

	void HitStop(int milliseconds);

	void PushEnemy(Vector3& enemyPosition, float pushSpeed);

#pragma region Animation

	void UpdateAnimationTime(float animationTime, bool isLoop, float frameRate, int animationIndex,
		std::unique_ptr<Model>& modelFighterBody);

	void DownAnimation();

#pragma endregion

#pragma region UIの更新

	void HPBarUpdate();

	void GuardGaugeBarUpdate();

	void FinisherGaugeBarUpdate();

#pragma endregion

#pragma region プレイヤーの行動

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

#pragma region プレイヤーの基本パラメータ


	//再生するanimationの番号
	uint32_t animationIndex_ = 4;
	float animationTime_ = 0.0f;

	//ダウン演出の時間
	int downAnimationTimer_ = 60;

	//ガード演出の時間
	int guardAnimationTimer_ = 60;

	//リセットの時間
	int resetTimer_ = 60;

	//スタンの時間
	int stanTimer_ = 60;

	//当たり判定
	AABB aabb_ = { {-0.3f,-0.3f,-0.3f},{0.3f,0.3f,0.3f} };

#pragma endregion

	//敵
	Enemy* enemy_ = nullptr;

	//カーソル
	std::unique_ptr<Model> playerCursol_;
	WorldTransform worldTransformCursol_;


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
};

