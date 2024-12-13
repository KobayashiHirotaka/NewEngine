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
	/// <summary>インスタンスの取得</summary>
	static SceneManager* GetInstance();

	/// <summary>インスタンスの削除</summary>
	static void DeleteInstance();

	/// <summary>更新</summary>
	void Update();

	/// <summary>描画</summary>
	void Draw();

	/// <summary>シーン切り替え</summary>
	void ChangeScene(const std::string& sceneName);

	//Setter
	//シーンファクトリー
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; };

private:
	//シングルトン
	SceneManager();
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	const SceneManager& operator = (const SceneManager&) = delete;

private:
	//SceneManagerのインスタンス
	static SceneManager* sInstance_;

	//現在のシーンのポインタ
	IScene* currentScene_;

	//次のシーンのポインタ
	IScene* nextScene_;

	//シーン生成用のファクトリークラス
	AbstractSceneFactory* sceneFactory_ = nullptr;
};