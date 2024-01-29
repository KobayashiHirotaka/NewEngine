#pragma once

class SceneManager;

class IScene 
{
public:
	virtual ~IScene() {};
	virtual void Initialize(SceneManager* sceneManager) = 0;
	virtual void Update(SceneManager* sceneManager) = 0;
	virtual void Draw(SceneManager* sceneManager) = 0;
};