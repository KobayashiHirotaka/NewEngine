/**
 * @file Game3dObjectFactory.cpp
 * @brief 3Dオブジェクトの生成を行うファクトリークラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Game3dObjectFactory.h"

IGame3dObject* Game3dObjectFactory::CreateGameObject(const std::string& objectName)
{
	if (objectName == "Player")
	{
		Player* player = new Player();
		return player;
	}
	else if (objectName == "Enemy")
	{
		Enemy* enemy = new Enemy();
		return enemy;
	}
	else if (objectName == "Ground")
	{
		Ground* ground = new Ground();
		return ground;
	}
	
	return nullptr;
}