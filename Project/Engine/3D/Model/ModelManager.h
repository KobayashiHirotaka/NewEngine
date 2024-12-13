/**
 * @file ModelManager.h
 * @brief モデルの管理(読み込み、検索)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

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
	/// <summary>インスタンスの取得</summary>
	static ModelManager* GetInstance();

	/// <summary>インスタンスの削除</summary>
	static void DeleteInstance();

	/// <summary>初期化</summary>
	void Initialize();

	/// <summary>モデル読み込み</summary>
	void LoadModel(const std::string& directoryPath, const std::string& filename);

	/// <summary>モデル検索</summary>
	Model* FindModel(const std::string& filePath);

private:
	//シングルトン
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

private:
	//ModelManagerのインスタンス
	static ModelManager* sInstance_;

	//モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models_;
};

