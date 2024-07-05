#pragma once
#include "Engine/Framework/AbstractSceneFactory.h"
#include "Application/Game/Scenes/GameTitleScene.h"
#include "Application/Game/Scenes/GamePlayScene.h"
#include "Application/Game/Scenes/GameWinScene.h"
#include "Application/Game/Scenes/GameLoseScene.h"

class SceneFactory : public AbstractSceneFactory 
{
public:
	IScene* CreateScene(const std::string& sceneName) override;
};

