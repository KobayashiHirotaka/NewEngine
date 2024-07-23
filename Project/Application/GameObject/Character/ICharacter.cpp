#include "ICharacter.h"
#include "Application/Game/Scenes/GamePlayScene.h"

void ICharacter::Initialize()
{

}

void ICharacter::Update()
{

}

void ICharacter::Draw(const Camera& camera)
{

}

void ICharacter::DrawBone(const Camera& camera)
{

}

void ICharacter::DrawSprite()
{

}

void ICharacter::DrawParticle(const Camera& camera)
{

}

void ICharacter::ImGui(const char* title)
{

}

void ICharacter::Reset()
{
	//行動
	characterState_.behaviorRequest = Behavior::kRoot;

	//アニメーション用の変数
	attackData_.attackAnimationFrame = 0;
	animationTime_ = 0;
	timerData_.downAnimationTimer = 60;

	//コンボカウント
	comboCount_ = 0;

	//ゲージ
	guardGauge_ = 0.0f;
	finisherGauge_ = 0.0f;

	//ガードしているかどうか
	characterState_.isGuard = false;

	//各攻撃をしているかどうか
	attackData_.isAttack = false;
	attackData_.isLightPunch = false;
	attackData_.isMiddlePunch = false;
	attackData_.isHighPunch = false;
	attackData_.isTCMiddlePunch = false;
	attackData_.isTCHighPunch = false;
	attackData_.isTackle = false;

	//キャラクターと当たっているかどうか
	characterState_.isHitCharacter = false;

	//各攻撃を受けているかどうか
	characterState_.isHitLightPunch = false;
	characterState_.isHitMiddlePunch = false;
	characterState_.isHitHighPunch = false;
	characterState_.isHitTCMiddlePunch = false;
	characterState_.isHitTCHighPunch = false;
	characterState_.isHitTackle = false;
	characterState_.isDown = false;

	//リセット
	isReset_ = false;
}

void ICharacter::DownAnimation()
{

}

void ICharacter::BehaviorRootInitialize()
{

}

void ICharacter::BehaviorRootUpdate()
{
	
}

void ICharacter::BehaviorAttackInitialize()
{

}

void ICharacter::BehaviorAttackUpdate()
{

}

void ICharacter::BehaviorJumpInitialize()
{

}

void ICharacter::BehaviorJumpUpdate()
{

}

void ICharacter::BehaviorStanInitialize()
{

}

void ICharacter::BehaviorStanUpdate()
{

}

void ICharacter::HPBarUpdate()
{

}

void ICharacter::GuardGaugeBarUpdate()
{

}

void ICharacter::FinisherGaugeBarUpdate()
{

}

void ICharacter::ComboNumberSpriteUpdate()
{

}