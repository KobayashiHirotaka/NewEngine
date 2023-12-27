#pragma once
#include "IScene.h"
#include <memory>

class GameStartScene : public IScene
{
public:
	GameStartScene();

	~GameStartScene();

	void Initialize(SceneManager* sceneManager)override;

	void Update(SceneManager* sceneManager)override;

	void Draw(SceneManager* sceneManager)override;
};
