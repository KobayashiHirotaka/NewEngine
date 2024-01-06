#pragma once
#include <cstdint>

const uint32_t kCollisionAttributePlayer = 0b0001;
const uint32_t kCollisionAttributeEnemy = 0b0010;
const uint32_t kCollisionAttributePlayerWeapon = 0b0100;
const uint32_t kCollisionAttributeEnemyWeapon = 0b1000;

const uint32_t kCollisionMaskPlayer = 0b1110;
const uint32_t kCollisionMaskEnemy = 0b1101;
const uint32_t kCollisionMaskPlayerWeapon = 0b1011;
const uint32_t kCollisionMaskEnemyWeapon = 0b0111;

const uint32_t kCollisionPrimitiveSphere = 0b1;
const uint32_t kCollisionPrimitiveAABB = 0b1 << 1;