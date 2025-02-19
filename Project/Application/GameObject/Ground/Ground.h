/**
 * @file Ground.h
 * @brief 地面の初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/3D/Model/IGame3dObject.h"
#include "Engine/Components/Input/Input.h"

class Ground : public IGame3dObject
{
public:
	/// <summary>初期化</summary>
	void Initialize()override;

	/// <summary>更新</summary>
	void Update()override;

	/// <summary>描画</summary>
	void Draw(const Camera& camera)override;

	/// <summary>ImGui</summary>
	void ImGui()override;

	/// <summary>当たり判定</summary>
	void OnCollision(Collider*)override;

private:
};