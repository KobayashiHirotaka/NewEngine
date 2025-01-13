/**
 * @file LevelLoader.h
 * @brief Blenderからオブジェクトのデータを読み込んでゲームに反映
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Model/IGame3dObject.h"
#include "Engine/3D/Model/Game3dObjectManager.h"
#include "Engine/3D/WorldTransform/WorldTransform.h"
#include "Engine/Utility/Structs.h"
#include "Engine/Externals/nlohmann/json.hpp"
#include <iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<cassert>

using json = nlohmann::json;

class LevelLoader
{
public:
    //ディレクトリパス
    static const std::string kDirectoryPath;

    //ファイル拡張子
    static const std::string kExtension;

    struct LevelData
    {
        struct ObjectData
        {
            //名前
            std::string objectName;
            std::string fileName;

            //位置
            Vector3 translation;

            //回転
            Vector3 rotation;

            //スケール
            Vector3 scale;

            //表示されているか
            bool isVisible;
        };

        struct CameraObjectData
        {
            //名前
            std::string objectName;

            //位置
            Vector3 translation;

            //回転
            Vector3 rotation;
        };

        //オブジェクトデータ
        std::vector<LevelData::ObjectData> objects;
        std::vector<LevelData::CameraObjectData> cameras;
    };

    /// <summary>インスタンスの取得</summary>
    static LevelLoader* GetInstance();

    /// <summary>インスタンスの削除</summary>
    static void DeleteInstance();

    /// <summary>レベルデータの読み込み</summary>
    void LoadLevel(const std::string fileName);

    /// <summary>レベルデータからオブジェクトを生成</summary>
    void CreateObjectsFromLevelData(const LevelData* levelData);

    /// <summary>JSONからレベルデータを読み込む</summary>
    void LoadObjectFromJson(LevelData* levelData, json& parent);

private:
    //シングルトン
    LevelLoader() = default;
    ~LevelLoader() = default;
    LevelLoader(const LevelLoader& obj) = default;
    LevelLoader& operator=(const LevelLoader& obj) = default;

private:
    //LevelLoaderのインスタンス
    static LevelLoader* sInstance_;

    //レベルデータ
    std::map<std::string, std::unique_ptr<LevelData>>levelDatas_;
};