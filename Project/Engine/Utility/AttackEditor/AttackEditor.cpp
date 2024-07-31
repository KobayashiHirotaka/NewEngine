#include "AttackEditor.h"

AttackEditor* AttackEditor::GetInstance()
{
    static AttackEditor instance;

    return &instance;
}


void AttackEditor::Update() 
{
    ImGui::Begin("Attack Editor");

    if (ImGui::Button("Add")) 
    {
        std::string newTabName = "newAttack" + std::to_string(attackParameter_.size() + 1);

        if (attackParameter_.find(newTabName) == attackParameter_.end()) 
        {
            attackParameter_[newTabName] = AttackParameter();
        }
    }

    for (auto it = attackParameter_.begin(); it != attackParameter_.end(); )
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
                    if (tempTabName_ != tabName && attackParameter_.find(tempTabName_) == attackParameter_.end()) 
                    {
                        AttackParameter tempParam = std::move(param);
                        it = attackParameter_.erase(it);
                        attackParameter_[tempTabName_] = std::move(tempParam);
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
        SaveFile(saveFilePath_);
        std::string message = saveFilePath_ + " saved";
        MessageBoxA(nullptr, message.c_str(), "AttackEditor", 0);
    }

    if (ImGui::Button("Load")) 
    {
        LoadFile(loadFilePath_);
        std::string message = loadFilePath_ + " loaded";
        MessageBoxA(nullptr, message.c_str(), "AttackEditor", 0);
    }

    ImGui::End();
}

void AttackEditor::SaveFile(const std::string& saveFilePath)
{
    json root = json::object();

    for (const auto& [tabName, param] : attackParameter_)
    {
        root[tabName] = {
            {"attackStartTime", param.attackStartTime},
            {"attackEndTime", param.attackEndTime},
            {"recoveryTime", param.recoveryTime}
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

void AttackEditor::LoadFile(const std::string& loadFilePath)
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

    attackParameter_.clear();

    for (auto it = root.begin(); it != root.end(); ++it) 
    {
        const std::string& tabName = it.key();
        const json& param = it.value();

        attackParameter_[tabName] = {
            param["attackStartTime"].get<int>(),
            param["attackEndTime"].get<int>(),
            param["recoveryTime"].get<int>()
        };
    }
}

void AttackEditor::SetAttackParameters(const std::string& name, int& attackStartTime, int& attackEndTime, int& recoveryTime)
{
    auto it = attackParameter_.find(name);
    if (it != attackParameter_.end()) 
    {
        attackStartTime = it->second.attackStartTime;
        attackEndTime = it->second.attackEndTime;
        recoveryTime = it->second.recoveryTime;
    }
}