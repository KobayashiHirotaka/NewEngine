#pragma once
#include "Engine//3D/Model/Model.h"

class Game3dObjectManager;

class IGame3dObject
{
public:
	virtual ~IGame3dObject() = default;

	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void Draw(const Camera& camera) = 0;

	virtual void ImGui(const char* title) = 0;

	//Getter,Setter
	const Model* GetModel() const { return model_.get(); }
	void SetModel(std::unique_ptr<Model> model) { model_ = std::move(model); }

	void SetGameObjectManager(Game3dObjectManager* game3dObjectManager) { game3dObjectManager_ = game3dObjectManager; };

	const bool GetIsVisible() const { return isVisible_; };
	void SetIsVisible(const bool& isVisible) { isVisible_ = isVisible; };

	void SetPosition(const Vector3& position) { worldTransform_.translation = position; };
	void SetPositionX(const float& positionX) { worldTransform_.translation.x = positionX; };
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation = rotation; };
	void SetScale(const Vector3& scale) { worldTransform_.scale = scale; };

	int const GetHP() const { return hp_; };
	void SetHp(const int& hp) { hp_ = hp; };
	void SetMaxHp(const int& maxHp) { maxHp_ = maxHp; };

	void SetFrontSpeed(const float& frontSpeed) { frontSpeed_ = frontSpeed; };
	void SetBackSpeed(const float& backSpeed) { backSpeed_ = backSpeed; };

	void SetGuardGauge(const float& guardGauge) { guardGauge_ = guardGauge; };
	void SetMaxGuardGauge(const float& maxGuardGauge) { maxGuardGauge_ = maxGuardGauge; };

	const float GetFinisherGauge() const { return finisherGauge_; };
	void SetFinisherGauge(const float& finisherGauge) { finisherGauge_ = finisherGauge; };
	void SetMaxFinisherGauge(const float& maxFinisherGauge) { maxFinisherGauge_ = maxFinisherGauge; };

	const std::string GetTag() const { return tag_; };
	void SetTag(const std::string tag) { tag_ = tag; };

protected:
	Game3dObjectManager* game3dObjectManager_ = nullptr;

	WorldTransform worldTransform_{};

	std::unique_ptr<Model> model_ = nullptr;

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

	//描画するかどうか
	bool isVisible_ = false;
};
