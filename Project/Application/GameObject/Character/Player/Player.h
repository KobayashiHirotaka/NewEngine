#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/Model/IGame3dObject.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/3D/Particle/ParticleModel.h"
#include "Engine/3D/Particle/ParticleSystem.h"

class Player : public IGame3dObject
{
public:
	enum class Behavior
	{
		kRoot,
		kAttack,
		kJump,
		kThrow,
		kStan
	};

	enum class Direction
	{
		Left,
		Right
	};
	
	~Player();

	void Initialize()override;

	void Update()override;

	void Draw(const Camera& camera)override;

	void BoneDraw(const Camera& camera);

	void DrawParticle(const Camera& camera);

	uint32_t GetAnimationIndex() { return animationIndex; };

private:
#pragma region プレイヤーの行動関数

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

	void UpdateAnimationTime(float animationTime, bool isLoop, float frameRate, int animationIndex,
		std::unique_ptr<Model>& modelFighterBody);

private:
#pragma region インスタンス

	//modelManager
	ModelManager* modelManager_ = nullptr;

	//input
	Input* input_ = nullptr;

	//audio
	Audio* audio_ = nullptr;

#pragma endregion

#pragma region プレイヤーの基本パラメータ

	//behavior
	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	//向いている方向
	Direction playerDirection = Direction::Right;

	//再生するanimationの番号
	uint32_t animationIndex = 0;

	//hp
	float maxHP_ = 10.0f;
	float HP_ = maxHP_;

	//ガードゲージ
	float maxGuardGauge_ = 50.0f;
	float guardGauge_ = 0.0f;

	//必殺技のゲージ
	float maxFinisherGauge_ = 50.0f;
	float finisherGauge_ = 50.0f;

#pragma endregion

#pragma region プレイヤーの移動パラメータ

	//移動
	Vector3 velocity_ = {};
	float speed_ = 0.3f;

	//足の速さ(向いている方向に移動する場合)
	float characterFrontSpeed_ = 0.1f;

	//足の速さ(向いている方向とは逆に移動する場合)
	float characterBackSpeed_ = 0.05f;

#pragma endregion

#pragma region プレイヤーの攻撃パラメータ

#pragma endregion

#pragma region プレイヤーのフラグ

	//当たっているかどうか
	bool isHit_ = false;

	//ダウンしているかどうか
	bool isDown_ = false;

	//ガードしているかどうか
	bool isGuard_ = false;

#pragma endregion

#pragma region パーティクル

	//particle
	std::unique_ptr<ParticleModel> particleModel_ = nullptr;
	std::unique_ptr<ParticleSystem> particleSystem_ = nullptr;

#pragma endregion
};

