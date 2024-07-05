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
	/*else if (objectName == "PlayerWeapon")
	{
		PlayerWeapon* playerWeapon = new PlayerWeapon();
		return playerWeapon;
	}
	else if (objectName == "EnemyWeapon")
	{
		EnemyWeapon* enemyWeapon = new EnemyWeapon();
		return enemyWeapon;
	}
	else if (objectName == "TestObject")
	{
		TestObject* testObject = new TestObject();
		return testObject;
	}*/
	return nullptr;
}