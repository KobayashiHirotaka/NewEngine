/**
 * @file BaseCharacter.h
 * @brief 各キャラクターの基底クラス
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
		float takeFinisherGaugeIncreaseAmount = 1.5f;

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

	/// <summary>弾を発射</summary>
	virtual void ShootBullet(const Vector3& startPosition, const Vector3& velocity) = 0;


	/// <summary>向きの更新</summary>
	void UpdateDirection(Vector3 character1Position, Vector3 character2Position);

	//移動
	/// <summary>移動</summary>
	void Move(const Vector3 velocity);

	//攻撃
	/// <summary>攻撃の開始</summary>
	void StartAttack(bool& isAttackType);

	/// <summary>攻撃の終了</summary>
	void EndAttack(bool& isAttackType);

	/// <summary>攻撃タイミングを評価</summary>
	void EvaluateAttackTiming(int& attackAnimationFrame);


	//当たり判定
	/// <summary>当たり判定のリセット</summary>
	virtual void ResetCollision() = 0;


	/// <summary>アニメーションの更新</summary>
	void UpdateAnimationTime(float& animationTime, const bool isLoop, const float frameRate,
		const int animationIndex, Model* model);

	/// <summary>ダウンアニメーションの終了</summary>
	void EndDownAnimation(const int animationIndex);


	/// <summary>WorldPositionの取得</summary>
	Vector3 GetWorldPosition();


	//基本データに関するGetter
	BaseData GetBaseData() { return baseData_; };

	float GetAnimationTime() const { return animationTime_; };

	float GetDistance() const { return distance_; };

	const float GetMaxDistance() const { return kMaxDistance_; };

	//キャラクターの状態に関するGetter
	CharacterState GetCharacterState() const { return characterState_; };

	//攻撃に関するGetter
	AttackData& GetAttackData() { return attackData_; };

	std::string GetAttackType() const { return attackType_; };

	std::string GetDownType() const { return downType_; };

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

	void SetAnimationIndex(uint32_t animationIndex) { animationIndex_ = animationIndex; };

	void SetPositionY(float positionY) { worldTransform_.translation.y = positionY; };

	void SetIsGround(bool isGround) { characterState_.isGround = isGround; };

	void SetIsDown(bool isDown) { characterState_.isDown = isDown; };

	//攻撃に関するSetter
	void SetIsAttack(bool isAttack) { attackData_.isAttack = isAttack; };

	void SetDamage(int damage) { attackData_.damage = damage; };

	void SetHitStop(HitStop* hitStop) { hitStop_ = hitStop; };

	void SetAttackType(std::string attackType) { attackType_ = attackType; };

	void SetDownType(std::string downType) { downType_ = downType; };

	void SetIsLightPunch(bool isLightPunch) { characterState_.isHitLightPunch = isLightPunch; };

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

	//弾のモデル
	std::unique_ptr<Model> bulletModel_;

	//再生するanimationの番号
	uint32_t animationIndex_ = 4;
	float animationTime_ = 0.0f;

	//サウンド
	uint32_t attackSoundHandle_ = 0u;
	uint32_t weaponAttackSoundHandle_ = 0u;
	uint32_t damageSoundHandle_ = 0u;
	uint32_t guardSoundHandle_ = 0u;
	float volume_ = 1.0f;

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

	//ダウンタイプ
	std::string downType_;

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
