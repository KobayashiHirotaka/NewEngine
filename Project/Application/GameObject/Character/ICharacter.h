#pragma once
#include "Engine/3D/Model/IGame3dObject.h"

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
		kThrow,
		kStan
	};

	struct WorkAttack
	{
		Vector3 translation;

		Vector3 rotation;

		uint32_t attackParameter = 0;

		int stiffnessTimer = 60;

		bool comboNext = false;

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

		//finisher
		bool isFinisher = false;

		//ジャンプ攻撃
		bool isJumpAttack = false;
	};

	virtual void BoneDraw(const Camera& camera) = 0;

	virtual void DrawSprite() = 0;

	virtual void DrawParticle(const Camera& camera) = 0;

	virtual void Reset() = 0;

protected:


};
