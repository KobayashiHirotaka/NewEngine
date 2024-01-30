#pragma once

class SceneManager;

class IScene 
{
public:
	virtual ~IScene() = default;
	virtual void Initialize(SceneManager* sceneManager) = 0;
	virtual void Update(SceneManager* sceneManager) = 0;
	virtual void Draw(SceneManager* sceneManager) = 0;
	virtual void Finalize(SceneManager* sceneManager) = 0;

	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; };

protected:
	SceneManager* sceneManager_ = nullptr;
};