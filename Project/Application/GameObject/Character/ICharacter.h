#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Utility/Collision/Collider.h"
#include "Engine/Utility/Collision/CollisionConfig.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/2D/Sprite/UI.h"
#include "Engine/3D/Particle/ParticleEffectPlayer.h"
#include "Engine/3D/Model/IGame3dObject.h"
#include <random>
#include <numbers>

class ICharacter : public IGame3dObject
{
public:
	enum class Direction
	{
		Left,
		Right
	};

	enum class Behavior
	{
		kRoot,
		kAttack,
		kJump,
		kStan
	};

	struct CharacterState
	{
		//現在の行動
		Behavior behavior = Behavior::kRoot;

		//行動のリクエスト
		std::optional<Behavior> behaviorRequest = std::nullopt;

		//向いている方向
		Direction direction = Direction::Right;

		//キャラクターと当たっているかどうか
		bool isHitCharacter = false;

		//ダウンしているかどうか
		bool isDown = false;

		//ガードしているかどうか
		bool isGuard = false;

		//各攻撃があたっているかどうか
		//通常攻撃
		bool isHitLightPunch = false;
		bool isHitMiddlePunch = false;
		bool isHitHighPunch = false;
		bool isHitTCMiddlePunch = false;
		bool isHitTCHighPunch = false;

		//技
		bool isHitTackle = false;

		//弾
		bool isHitBullet = false;
		bool isHitAirBullet = false;
	};

	struct EffectState
	{
		//シェイクしているかどうか
		bool isShake = false;

		//HSVFilterをかけるかどうか
		bool isHSVFilter = false;
	};

	struct MoveData
	{
		//移動
		Vector3 velocity = {};
	};

	struct AttackData
	{
		//攻撃フレーム
		int attackAnimationFrame = 0;

		//攻撃しているか
		bool isAttack = false;

		//弱攻撃
		bool isLightPunch = false;

		//中攻撃
		bool isMiddlePunch = false;

		//強攻撃
		bool isHighPunch = false;

		//中攻撃(ターゲットコンボ用)
		bool isTCMiddlePunch = false;

		//強攻撃(ターゲットコンボ用)
		bool isTCHighPunch = false;

		//タックル攻撃
		bool isTackle = false;

		//弾攻撃
		bool isShot = false;
	};

	struct TimerData
	{
		//ダウン演出の時間
		int downAnimationTimer = 60;

		//ガード演出の時間
		int guardAnimationTimer = 60;

		//スタンの時間
		int stanTimer = 60;

		//コンボの猶予時間
		int comboTimer = 60;
	};

	virtual void DrawBone(const Camera& camera) = 0;

	virtual void DrawSprite() = 0;

	virtual void DrawParticle(const Camera& camera) = 0;

	virtual void Reset() = 0;

	//キャラクターの行動関数
	virtual void BehaviorRootInitialize() = 0;

	virtual void BehaviorRootUpdate() = 0;

	virtual void BehaviorAttackInitialize() = 0;
	 
	virtual void BehaviorAttackUpdate() = 0;

	virtual void BehaviorJumpInitialize() = 0;

	virtual void BehaviorJumpUpdate() = 0;

	virtual void BehaviorStanInitialize() = 0;

	virtual void BehaviorStanUpdate() = 0;

	//UIの更新関数
	virtual void HPBarUpdate() = 0;

	virtual void GuardGaugeBarUpdate() = 0;

	virtual void FinisherGaugeBarUpdate() = 0;

	virtual void ComboNumberSpriteUpdate() = 0;

	//移動に関するGetter
	Direction GetDirection() { return characterState_.direction; };

	//攻撃に関するGetter
	bool GetIsAttack() { return attackData_.isAttack; };

	bool GetIsLightPunch() { return attackData_.isLightPunch; };

	bool GetIsMiddlePunch() { return attackData_.isMiddlePunch; };

	bool GetIsHighPunch() { return attackData_.isHighPunch; };

	bool GetIsTCMiddlePunch() { return attackData_.isTCMiddlePunch; };

	bool GetIsTCHighPunch() { return attackData_.isTCHighPunch; };

	bool GetIsTackle() { return attackData_.isTackle; };

	bool GetIsShot() { return attackData_.isShot; };

	int GetAttackAnimationFrame() { return attackData_.attackAnimationFrame; };

	//エフェクトに関するGetter
	bool GetIsShake() { return effectState_.isShake; };

	bool GetIsHSVFilter() { return effectState_.isHSVFilter; };

	bool GetIsDown() { return characterState_.isDown; };

	//Setter
	void SetIsReset(bool isReset) { isReset_ = isReset; };

protected:
	CharacterState characterState_;

	EffectState effectState_;

	MoveData moveData_;

	AttackData attackData_;

	TimerData timerData_;

	std::unique_ptr<ParticleEffectPlayer> particleEffectPlayer_;
	bool isParticle_ = false;

	//再生するanimationの番号
	uint32_t animationIndex_ = 4;

	//コンボの数
	int comboCount_ = 0;

	//リセットしているかどうか
	bool isReset_ = false;

	//デバッグ用
	bool isDebug_ = false;
};