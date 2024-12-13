/**
 * @file Game3dObjectFactory.h
 * @brief 3Dオブジェクトの生成を行うファクトリークラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "IGame3dObject.h"
#include "Application/GameObject/Character/Player/Player.h"
#include "Application/GameObject/Character/Enemy/Enemy.h"
#include "Application/GameObject/Ground/Ground.h"

class Game3dObjectFactory
{
public:
	/// <summary>ゲームオブジェクトの生成</summary>
	IGame3dObject* CreateGameObject(const std::string& objectName);

private:
};