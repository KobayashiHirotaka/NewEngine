#pragma once
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include "Engine/Utility/Math/MyMath.h"
#include "Engine/externals/nlohmann/json.hpp"
#include <variant>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <filesystem>

using json = nlohmann::json;

struct AttackParameter 
{
    int anticipationTime = 0;

    int chargeTime = 0;

    int swingTime = 0;

    int recoveryTime = 0;
};

class AttackEditor
{
public:
    static AttackEditor* GetInstance();

    void Update();

    void SaveFile(const std::string& saveFilePath);

    void LoadFile(const std::string& loadFilePath);

private:
    AttackEditor() = default;
    ~AttackEditor() = default;
    AttackEditor(const AttackEditor& obj) = default;
    AttackEditor& operator=(const AttackEditor& obj) = default;

private:
    std::map<std::string, AttackParameter> attackParameter_;

    std::string tempTabName_;

    std::string saveFilePath_ = "resource/AttackData/AttackData.json";

    std::string loadFilePath_ = "resource/AttackData/AttackData.json";
};