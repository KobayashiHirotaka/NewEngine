/**
 * @file AttackEditor.cpp
 * @brief 攻撃に関するパラメータ調整ができるエディター
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "AttackEditor.h"

AttackEditor* AttackEditor::GetInstance()
{
    static AttackEditor sInstance;

    return &sInstance;
}

void AttackEditor::Initialize()
{
    LoadJapaneseFont();

    LoadFile(loadPlayerFilePath_, playerAttackParameter_);
    LoadFile(loadEnemyFilePath_, enemyAttackParameter_);
}

void AttackEditor::Update()
{
    ImGui::Begin("アタックエディター");

    if (ImGui::BeginTabBar("##tabs"))
    {
        //プレイヤータブ
        if (ImGui::BeginTabItem("プレイヤー"))
        {
            if (ImGui::Button("追加"))
            {
                std::string newTabName = "攻撃" + std::to_string(playerAttackParameter_.size() + 1);

                if (playerAttackParameter_.find(newTabName) == playerAttackParameter_.end())
                {
                    playerAttackParameter_[newTabName] = AttackParameter();
                }
            }

            for (auto it = playerAttackParameter_.begin(); it != playerAttackParameter_.end(); )
            {
                auto& [tabName, param] = *it;
                ImGui::PushID(tabName.c_str());

                char buf[256];
                strcpy_s(buf, sizeof(buf), tabName.c_str());

                if (ImGui::CollapsingHeader(buf))
                {
                    if (ImGui::InputText("攻撃の名前", buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        tempTabName_ = buf;
                        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
                        {
                            if (tempTabName_ != tabName && playerAttackParameter_.find(tempTabName_) == playerAttackParameter_.end())
                            {
                                AttackParameter tempParam = std::move(param);
                                it = playerAttackParameter_.erase(it);
                                playerAttackParameter_[tempTabName_] = std::move(tempParam);
                            }
                        }
                    }
                    else
                    {
                        ++it;
                    }

                    ImGui::SliderInt("当たり判定の付き初め", &param.attackStartTime, 0, 60);
                    ImGui::SliderInt("当たり判定の付き終わり", &param.attackEndTime, 0, 60);
                    ImGui::SliderInt("硬直時間", &param.recoveryTime, 0, 100);
                    ImGui::SliderInt("ダメージ", &param.damage, 0, 100);
                    ImGui::SliderFloat("ガードゲージ増加量", &param.guardGaugeIncreaseAmount, 0.0f, 50.0f);
                    ImGui::SliderFloat("必殺技ゲージ増加量", &param.finisherGaugeIncreaseAmount, 0.0f, 50.0f);
                    ImGui::SliderFloat("ヒットストップ", &param.hitStop, 0.0f, 1.0f);
                    
                    ImGui::Text("右向きの当たり判定");
                    ImGui::SliderFloat3("最小値(右向き)", &param.rightCollisionMin.x, -3.0f, 3.0f);
                    ImGui::SliderFloat3("最大値(右向き)", &param.rightCollisionMax.x, -3.0f, 3.0f);

                    ImGui::Text("左向きの当たり判定");
                    ImGui::SliderFloat3("最小値(左向き)", &param.leftCollisionMin.x, -3.0f, 3.0f);
                    ImGui::SliderFloat3("最大値(左向き)", &param.leftCollisionMax.x, -3.0f, 3.0f);
                }
                else
                {
                    ++it;
                }

                ImGui::PopID();
                ImGui::Separator();
            }

            if (ImGui::Button("保存"))
            {
                SaveFile(savePlayerFilePath_, playerAttackParameter_);
                std::string message = savePlayerFilePath_ + " saved";
                MessageBoxA(nullptr, message.c_str(), "AttackEditor", 0);
            }

            if (ImGui::Button("読み込み"))
            {
                LoadFile(loadPlayerFilePath_, playerAttackParameter_);
                std::string message = loadPlayerFilePath_ + " loaded";
                MessageBoxA(nullptr, message.c_str(), "AttackEditor", 0);
            }

            ImGui::EndTabItem();
        }

        //エネミータブ
        if (ImGui::BeginTabItem("エネミー"))
        {
            if (ImGui::Button("追加"))
            {
                std::string newTabName = "攻撃" + std::to_string(enemyAttackParameter_.size() + 1);

                if (enemyAttackParameter_.find(newTabName) == enemyAttackParameter_.end())
                {
                    enemyAttackParameter_[newTabName] = AttackParameter();
                }
            }

            for (auto it = enemyAttackParameter_.begin(); it != enemyAttackParameter_.end(); )
            {
                auto& [tabName, param] = *it;
                ImGui::PushID(tabName.c_str());

                char buf[256];
                strcpy_s(buf, sizeof(buf), tabName.c_str());

                if (ImGui::CollapsingHeader(buf))
                {
                    if (ImGui::InputText("攻撃の名前", buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        tempTabName_ = buf;
                        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
                        {
                            if (tempTabName_ != tabName && enemyAttackParameter_.find(tempTabName_) == enemyAttackParameter_.end())
                            {
                                AttackParameter tempParam = std::move(param);
                                it = enemyAttackParameter_.erase(it);
                                enemyAttackParameter_[tempTabName_] = std::move(tempParam);
                            }
                        }
                    }
                    else
                    {
                        ++it;
                    }

                    ImGui::SliderInt("当たり判定の付き初め", &param.attackStartTime, 0, 60);
                    ImGui::SliderInt("当たり判定の付き終わり", &param.attackEndTime, 0, 60);
                    ImGui::SliderInt("硬直時間", &param.recoveryTime, 0, 100);
                    ImGui::SliderInt("ダメージ", &param.damage, 0, 100);
                    ImGui::SliderFloat("ガードゲージ増加量", &param.guardGaugeIncreaseAmount, 0.0f, 50.0f);
                    ImGui::SliderFloat("必殺技ゲージ増加量", &param.finisherGaugeIncreaseAmount, 0.0f, 50.0f);
                    ImGui::SliderFloat("ヒットストップ", &param.hitStop, 0.0f, 1.0f);

                    ImGui::Text("右向きの当たり判定");
                    ImGui::SliderFloat3("最小値(右向き)", &param.rightCollisionMin.x, -3.0f, 3.0f);
                    ImGui::SliderFloat3("最大値(右向き)", &param.rightCollisionMax.x, -3.0f, 3.0f);

                    ImGui::Text("左向きの当たり判定");
                    ImGui::SliderFloat3("最小値(左向き)", &param.leftCollisionMin.x, -3.0f, 3.0f);
                    ImGui::SliderFloat3("最大値(左向き)", &param.leftCollisionMax.x, -3.0f, 3.0f);
                }
                else
                {
                    ++it;
                }

                ImGui::PopID();
                ImGui::Separator();
            }

            if (ImGui::Button("保存"))
            {
                SaveFile(saveEnemyFilePath_, enemyAttackParameter_);
                std::string message = saveEnemyFilePath_ + " saved";
                MessageBoxA(nullptr, message.c_str(), "AttackEditor", 0);
            }

            if (ImGui::Button("読み込み"))
            {
                LoadFile(loadEnemyFilePath_, enemyAttackParameter_);
                std::string message = loadEnemyFilePath_ + " loaded";
                MessageBoxA(nullptr, message.c_str(), "AttackEditor", 0);
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void AttackEditor::LoadJapaneseFont()
{
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig config;
    config.MergeMode = false; 
    config.PixelSnapH = true;

    //日本語フォントの指定
    if (io.Fonts->AddFontFromFileTTF("resource/AttackData/NotoSansJP-Medium.ttf", 18.0f, &config, io.Fonts->GetGlyphRangesJapanese()))
    {
        io.Fonts->Build(); 
    }
}

void AttackEditor::SaveFile(const std::string& saveFilePath, const std::unordered_map<std::string, AttackParameter>& attackParameters)
{
    json root = json::object();

    for (const auto& [tabName, param] : attackParameters)
    {
        root[tabName] = {
            {"attackStartTime", param.attackStartTime},
            {"attackEndTime", param.attackEndTime},
            {"recoveryTime", param.recoveryTime},
            {"damage", param.damage},
            {"guardGaugeIncreaseAmount", param.guardGaugeIncreaseAmount},
            {"finisherGaugeIncreaseAmount", param.finisherGaugeIncreaseAmount},
            {"hitStop",param.hitStop},
            {"rightCollisionMin", {{"x", param.rightCollisionMin.x}, {"y", param.rightCollisionMin.y}, {"z", param.rightCollisionMin.z}}},
            {"rightCollisionMax", {{"x", param.rightCollisionMax.x}, {"y", param.rightCollisionMax.y}, {"z", param.rightCollisionMax.z}}},
            {"leftCollisionMin", {{"x", param.leftCollisionMin.x}, {"y", param.leftCollisionMin.y}, {"z", param.leftCollisionMin.z}}},
            {"leftCollisionMax", {{"x", param.leftCollisionMax.x}, {"y", param.leftCollisionMax.y}, {"z", param.leftCollisionMax.z}}}
        };
    }

    std::filesystem::path dir("resource/AttackData");
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directory(dir);
    }

    std::ofstream ofs(saveFilePath);
    if (ofs.fail())
    {
        std::string message = "Failed to open data file for write";
        MessageBoxA(nullptr, message.c_str(), "AttackEditor", 0);
        assert(0);
        return;
    }

    ofs << std::setw(4) << root << std::endl;
    ofs.close();
}

void AttackEditor::LoadFile(const std::string& loadFilePath, std::unordered_map<std::string, AttackParameter>& attackParameters)
{
    std::ifstream ifs(loadFilePath);
    if (ifs.fail())
    {
        std::string message = "Failed to open data file for read";
        MessageBoxA(nullptr, message.c_str(), "AttackEditor", 0);
        assert(0);
        return;
    }

    json root;
    ifs >> root;
    ifs.close();

    attackParameters.clear();

    for (auto it = root.begin(); it != root.end(); ++it)
    {
        const std::string& tabName = it.key();
        const json& param = it.value();

        attackParameters[tabName] = {
            param["attackStartTime"].get<int>(),
            param["attackEndTime"].get<int>(),
            param["recoveryTime"].get<int>(),
            param["damage"].get<int>(),
            param["guardGaugeIncreaseAmount"].get<float>(),
            param["finisherGaugeIncreaseAmount"].get<float>(),
            param["hitStop"].get<float>(),
            {param["rightCollisionMin"]["x"].get<float>(), param["rightCollisionMin"]["y"].get<float>(), param["rightCollisionMin"]["z"].get<float>()},
            {param["rightCollisionMax"]["x"].get<float>(), param["rightCollisionMax"]["y"].get<float>(), param["rightCollisionMax"]["z"].get<float>()},
            {param["leftCollisionMin"]["x"].get<float>(), param["leftCollisionMin"]["y"].get<float>(), param["leftCollisionMin"]["z"].get<float>()},
            {param["leftCollisionMax"]["x"].get<float>(), param["leftCollisionMax"]["y"].get<float>(), param["leftCollisionMax"]["z"].get<float>()}
        };
    }
}


void AttackEditor::SetAttackParameters(const std::string& name, int& attackStartTime, int& attackEndTime, int& recoveryTime, int& damage,
    float& guardGaugeIncreaseAmount, float& finisherGaugeIncreaseAmount, float& hitStop, AABB& collision, bool isPlayer,
    Direction& direction)
{
    const auto& attackParameters = isPlayer ? playerAttackParameter_ : enemyAttackParameter_;

    auto it = attackParameters.find(name);
    if (it != attackParameters.end())
    {
        attackStartTime = it->second.attackStartTime;
        attackEndTime = it->second.attackEndTime;
        recoveryTime = it->second.recoveryTime;
        damage = it->second.damage;
        guardGaugeIncreaseAmount = it->second.guardGaugeIncreaseAmount;
        finisherGaugeIncreaseAmount = it->second.finisherGaugeIncreaseAmount;
        hitStop = it->second.hitStop;
        
        //向きに応じてAABBを設定
        if (direction == Direction::Right)
        {
            collision.min = it->second.rightCollisionMin;
            collision.max = it->second.rightCollisionMax;
        }
        else
        {
            collision.min = it->second.leftCollisionMin;
            collision.max = it->second.leftCollisionMax;
        }
    }
}