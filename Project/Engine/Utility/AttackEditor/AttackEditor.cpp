#include "AttackEditor.h"

AttackEditor* AttackEditor::GetInstance()
{
    static AttackEditor instance;

    return &instance;
}

void AttackEditor::Initialize()
{
    LoadFile(loadPlayerFilePath_, playerAttackParameter_);
    LoadFile(loadEnemyFilePath_, enemyAttackParameter_);
}

void AttackEditor::Update()
{
    ImGui::Begin("Attack Editor");

    if (ImGui::BeginTabBar("##tabs"))
    {
        // Player タブ
        if (ImGui::BeginTabItem("Player"))
        {
            if (ImGui::Button("Add"))
            {
                std::string newTabName = "newAttack" + std::to_string(playerAttackParameter_.size() + 1);

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

                if (ImGui::CollapsingHeader(buf, ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::InputText("##TabName", buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue))
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

                    ImGui::SliderInt("attackStartTime", &param.attackStartTime, 0, 60);
                    ImGui::SliderInt("attackEndTime", &param.attackEndTime, 0, 60);
                    ImGui::SliderInt("recoveryTime", &param.recoveryTime, 0, 100);
                    ImGui::SliderInt("damage", &param.damage, 0, 100);
                    ImGui::SliderFloat("guardGaugeIncreaseAmount", &param.guardGaugeIncreaseAmount, 0.0f, 50.0f);
                    ImGui::SliderFloat("finisherGaugeIncreaseAmount", &param.finisherGaugeIncreaseAmount, 0.0f, 50.0f);
                }
                else
                {
                    ++it;
                }

                ImGui::PopID();
                ImGui::Separator();
            }

            if (ImGui::Button("Save"))
            {
                SaveFile(savePlayerFilePath_, playerAttackParameter_);
                std::string message = savePlayerFilePath_ + " saved";
                MessageBoxA(nullptr, message.c_str(), "AttackEditor", 0);
            }

            if (ImGui::Button("Load"))
            {
                LoadFile(loadPlayerFilePath_, playerAttackParameter_);
                std::string message = loadPlayerFilePath_ + " loaded";
                MessageBoxA(nullptr, message.c_str(), "AttackEditor", 0);
            }

            ImGui::EndTabItem();
        }

        // Enemy タブ
        if (ImGui::BeginTabItem("Enemy"))
        {
            if (ImGui::Button("Add"))
            {
                std::string newTabName = "newAttack" + std::to_string(enemyAttackParameter_.size() + 1);

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

                if (ImGui::CollapsingHeader(buf, ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::InputText("##TabName", buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue))
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

                    ImGui::SliderInt("attackStartTime", &param.attackStartTime, 0, 60);
                    ImGui::SliderInt("attackEndTime", &param.attackEndTime, 0, 60);
                    ImGui::SliderInt("recoveryTime", &param.recoveryTime, 0, 100);
                    ImGui::SliderInt("damage", &param.damage, 0, 100);
                    ImGui::SliderFloat("guardGaugeIncreaseAmount", &param.guardGaugeIncreaseAmount, 0.0f, 50.0f);
                    ImGui::SliderFloat("finisherGaugeIncreaseAmount", &param.finisherGaugeIncreaseAmount, 0.0f, 50.0f);
                }
                else
                {
                    ++it;
                }

                ImGui::PopID();
                ImGui::Separator();
            }

            if (ImGui::Button("Save"))
            {
                SaveFile(saveEnemyFilePath_, enemyAttackParameter_);
                std::string message = saveEnemyFilePath_ + " saved";
                MessageBoxA(nullptr, message.c_str(), "AttackEditor", 0);
            }

            if (ImGui::Button("Load"))
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
            {"finisherGaugeIncreaseAmount", param.finisherGaugeIncreaseAmount}
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
            param["finisherGaugeIncreaseAmount"].get<float>()
        };
    }
}


void AttackEditor::SetAttackParameters(const std::string& name, int& attackStartTime, int& attackEndTime, int& recoveryTime,
    int& damage, float guardGaugeIncreaseAmount, float finisherGaugeIncreaseAmount, bool isPlayer)
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
    }
}