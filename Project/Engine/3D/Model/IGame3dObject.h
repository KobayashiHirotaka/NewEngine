#pragma once
#include "Engine//3D/Model/Model.h"

class Game3dObjectManager;

class IGame3dObject
{
public:
	virtual ~IGame3dObject() = default;

	virtual void Initialize();

	virtual void Update();

	virtual void Draw(const Camera& camera);

	float GetHP() { return hp_; };

	bool GetIsVisible() { return isVisible_; };

	//モデル
	const Model* GetModel() const { return model_.get(); }
	void SetModel(std::unique_ptr<Model> model) { model_ = std::move(model); }

	//ゲームオブジェクトマネージャーのsetter
	void SetGameObjectManager(Game3dObjectManager* game3dObjectManager) { game3dObjectManager_ = game3dObjectManager; };

	void SetIsVisible(const bool& isVisible) { isVisible_ = isVisible; };

	//トランスフォーム用のsetter
	void SetPosition(const Vector3& position) { worldTransform_.translation = position; };
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation = rotation; };
	void SetScale(const Vector3& scale) { worldTransform_.scale = scale; };

	//キャラクターのパラメータ用のsetter
	void SetHp(const float& hp) { hp_ = hp; };
	void SetMaxHp(const float& maxHp) { maxHp_ = maxHp; };

	void SetFrontSpeed(const float& frontSpeed) { frontSpeed_ = frontSpeed; };
	void SetBackSpeed(const float& backSpeed) { backSpeed_ = backSpeed; };

	void SetGuardGauge(const float& guardGauge) { guardGauge_ = guardGauge; };
	void SetMaxGuardGauge(const float& maxGuardGauge) { maxGuardGauge_ = maxGuardGauge; };

	void SetFinisherGauge(const float& finisherGauge) { finisherGauge_ = finisherGauge; };
	void SetMaxFinisherGauge(const float& maxFinisherGauge) { maxFinisherGauge_ = maxFinisherGauge; };

	//タグ
	const std::string GetTag() { return tag_; };
	void SetTag(const std::string tag) { tag_ = tag; };

protected:
	Game3dObjectManager* game3dObjectManager_ = nullptr;

	WorldTransform worldTransform_{};

	std::unique_ptr<Model> model_ = nullptr;

	std::string tag_;

	bool isVisible_;

	//キャラクターのパラメータ
	//hp
	float maxHp_;
	float hp_;

	//足の速さ
	float frontSpeed_;
	float backSpeed_;

	//ガードゲージ
	float maxGuardGauge_;
	float guardGauge_;

	//必殺技のゲージ
	float maxFinisherGauge_;
	float finisherGauge_;
};
