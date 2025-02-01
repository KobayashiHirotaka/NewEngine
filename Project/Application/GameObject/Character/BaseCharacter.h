/**
 * @file BaseCharacter.h
 * @brief 各キャラクター(Player,Enemy)の基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/3D/Model/IGame3dObject.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/Line/LineBox.h"
#include "Engine/Utility/Collision/Collider.h"
#include "Engine/Utility/Collision/CollisionConfig.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/2D/Sprite/UI.h"
#include "Application/GameObject/Effect/ParticleEffectPlayer.h"
#include "Application/GameObject/Character/Direction.h"
#include "Application/Game/HitStop/HitStop.h"
#include "Application/Game/AttackEditor/AttackEditor.h"
#include <random>
#include <numbers>
#include <unordered_map>
#include <algorithm>

class BaseCharacter : public IGame3dObject
{
public:
	//行動
	enum class Behavior
	{
		kRoot,
		kAttack,
		kJump,
		kStan
	};

	//キャラクターの状態
	struct CharacterState
	{
		//現在の行動
		Behavior behavior = Behavior::kRoot;

		//行動のリクエスト
		std::optional<Behavior> behaviorRequest = std::nullopt;

		//向いている方向
		Direction direction = Direction::Right;

		//右方向を向いているときのモデルの回転
		const float rightDirectionRotation = 1.7f;

		//左方向を向いているときのモデルの回転
		const float leftDirectionRotation = 4.6f;

		//キャラクターと当たっているかどうか
		bool isHitCharacter = false;

		//ダウンしているかどうか
		bool isDown = false;

		//ガードしているかどうか
		bool isGuard = false;

		//地上にいるかどうか
		bool isGround = false;

		//各攻撃があたっているかどうか
		//通常攻撃
		bool isHitLightPunch = false;
		bool isHitMiddlePunch = false;
		bool isHitHighPunch = false;
		bool isHitTCMiddlePunch = false;
		bool isHitTCHighPunch = false;

		//ジャンプ攻撃
		bool isHitJumpAttack = false;

		//タックル攻撃
		bool isHitTackle = false;

		//アッパー攻撃
		bool isHitUppercut = false;

		//超必殺技
		bool isHitFinisherFirstAttack = false;
		bool isHitFinisherSecondAttack = false;

		//弾
		bool isHitBullet = false;
		bool isHitAirBullet = false;

		//ヒットストップ
		bool isHitStop = false;
	};

	//エフェクト
	struct EffectState
	{
		//シェイクしているかどうか
		bool isShake = false;

		//HSVFilterをかけるかどうか
		bool isHSVFilter = false;
	};

	//基本データ
	struct BaseData
	{
		//HP
		const int kMaxHp_ = 100;
		int hp_ = 0;

		//ガードゲージ
		const float kMaxGuardGauge_ = 50.0f;
		float guardGauge_ = 0.0f;

		//必殺技のゲージ
		const float kMaxFinisherGauge_ = 50.0f;
		float finisherGauge_ = 0.0f;
	};

	//移動データ
	struct MoveData
	{
		//足の速さ
		float frontSpeed_ = 0.0f;
		float backSpeed_ = 0.0f;

		//速度
		Vector3 velocity = {};
	};

	//攻撃データ
	struct AttackData
	{
		//攻撃フレーム
		int attackAnimationFrame = 0;

		//攻撃ごとのパラメータ
		//攻撃判定のつき初め
		int attackStartTime = 0;

		//攻撃判定のつき終わり
		int attackEndTime = 0;

		//攻撃時の硬直
		int recoveryTime = 0;

		//キャンセルの開始時間
		int cancelStartTime = 0;

		//キャンセルの終了時間
		int cancelEndTime = 0;

		//ダメージ
		int damage = 0;

		//ヒット時の硬直
		int hitRecoveryTime = 0;

		//ガードゲージ増加量
		float guardGaugeIncreaseAmount = 0.0f;

		//必殺技ゲージ増加量
		float finisherGaugeIncreaseAmount = 0.0f;

		//ヒットストップ
		float hitStop = 0.0f;

		//攻撃を受ける側の必殺技ゲージ増加量
		const float takeFinisherGaugeIncreaseAmount = 1.5f;

		//攻撃しているか
		bool isAttack = false;

		//硬直中か
		bool isRecovery = false;

		//ダメージを受けているかどうか
		bool isDamaged = false;

		//ガードしたかどうか
		bool isGuarded = false;

		//必殺技ゲージが増えているかどうか
		bool isFinisherGaugeIncreased = false;

		bool isHitStop_ = false;

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

		//アッパー攻撃
		bool isUppercut = false;

		//弾攻撃
		bool isShot = false;

		//ジャンプ攻撃
		bool isJumpAttack = false;

		//超必殺技
		bool isFinisher = false;
		bool isFinisherFirstAttack = false;
		bool isFinisherSecondAttack = false;
	};

	//時間データ
	struct TimerData
	{
		//ダウン演出の時間
		const int maxDownAnimationTimer = 60;
		int downAnimationTimer = maxDownAnimationTimer;

		//ガード演出の時間
		const int maxGuardAnimationTimer = 60;
		int guardAnimationTimer = maxGuardAnimationTimer;

		//エフェクトを出す時間
		const int maxEffectTimer = 60;
		int effectTimer = maxEffectTimer;

		//スタンの時間
		const int maxStanTimer = 60;
		int stanTimer = maxStanTimer;

		//コンボの猶予時間
		const int maxComboTimer = 60;
		int comboTimer = maxComboTimer;

		//超必殺技の演出時間
		const int maxFinisherTimer = 120;
		int finisherTimer = maxFinisherTimer;
	};

	//基本
	/// <summary>デストラクタ</summary>
	virtual ~BaseCharacter() = default;

	/// <summary>初期化</summary>
	virtual void Initialize() = 0;

	/// <summary>更新</summary>
	virtual void Update() = 0;

	/// <summary>描画</summary>
	virtual void Draw(const Camera& camera) = 0;

	/// <summary>骨の描画</summary>
	virtual void DrawBone(const Camera& camera) = 0;

	/// <summary>当たり判定の描画</summary>
	virtual void DrawCollision(const Camera& camera) = 0;

	/// <summary>スプライトの描画</summary>
	virtual void DrawSprite() = 0;

	/// <summary>パーティクルの描画</summary>
	virtual void DrawParticle(const Camera& camera) = 0;

	/// <summary>ImGui</summary>
	virtual void ImGui() = 0;

	/// <summary>リセット</summary>
	virtual void Reset() = 0;


	//アニメーション
	/// <summary>ダウンアニメーション</summary>
	virtual void DownAnimation() = 0;

	/// <summary>ダウンアニメーションの終了</summary>
	virtual void EndDownAnimation(int animationIndex, bool& isHitAttackType) = 0;


	//キャラクターの状態
	/// <summary>移動状態の初期化</summary>
	virtual void InitializeBehaviorRoot() = 0;

	/// <summary>移動状態の更新</summary>
	virtual void UpdateBehaviorRoot() = 0;

	/// <summary>攻撃状態の初期化</summary>
	virtual void InitializeBehaviorAttack() = 0;
	 
	/// <summary>攻撃状態の更新</summary>
	virtual void UpdateBehaviorAttack() = 0;

	/// <summary>ジャンプ状態の初期化</summary>
	virtual void InitializeBehaviorJump() = 0;

	/// <summary>ジャンプ状態の更新</summary>
	virtual void UpdateBehaviorJump() = 0;

	/// <summary>スタン状態の初期化</summary>
	virtual void InitializeBehaviorStan() = 0;

	/// <summary>スタン状態の更新</summary>
	virtual void UpdateBehaviorStan() = 0;


	//攻撃
	/// <summary>攻撃の開始</summary>
	virtual void StartAttack(bool& isAttackType) = 0;

	/// <summary>攻撃の終了</summary>
	virtual void EndAttack(bool& isAttackType) = 0;

	/// <summary>攻撃タイミングを評価</summary>
	virtual void EvaluateAttackTiming() = 0;

	/// <summary>ダメージの適応</summary>
	virtual void ApplyDamage() = 0;

	/// <summary>コンボされているとき</summary>
	virtual void HitCombo() = 0;

	/// <summary>コンボカウントの更新</summary>
	virtual void ComboCountUpdate(const int kRecoveryTime) = 0;


	//当たり判定
	/// <summary>当たり判定のリセット</summary>
	virtual void ResetCollision() = 0;


	//UIの更新
	/// <summary>HPのUIの更新</summary>
	virtual void UpdateHPBar() = 0;

	/// <summary>ガードゲージのUIの更新</summary>
	virtual void UpdateGuardGaugeBar() = 0;

	/// <summary>ガードゲージのUIの適応</summary>
	virtual void AdjustGuardGauge() = 0;

	/// <summary>必殺技ゲージのUIの適応</summary>
	virtual void UpdateFinisherGaugeBar() = 0;

	/// <summary>必殺技ゲージのUIの適応</summary>
	virtual void AdjustFinisherGauge(float value) = 0;

	/// <summary>コンボ表示のUIの更新</summary>
	virtual void UpdateComboNumberSprite() = 0;


	/// <summary>アニメーションの更新</summary>
	void UpdateAnimationTime(float& animationTime, const bool isLoop, const float frameRate,
		const int animationIndex, Model* model);


	//基本データに関するGetter
	int GetHP() const { return baseData_.hp_; };

	float GetFinisherGauge() const { return baseData_.finisherGauge_; };

	float GetAnimationTime() const { return animationTime_; };

	//移動に関するGetter
	Direction GetDirection() const { return characterState_.direction; };

	//攻撃に関するGetter
	bool GetIsAttack() const { return attackData_.isAttack; };

	bool GetIsLightPunch() const { return attackData_.isLightPunch; };

	bool GetIsMiddlePunch() const { return attackData_.isMiddlePunch; };

	bool GetIsHighPunch() const { return attackData_.isHighPunch; };

	bool GetIsTCMiddlePunch() const { return attackData_.isTCMiddlePunch; };

	bool GetIsTCHighPunch() const { return attackData_.isTCHighPunch; };

	bool GetIsJumpAttack() const { return attackData_.isJumpAttack; };

	bool GetIsTackle() const { return attackData_.isTackle; };

	bool GetIsUppercut() const { return attackData_.isUppercut; };

	bool GetIsShot() const { return attackData_.isShot; };

	bool GetIsFinisher() const { return attackData_.isFinisher; };

	bool GetIsFinisherFirstAttack() const { return attackData_.isFinisherFirstAttack; };

	bool GetIsFinisherSecondAttack() const { return attackData_.isFinisherSecondAttack; };

	int GetAttackAnimationFrame() const { return attackData_.attackAnimationFrame; };

	int GetDamage() const { return attackData_.damage; };

	int GetHitRecoveryTime() const { return attackData_.hitRecoveryTime; };

	float GetGuardGaugeIncreaseAmount() const { return attackData_.guardGaugeIncreaseAmount; };

	float GetFinisherGaugeIncreaseAmount() const { return attackData_.finisherGaugeIncreaseAmount; };

	float GetHitStop() const { return attackData_.hitStop; };

	//エフェクトに関するGetter
	bool GetIsShake() const { return effectState_.isShake; };

	bool GetIsHSVFilter() const { return effectState_.isHSVFilter; };

	bool GetIsDown() const { return characterState_.isDown; };

	int GetFinisherTimer() const { return timerData_.finisherTimer; };

	int GetComboCount() const { return comboCount_; };

	bool GetIsKO() const { return isKO_; };

	//基本データに関するSetter
	void SetHp(int hp) { baseData_.hp_ = hp; };

	void SetFrontSpeed(float frontSpeed) { moveData_.frontSpeed_ = frontSpeed; };
	void SetBackSpeed(float backSpeed) { moveData_.backSpeed_ = backSpeed; };

	void SetGuardGauge(float guardGauge) { baseData_.guardGauge_ = guardGauge; };

	void SetFinisherGauge(float finisherGauge) { baseData_.finisherGauge_ = finisherGauge; };

	void SetAnimationIndex(uint32_t animationIndex) { animationIndex_ = animationIndex; };

	//攻撃に関するSetter
	void SetDamage(int damage) { attackData_.damage = damage; };

	void SetGuardGaugeIncreaseAmount(float guardGaugeIncreaseAmount) { attackData_.guardGaugeIncreaseAmount = guardGaugeIncreaseAmount; };

	void SetFinisherGaugeIncreaseAmount(float finisherGaugeIncreaseAmount) { attackData_.finisherGaugeIncreaseAmount = finisherGaugeIncreaseAmount; };

	void SetIsGuarded(bool isGuarded) { attackData_.isGuarded = isGuarded; };

	void SetHitStop(HitStop* hitStop) { hitStop_ = hitStop; };

	//リセット状態のSetter
	void SetIsReset(bool isReset) { isReset_ = isReset; };

protected:
	//Inputのポインタ
	Engine::Input* input_ = nullptr;

	//Audioのポインタ
	Engine::Audio* audio_ = nullptr;

	//キャラクターの状態
	CharacterState characterState_;

	//エフェクト
	EffectState effectState_;

	//基本データ
	BaseData baseData_;

	//移動データ
	MoveData moveData_;

	//攻撃データ
	AttackData attackData_;

	//時間データ
	TimerData timerData_;

	//始動技
	std::string firstAttack_;

	//HitStop
	HitStop* hitStop_;

	//パーティクル
	std::unique_ptr<ParticleEffectPlayer> particleEffectPlayer_;
	bool isParticle_ = false;

	//再生するanimationの番号
	uint32_t animationIndex_ = 4;
	float animationTime_ = 0.0f;

	//コンボの数
	int comboCount_ = 0;

	//リセットしているかどうか
	bool isReset_ = false;

	//デバッグ用
	bool isDebug_ = false;

	bool isFinisherCharge_ = false;

	//画面端
	float leftEdge_ = -6.0f;
	float rightEdge_ = 6.0f;

	//攻撃時にめり込まないための画面端
	float attackLeftEdge_ = -5.5f;
	float attackRightEdge_ = 5.5f;

	//距離
	Vector3 difference_ = { 0.0f,0.0f,0.0f };
	float distance_ = 0.0f;
	const float kMaxDistance_ = 4.3f;

	//前フレームのX座標
	float previousPositionX_ = 0.0f;

	//エディター用
	std::string attackType_;

	//KOしているかどうか
	bool isKO_ = false;

	//時間の調整用
	const float kScaleFacter_ = 100.0f;

	//ゲーム開始時間
	const int kMaxMigrationTime_ = 200;

	//補完速度
	const float kLerpSpeed_ = 0.1f;

	//通常時の当たり判定
	const AABB defaultCollsiion_ = { {-0.3f,0.0f,-0.3f},{0.3f,1.0f,0.3f} };

	//コンボカウントの定数
	const int kComboCount_[10] = { 0,1,2,3,4,5,6,7,8,9 };
};
