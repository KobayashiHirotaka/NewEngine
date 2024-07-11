#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Model/IGame3dObject.h"
#include "Engine/3D/Model/Game3dObjectManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/Utility/Structs.h"
#include "Engine/externals/nlohmann/json.hpp"
#include <iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<cassert>

using json = nlohmann::json;

class LevelLoader
{
public:
    static const std::string kDirectoryPath;

    static const std::string kExtension;

    struct LevelData
    {
        struct ObjectData
        {
            std::string objectName;
            std::string fileName;

            Vector3 translation;
            Vector3 rotation;
            Vector3 scale;

            //キャラクターごとのデータ
            std::string type;
            float hp;
            float frontSpeed;
            float backSpeed;
        };

        struct CameraObjectData
        {
            std::string objectName;
            Vector3 translation;
            Vector3 rotation;
        };

        std::vector<LevelData::ObjectData> objects;

        std::vector<LevelData::CameraObjectData> cameras;
    };

    static LevelLoader* GetInstance();

    static void DeleteInstance();

    void LoadLevel(const std::string fileName);

    void CreateObjectsFromLevelData(const LevelData* levelData);

    void LoadObjectFromJson(LevelData* levelData, json& parent);

private:
    LevelLoader() = default;
    ~LevelLoader() = default;
    LevelLoader(const LevelLoader& obj) = default;
    LevelLoader& operator=(const LevelLoader& obj) = default;

private:
    static LevelLoader* instance_;

    std::map<std::string, std::unique_ptr<LevelData>>levelDatas_;
};