#include "LevelData.h"

LevelData* LevelData::GetInstance()
{
	static LevelData instance;

	return &instance;
}

void LevelData::Initialize(const std::string fileName)
{
	const std::string fullpath = kDirectoryPath_ + fileName + kExtension_;

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
		assert(object.contains("type"));

		//種別を取得
		std::string type = object["type"].get<std::string>();

		//TODO: objectを再帰関数にまとめ、再帰呼び出しで枝を走査する
		if (object.contains("children"))
		{

		}

		if (type.compare("MESH") == 0)
		{
			//要素追加
			levelData->objects.emplace_back(LevelData::ObjectData{});

			//今追加した要素の参照を得る
			LevelData::ObjectData& objectData = levelData->objects.back();

			if (object.contains("file_name"))
			{
				objectData.fileName = object["file_name"];
			}

			json& transform = object["transform"];

			//平行移動
			objectData.worldTransform.translation.x = (float)transform["translation"][0];
			objectData.worldTransform.translation.y = (float)transform["translation"][2];
			objectData.worldTransform.translation.z = (float)transform["translation"][1];

			//回転角
			objectData.worldTransform.rotation.x = -(float)transform["rotation"][0];
			objectData.worldTransform.rotation.y = -(float)transform["rotation"][2];
			objectData.worldTransform.rotation.z = -(float)transform["rotation"][1];

			//スケーリング
			objectData.worldTransform.scale.x = (float)transform["scaling"][0];
			objectData.worldTransform.scale.y = (float)transform["scaling"][2];
			objectData.worldTransform.scale.z = (float)transform["scaling"][1];
		}
	}
}
