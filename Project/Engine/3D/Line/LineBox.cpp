/**
 * @file LineBox.cpp
 * @brief 線(Box)の生成、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "LineBox.h"

void LineBox::Initialize(AABB aabb)
{
    for (int i = 0; i < 12; i++)
    {
        lines_[i] = std::make_unique<Line>();
    }

    //AABBの最小座標
    min_ = { aabb.min.x, aabb.min.y, aabb.min.z, 1.0f };

    //AABBの最大座標
    max_ = { aabb.max.x, aabb.max.y, aabb.max.z, 1.0f };

    //AABBの8つの頂点を定義
    vertices_[0] = { Vector4(min_.x, min_.y, min_.z, 1.0f) };
    vertices_[1] = { Vector4(max_.x, min_.y, min_.z, 1.0f) };
    vertices_[2] = { Vector4(min_.x, max_.y, min_.z, 1.0f) };
    vertices_[3] = { Vector4(max_.x, max_.y, min_.z, 1.0f) };

    vertices_[4] = { Vector4(min_.x, min_.y, max_.z, 1.0f) };
    vertices_[5] = { Vector4(max_.x, min_.y, max_.z, 1.0f) };
    vertices_[6] = { Vector4(min_.x, max_.y, max_.z, 1.0f) };
    vertices_[7] = { Vector4(max_.x, max_.y, max_.z, 1.0f) };

    //AABBの12本の辺をラインとして描画
    lines_[0].reset(Line::Create(vertices_[0], vertices_[4]));
    lines_[1].reset(Line::Create(vertices_[0], vertices_[1]));
    lines_[2].reset(Line::Create(vertices_[4], vertices_[5]));
    lines_[3].reset(Line::Create(vertices_[1], vertices_[5]));

    lines_[4].reset(Line::Create(vertices_[2], vertices_[6]));
    lines_[5].reset(Line::Create(vertices_[2], vertices_[3]));
    lines_[6].reset(Line::Create(vertices_[6], vertices_[7]));
    lines_[7].reset(Line::Create(vertices_[3], vertices_[7]));

    lines_[8].reset(Line::Create(vertices_[0], vertices_[2]));
    lines_[9].reset(Line::Create(vertices_[4], vertices_[6]));
    lines_[10].reset(Line::Create(vertices_[1], vertices_[3]));
    lines_[11].reset(Line::Create(vertices_[5], vertices_[7]));
}

void LineBox::Update(AABB aabb)
{
    //AABBの最小座標
    min_ = { aabb.min.x, aabb.min.y, aabb.min.z, 1.0f };

    //AABBの最大座標
    max_ = { aabb.max.x, aabb.max.y, aabb.max.z, 1.0f };

    //AABBの8つの頂点を定義
    vertices_[0] = { Vector4(min_.x, min_.y, min_.z, 1.0f) };
    vertices_[1] = { Vector4(max_.x, min_.y, min_.z, 1.0f) };
    vertices_[2] = { Vector4(min_.x, max_.y, min_.z, 1.0f) };
    vertices_[3] = { Vector4(max_.x, max_.y, min_.z, 1.0f) };
    vertices_[4] = { Vector4(min_.x, min_.y, max_.z, 1.0f) };
    vertices_[5] = { Vector4(max_.x, min_.y, max_.z, 1.0f) };
    vertices_[6] = { Vector4(min_.x, max_.y, max_.z, 1.0f) };
    vertices_[7] = { Vector4(max_.x, max_.y, max_.z, 1.0f) };

    lines_[0]->Update(vertices_[0], vertices_[4]);
    lines_[1]->Update(vertices_[0], vertices_[1]);
    lines_[2]->Update(vertices_[4], vertices_[5]);
    lines_[3]->Update(vertices_[1], vertices_[5]);

    lines_[4]->Update(vertices_[2], vertices_[6]);
    lines_[5]->Update(vertices_[2], vertices_[3]);
    lines_[6]->Update(vertices_[6], vertices_[7]);
    lines_[7]->Update(vertices_[3], vertices_[7]);

    lines_[8]->Update(vertices_[0], vertices_[2]);
    lines_[9]->Update(vertices_[4], vertices_[6]);
    lines_[10]->Update(vertices_[1], vertices_[3]);
    lines_[11]->Update(vertices_[5], vertices_[7]);
}

void LineBox::Draw(WorldTransform& worldTransform, const Camera& camera)
{
    for (int i = 0; i < 12; i++)
    {
        lines_[i]->Draw(worldTransform, camera);
    }
}

LineBox* LineBox::Create(AABB aabb)
{
    LineBox* lineBox = new LineBox();
    lineBox->Initialize(aabb);
    return lineBox;
}