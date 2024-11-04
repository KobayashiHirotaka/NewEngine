#pragma once
#include "Engine/Framework/EngineCore.h"
#include "Application/Game/Scenes/SceneFactory.h"

class GameManager : public EngineCore
{
public:
	void Initialize() override;

	//void Update()override;

	void Draw()override;

	//void Finalize()override;
};

