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
	void Initialize(AABB aabb);

	void Update(AABB aabb);

	void Draw(WorldTransform& worldTransform, const Camera& camera);

	static LineBox* Create(AABB aabb);

private:
	std::unique_ptr<Line> lines_[12];

	Vector4 vertices_[8];

	Vector4 min_ = { 0.0f,0.0f,0.0f,1.0f };

	Vector4 max_ = { 0.0f,0.0f,0.0f,1.0f };
};

