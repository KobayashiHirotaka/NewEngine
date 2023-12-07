#pragma once
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include "IScene.h"

class GameStartScene : public IScene
{
public:
	void Initialize()override;
	void Update()override;
	void Draw()override;
};
