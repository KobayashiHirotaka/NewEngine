#include "LevelLoader.h"

const std::string LevelLoader::kDirectoryPath = "resource/Level/";

const std::string LevelLoader::kExtension = ".json";

LevelLoader* LevelLoader::instance_ = nullptr;

LevelLoader* LevelLoader::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new LevelLoader();
	}
	return instance_;
}

void LevelLoader::DeleteInstance()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void LevelLoader::LoadLevel(const std::string fileName)
{
	const std::string fullpath = kDirectoryPath + fileName + kExtension;

	//ファイルストリーム
	std::ifstream file;

	//ファイルを開く
	file.open(fullpath);

	//ファイルオープン失敗をチェック
	if (file.fail())
	{
		assert(0);
	}

	//JSON文字列から解凍したデータ
	json deserialized;

	//解凍
	file >> deserialized;

	//正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	assert(name.compare("scene") == 0);

	//レベルデータ格納用インスタンスを取得
	LevelData* levelData = new LevelData();

	//"objects"の全objectを走査
	for (json& object : deserialized["objects"])
	{
		LoadObjectFromJson(levelData, object);
	}

	CreateObjectsFromLevelData(levelData);

	//levelDataの保存
	levelDatas_[fileName] = std::unique_ptr<LevelData>(levelData);
}

void LevelLoader::LoadObjectFromJson(LevelData* levelData, json& object)
{

	assert(object.contains("type"));

	//種別を取得
	std::string type = object["type"].get<std::string>();

	if (type.compare("MESH") == 0)
	{
		//要素追加
		levelData->objects.emplace_back(LevelData::ObjectData{});

		//今追加した要素の参照を得る
		LevelData::ObjectData& objectData = levelData->objects.back();

		if (object.contains("name"))
		{
			objectData.objectName = object["name"];
		}

		if (object.contains("file_name"))
		{
			objectData.fileName = object["file_name"];
		}

		if (object.contains("visible"))
		{
			objectData.isVisible = object["visible"].get<bool>();
		}
		else
		{
			objectData.isVisible = true;
		}

		if (objectData.objectName == "Player" || objectData.objectName == "Enemy")
		{
			json& characterData = object["character_data"];

			if (characterData.contains("hp"))
			{
				objectData.characterData.hp = (float)characterData["hp"];
			}

			if (characterData.contains("maxHp"))
			{
				objectData.characterData.maxHp = (float)characterData["maxHp"];
			}
	
			if (characterData.contains("frontSpeed")) 
			{
				objectData.characterData.frontSpeed = std::stof(characterData["frontSpeed"].get<std::string>());
			}

			if (characterData.contains("backSpeed")) 
			{
				objectData.characterData.backSpeed = std::stof(characterData["backSpeed"].get<std::string>());
			}

			if (characterData.contains("guardGauge"))
			{
				objectData.characterData.guardGauge = std::stof(characterData["guardGauge"].get<std::string>());
			}

			if (characterData.contains("maxGuardGauge"))
			{
				objectData.characterData.maxGuardGauge = std::stof(characterData["maxGuardGauge"].get<std::string>());
			}

			if (characterData.contains("finisherGauge"))
			{
				objectData.characterData.finisherGauge = std::stof(characterData["finisherGauge"].get<std::string>());
			}

			if (characterData.contains("maxFinisherGauge"))
			{
				objectData.characterData.maxFinisherGauge = std::stof(characterData["maxFinisherGauge"].get<std::string>());
			}
		}

		json& transform = object["transform"];

		//平行移動
		objectData.translation.x = (float)transform["translation"][0];
		objectData.translation.y = (float)transform["translation"][2];
		objectData.translation.z = (float)transform["translation"][1];

		//回転角
		objectData.rotation.x = -(float)transform["rotation"][0];
		objectData.rotation.y = -(float)transform["rotation"][2];
		objectData.rotation.z = -(float)transform["rotation"][1];

		//スケーリング
		objectData.scale.x = (float)transform["scaling"][0];
		objectData.scale.y = (float)transform["scaling"][2];
		objectData.scale.z = (float)transform["scaling"][1];
	}
	else if (type.compare("CAMERA") == 0)
	{
		//要素追加
		levelData->cameras.emplace_back(LevelData::CameraObjectData{});

		//今追加した要素の参照を得る
		LevelData::CameraObjectData& objectData = levelData->cameras.back();

		if (object.contains("name"))
		{
			objectData.objectName = object["name"];
		}

		json& transform = object["transform"];

		//平行移動
		objectData.translation.x = (float)transform["translation"][0];
		objectData.translation.y = (float)transform["translation"][2];
		objectData.translation.z = (float)transform["translation"][1];

		//回転角
		objectData.rotation.x = -(float)transform["rotation"][0];
		objectData.rotation.y = -(float)transform["rotation"][2];
		objectData.rotation.z = -(float)transform["rotation"][1];
	}

	//objectを再帰関数にまとめ、再帰呼び出しで枝を走査する
	if (object.contains("children"))
	{
		LoadObjectFromJson(levelData, object);
	}
}

void LevelLoader::CreateObjectsFromLevelData(const LevelData* levelData)
{
	std::string directoryPath = "resource/models/";
	std::string fileName;

	//レベルデータからobjectを生成、配置
	for (auto& objectData : levelData->objects)
	{
		if (std::filesystem::exists(directoryPath + objectData.fileName + ".obj"))
		{
			fileName = objectData.fileName + ".obj";
		}
		else if (std::filesystem::exists(directoryPath + objectData.fileName + ".gltf"))
		{
			fileName = objectData.fileName + ".gltf";
		}

		//ファイル名から登録済みモデルを検索
		std::unique_ptr<Model> model;
		model.reset(Model::CreateFromOBJ(directoryPath, fileName));

		//モデルを指定してobjectを生成
		IGame3dObject* newObject = Game3dObjectManager::CreateGameObject(objectData.objectName);

		//モデルをセット
		newObject->SetModel(std::move(model));

		//座標
		newObject->SetPosition(objectData.translation);

		//回転角
		newObject->SetRotation(objectData.rotation);

		//スケーリング
		newObject->SetScale(objectData.scale);

		newObject->SetIsVisible(objectData.isVisible);

		if (objectData.objectName == "Player" || objectData.objectName == "Enemy")
		{
			newObject->SetHp(objectData.characterData.hp);
			newObject->SetMaxHp(objectData.characterData.maxHp);
			newObject->SetFrontSpeed(objectData.characterData.frontSpeed);
			newObject->SetBackSpeed(objectData.characterData.backSpeed);
			newObject->SetGuardGauge(objectData.characterData.guardGauge);
			newObject->SetMaxGuardGauge(objectData.characterData.maxGuardGauge);
			newObject->SetFinisherGauge(objectData.characterData.finisherGauge);
			newObject->SetMaxFinisherGauge(objectData.characterData.maxFinisherGauge);
		}
	}
}