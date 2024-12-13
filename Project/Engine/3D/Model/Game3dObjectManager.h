/**
 * @file Game3dObjectManager.h
 * @brief 3Dオブジェクトの管理(初期化、更新、描画など)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "IGame3dObject.h"
#include "Game3dObjectFactory.h"
#include "Engine/3D/Camera/Camera.h"
#include <vector>
#include <memory>

class Game3dObjectManager
{
public:
	/// <summary>インスタンスの取得</summary>
	static Game3dObjectManager* GetInstance();

	/// <summary>インスタンスの削除</summary>
	static void DeleteInstance();

	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>更新</summary>
	void Update();

	/// <summary>描画</summary>
	void Draw(const Camera& camera);

	//オブジェクトのの生成
	static IGame3dObject* CreateGameObject(const std::string& objectName);
	IGame3dObject* CreateGameObjectInternal(const std::string& objectName);

	//Typeごとオブジェクトの生成
	template <typename Type>
	Type* CreateGameObjectFromType();

	template <typename Type>
	Type* CreateGameObjectInternalFromType();

	//Getter,Setter
	//ゲームオブジェクト
	template <typename Type>
	Type* GetGameObject(const std::string& tag);

	//ゲームオブジェクトファクトリー
	void SetGameObjectFactory(Game3dObjectFactory* gameObjectFactory) { gameObjectFactory_ = gameObjectFactory; };

private:
	//シングルトン
	Game3dObjectManager() = default;
	~Game3dObjectManager() = default;
	Game3dObjectManager(const Game3dObjectManager&) = delete;
	const Game3dObjectManager& operator=(const Game3dObjectManager&) = delete;

private:
	//Game3dObjectManagerのインスタンス
	static Game3dObjectManager* sInstance_;

	//ゲームオブジェクト
	std::vector<std::unique_ptr<IGame3dObject>> gameObjects_{};

	//ゲームオブジェクトファクトリー
	Game3dObjectFactory* gameObjectFactory_ = nullptr;
};

template <typename Type>
Type* Game3dObjectManager::CreateGameObjectFromType()
{
	//ゲームオブジェクトを作成
	Type* gameObject = Game3dObjectManager::GetInstance()->CreateGameObjectInternalFromType<Type>();
	return gameObject;
}

template <typename Type>
Type* Game3dObjectManager::CreateGameObjectInternalFromType()
{
	//新しいゲームオブジェクトを動的に作成
	Type* gameObject = new Type();
	gameObject->Initialize();
	gameObject->SetGameObjectManager(this);
	gameObjects_.push_back(std::unique_ptr<IGame3dObject>(gameObject));
	return gameObject;
}

template <typename Type>
Type* Game3dObjectManager::GetGameObject(const std::string& tag)
{
	//全てのゲームオブジェクトをループしてタグを検索
	for (std::unique_ptr<IGame3dObject>& gameObject : gameObjects_)
	{
		//タグを取得
		gameObject->GetTag();

		//タグが一致する場合、動的キャストして返す
		if (gameObject->GetTag() == tag)
		{
			return dynamic_cast<Type*> (gameObject.get());
		}
	}

	return nullptr;
}