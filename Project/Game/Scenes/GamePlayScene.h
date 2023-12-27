#pragma once
#include "IScene.h"
#include <memory>

class GamePlayScene : public IScene
{
public:
	GamePlayScene();

	~GamePlayScene();

	void Initialize(SceneManager* sceneManager)override;

	void Update(SceneManager* sceneManager)override;

	void Draw(SceneManager* sceneManager)override;
};
