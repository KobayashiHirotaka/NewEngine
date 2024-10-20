#include "AbstractSceneFactory.h"
#include "Application/Game/Scenes/IScene.h"
#include <memory>

class SceneManager
{
public:
	static SceneManager* GetInstance();

	static void DeleteInstance();

	void Update();

	void Draw();

	void ChangeScene(const std::string& sceneName);

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; };

private:
	SceneManager();
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	const SceneManager& operator = (const SceneManager&) = delete;

private:
	static SceneManager* instance_;

	IScene* currentScene_;

	IScene* nextScene_;

	AbstractSceneFactory* sceneFactory_ = nullptr;
};