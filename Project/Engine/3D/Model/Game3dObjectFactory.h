#pragma once
#include "IGame3dObject.h"
#include "Application/GameObject/Character/Player/Player.h"
#include "Application/GameObject/Character/Player/PlayerWeapon.h"
#include "Application/GameObject/Character/Enemy/Enemy.h"
#include "Application/GameObject/Character/Enemy/EnemyWeapon.h"
#include "Application/GameObject/TestObject/TestObject.h"

class Game3dObjectFactory
{
public:
	IGame3dObject* CreateGameObject(const std::string& objectName);

private:
};

