/**
 * @file SceneManager.h
 * @brief 各シーンの管理(更新、描画、シーン遷移など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

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
	static SceneManager* sInstance_;

	IScene* currentScene_;

	IScene* nextScene_;

	AbstractSceneFactory* sceneFactory_ = nullptr;
};