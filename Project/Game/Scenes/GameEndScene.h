#pragma once
#include"IScene.h"

class GameEndScene : public IScene
{
public:
	void Initialize()override;
	void Update()override;
	void Draw()override;
};
