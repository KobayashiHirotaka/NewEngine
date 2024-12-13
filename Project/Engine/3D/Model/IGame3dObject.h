/**
 * @file IGame3dObject.h
 * @brief 3Dオブジェクトの基底クラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine//3D/Model/Model.h"

class Game3dObjectManager;

class IGame3dObject
{
public:
	/// <summary>デストラクタ</summary>
	virtual ~IGame3dObject() = default;

	/// <summary>初期化</summary>
	virtual void Initialize() = 0;

	/// <summary>更新</summary>
	virtual void Update() = 0;

	/// <summary>描画</summary>
	virtual void Draw(const Camera& camera) = 0;

	/// <summary>ImGui</summary>
	virtual void ImGui() = 0;

	//Getter,Setter
	//モデル
	const Model* GetModel() const { return model_.get(); }
	void SetModel(std::unique_ptr<Model> model) { model_ = std::move(model); }

	//オブジェクトマネージャー
	void SetGameObjectManager(Game3dObjectManager* game3dObjectManager) { game3dObjectManager_ = game3dObjectManager; };

	//表示するか
	const bool GetIsVisible() const { return isVisible_; };
	void SetIsVisible(const bool& isVisible) { isVisible_ = isVisible; };

	//位置
	void SetPosition(const Vector3& position) { worldTransform_.translation = position; };
	void SetPositionX(const float& positionX) { worldTransform_.translation.x = positionX; };
	
	//回転
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation = rotation; };

	//スケール
	void SetScale(const Vector3& scale) { worldTransform_.scale = scale; };

	//体力
	int const GetHP() const { return hp_; };
	void SetHp(const int& hp) { hp_ = hp; };
	void SetMaxHp(const int& maxHp) { maxHp_ = maxHp; };

	//足の速さ
	void SetFrontSpeed(const float& frontSpeed) { frontSpeed_ = frontSpeed; };
	void SetBackSpeed(const float& backSpeed) { backSpeed_ = backSpeed; };

	//ガードゲージ
	void SetGuardGauge(const float& guardGauge) { guardGauge_ = guardGauge; };
	void SetMaxGuardGauge(const float& maxGuardGauge) { maxGuardGauge_ = maxGuardGauge; };

	//必殺技ゲージ
	const float GetFinisherGauge() const { return finisherGauge_; };
	void SetFinisherGauge(const float& finisherGauge) { finisherGauge_ = finisherGauge; };
	void SetMaxFinisherGauge(const float& maxFinisherGauge) { maxFinisherGauge_ = maxFinisherGauge; };

	//タグ
	const std::string GetTag() const { return tag_; };
	void SetTag(const std::string tag) { tag_ = tag; };

protected:
	//ゲームオブジェクトマネージャーのポインタ
	Game3dObjectManager* game3dObjectManager_ = nullptr;

	//WorldTransform
	WorldTransform worldTransform_{};

	//モデル
	std::unique_ptr<Model> model_ = nullptr;

	//タグ
	std::string tag_;

	//レベルエディターで設定できるキャラクターのパラメータ
	//HP
	int maxHp_ = 0;
	int hp_ = 0;

	//足の速さ
	float frontSpeed_ = 0.0f;
	float backSpeed_ = 0.0f;

	//ガードゲージ
	float maxGuardGauge_ = 0.0f;
	float guardGauge_ = 0.0f;

	//必殺技のゲージ
	float maxFinisherGauge_ = 0.0f;
	float finisherGauge_ = 0.0f;

	//表示するかどうか
	bool isVisible_ = false;
};
