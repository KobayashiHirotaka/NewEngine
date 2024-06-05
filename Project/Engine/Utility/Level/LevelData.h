#pragma once
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/Utility/Structs.h"
#include "Engine/externals/nlohmann/json.hpp"
#include<string>
#include<sstream>
#include<fstream>
#include<cassert>

using json = nlohmann::json;

class LevelData
{
public:
    struct ObjectData
    {
        std::string fileName;
        WorldTransform worldTransform;
    };

    static LevelData* GetInstance();

	void Update(const std::string fileName);

public:
    std::vector<ObjectData> objects;

private:
    LevelData() = default;

    ~LevelData() = default;

    LevelData(const LevelData& obj) = default;

    LevelData& operator=(const LevelData& obj) = default;

private:
	const std::string kDirectoryPath_ = "resource/Level/";

    const std::string kExtension_ = ".json";
};

