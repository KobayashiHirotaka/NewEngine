/**
 * @file ModelManager.cpp
 * @brief モデルの管理(読み込み、検索)を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "ModelManager.h"

ModelManager* ModelManager::sInstance_ = nullptr;

ModelManager* ModelManager::GetInstance()
{
	//インスタンスを生成
	if (sInstance_ == nullptr)
	{
		sInstance_ = new ModelManager();
	}
	return sInstance_;
}

void ModelManager::DeleteInstance()
{
	//インスタンスを削除
	if (sInstance_ != nullptr)
	{
		delete sInstance_;
		sInstance_ = nullptr;
	}
}

void ModelManager::Initialize()
{

}

void ModelManager::LoadModel(const std::string& directoryPath, const std::string& filename)
{
	//すでに読み込まれているモデルがある場合、処理を終了
	if (models_.contains(filename))
	{
		return;
	}

	//新しいモデルオブジェクトを作成し、モデルを読み込む
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model.reset(Model::CreateFromOBJ(directoryPath, filename));

	models_.insert(std::make_pair(filename, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath)
{
	//モデルが見つかれば、そのポインタを返す
	if (models_.contains(filePath))
	{
		return models_.at(filePath).get();
	}

	return nullptr;
}


