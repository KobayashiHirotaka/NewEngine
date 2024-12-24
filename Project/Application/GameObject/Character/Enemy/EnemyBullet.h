/**
 * @file EnemyBullet.h
 * @brief 敵の弾の管理(移動、削除)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Application/GameObject/Character/BaseBullet.h"

class EnemyBullet : public BaseBullet
{
public:
	/// <summary>初期化</summary>
	void Initialize()override;

	/// <summary>更新</summary>
	void Update()override;

	/// <summary>描画</summary>
	void Draw(const Camera& camera)override;

	/// <summary>パーティクルの描画</summary>
	void DrawParticle(const Camera& camera)override;

	/// <summary>ImGui</summary>
	void ImGui()override;

	/// <summary>当たり判定</summary>
	void OnCollision(Collider* collider)override;

	/// <summary>生成</summary>
	virtual void Create(Model* model, const Vector3& position, const Vector3& velocity)override;

	/// <summary>WorldPositionの取得</summary>
	Vector3 GetWorldPosition();

	//Getter
	//WorldTransform
	WorldTransform& GetWorldTransform() { return worldTransform_; }

	//Collider
	Collider* GetCollider() { return collider_.get(); }

private:
	//当たり判定
	std::unique_ptr<Collider>collider_ = nullptr;
	AABB aabb_ = { {-0.2f,-0.2f,-0.2f},{0.2f,0.2f,0.2f} };
};
