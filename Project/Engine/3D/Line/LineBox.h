/**
 * @file LineBox.h
 * @brief 線(Box)の生成、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/Base/DirectXCore/DirectXCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/3D/Model/Material.h"
#include "Engine/3D/Line/Line.h"
#include <cassert>

class LineBox
{
public:
	/// <summary>初期化</summary>
	void Initialize(AABB aabb);

	/// <summary>更新</summary>
	void Update(AABB aabb);

	/// <summary>描画</summary>
	void Draw(WorldTransform& worldTransform, const Camera& camera);

	/// <summary>LineBoxの生成</summary>
	static LineBox* Create(AABB aabb);

private:
	//線
	std::unique_ptr<Line> lines_[12];

	//頂点
	Vector4 vertices_[8];

	//最小値
	Vector4 min_ = { 0.0f,0.0f,0.0f,1.0f };

	//最大値
	Vector4 max_ = { 0.0f,0.0f,0.0f,1.0f };

	//線の最大数
	const int kMaxLines_ = 12;
};

