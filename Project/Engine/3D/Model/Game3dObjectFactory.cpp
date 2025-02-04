/**
 * @file Game3dObjectFactory.cpp
 * @brief 3Dオブジェクトの生成を行うファクトリークラス
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "Game3dObjectFactory.h"

IGame3dObject* Game3dObjectFactory::CreateGameObject(const std::string& objectName)
{
	//オブジェクト名に応じて対応するゲームオブジェクトを生成
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
	else if (objectName == "BackGround")
	{
		BackGround* backGround = new BackGround();
		return backGround;
	}

	
	return nullptr;
}