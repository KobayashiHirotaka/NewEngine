#include "Application/Game/Scenes/IScene.h"
#include <memory>

class SceneManager 
{
public:
	static SceneManager* GetInstance();

	static void DeleteInstance();

	void Update();

	void Draw();

	void ChangeScene(IScene* newScene);

private:
	SceneManager();
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	const SceneManager& operator = (const SceneManager&) = delete;

private:
	static SceneManager* instance_;

	IScene* currentScene_;
};