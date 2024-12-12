/**
 * @file CollisionConfig.h
 * @brief 当たり判定の属性
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include <cstdint>

//属性
const uint32_t kCollisionAttributePlayer = 0b0001;
const uint32_t kCollisionAttributeEnemy = 0b0010;
const uint32_t kCollisionAttributeEnemyBullet = 0b0100;

//マスク
const uint32_t kCollisionMaskPlayer = 0b1110;
const uint32_t kCollisionMaskEnemy = 0b1101;
const uint32_t kCollisionMaskEnemyBullet = 0b1011;

//形状
const uint32_t kCollisionPrimitiveSphere = 0b1;
const uint32_t kCollisionPrimitiveAABB = 0b1 << 1;