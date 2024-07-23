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

	virtual void DrawBone(const Camera& camera) = 0;

	virtual void DrawSprite() = 0;

	virtual void DrawParticle(const Camera& camera) = 0;

	virtual void Reset() = 0;

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

protected:
	CharacterState characterState_;

	EffectState effectState_;

	MoveData moveData_;

	AttackData attackData_;

	std::unique_ptr<ParticleEffectPlayer> particleEffectPlayer_;
	bool isParticle_ = false;

	//リセットしているかどうか
	bool isReset_ = false;
};
