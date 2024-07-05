#pragma once
#include "Engine/3D/Model/Model.h"
#include <cassert>
#include <dxcapi.h>
#include <fstream>
#include <list>
#include <string>
#include <sstream>
#include <map>

class ModelManager
{
public:
	static ModelManager* GetInstance();

	static void DeleteInstance();

	void Initialize();

	void LoadModel(const std::string& directoryPath, const std::string& filename);

	Model* FindModel(const std::string& filePath);

private:
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

private:
	static ModelManager* instance_;

	//モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models;
};

