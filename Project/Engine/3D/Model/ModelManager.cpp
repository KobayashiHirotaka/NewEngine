#include "ModelManager.h"

ModelManager* ModelManager::instance_ = nullptr;

ModelManager* ModelManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new ModelManager();
	}
	return instance_;
}

void ModelManager::DeleteInstance()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void ModelManager::Initialize()
{

}

void ModelManager::LoadModel(const std::string& directoryPath, const std::string& filename)
{
	if (models_.contains(filename))
	{
		return;
	}

	std::unique_ptr<Model> model = std::make_unique<Model>();
	model.reset(Model::CreateFromOBJ(directoryPath, filename));

	models_.insert(std::make_pair(filename, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath)
{
	if (models_.contains(filePath))
	{
		return models_.at(filePath).get();
	}

	return nullptr;
}


