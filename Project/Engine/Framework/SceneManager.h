#include "Application/Game/Scenes/IScene.h"
#include <memory>

class SceneManager 
{
public:
	static SceneManager* GetInstance();

	void Update();

	void Draw();

	void ChangeScene(IScene* newScene);

private:
	SceneManager();
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	const SceneManager& operator = (const SceneManager&) = delete;

private:
	IScene* currentScene_;
};