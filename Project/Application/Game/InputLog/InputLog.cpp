/**
 * @file InputLog.cpp
 * @brief 入力履歴表示、経過フレームの表示を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "InputLog.h"

void InputLog::Initialize()
{
	//Inputのinstance
	input_ = Input::GetInstance();

	//リソース
	//操作
	stickTextureHandle_[0] = TextureManager::LoadTexture("resource/images/arrow_1.png");
	stickTextureHandle_[1] = TextureManager::LoadTexture("resource/images/arrow_2.png");
	stickTextureHandle_[2] = TextureManager::LoadTexture("resource/images/arrow_3.png");
	stickTextureHandle_[3] = TextureManager::LoadTexture("resource/images/arrow_4.png");
	stickTextureHandle_[4] = TextureManager::LoadTexture("resource/images/arrow_6.png");
	stickTextureHandle_[5] = TextureManager::LoadTexture("resource/images/arrow_7.png");
	stickTextureHandle_[6] = TextureManager::LoadTexture("resource/images/arrow_8.png");
	stickTextureHandle_[7] = TextureManager::LoadTexture("resource/images/arrow_9.png");
    stickTextureHandle_[8] = TextureManager::LoadTexture("resource/images/N.png");

	//ボタン
	buttonTextureHandle_[0] = TextureManager::LoadTexture("resource/images/A.png");
	buttonTextureHandle_[1] = TextureManager::LoadTexture("resource/images/B.png");
	buttonTextureHandle_[2] = TextureManager::LoadTexture("resource/images/X.png");
	buttonTextureHandle_[3] = TextureManager::LoadTexture("resource/images/Y.png");
	buttonTextureHandle_[4] = TextureManager::LoadTexture("resource/images/LB.png");
	buttonTextureHandle_[5] = TextureManager::LoadTexture("resource/images/RB.png");

    //数字
    for (int i = 0; i < 10; ++i)
    {
        std::string path = "resource/number/" + std::to_string(i) + ".png";
        digitTextureHandles_[i] = TextureManager::LoadTexture(path.c_str());
    }

	//スプライトの初期化
	for (int i = 0; i < maxHistorySize_; i++) 
    {
		stickSprites_.emplace_back(Sprite::Create(stickTextureHandle_[0], { leftPositionX_, basePositionY_ - i * verticalSpacing_ }));
		stickSprites_.back()->SetSize({ 40.0f, 40.0f });

		buttonSprites_.emplace_back(Sprite::Create(buttonTextureHandle_[0], { rightPositionX_, basePositionY_ - i * verticalSpacing_ }));
		buttonSprites_.back()->SetSize({ 40.0f, 40.0f });

        numberTensSprites_.emplace_back(Sprite::Create(digitTextureHandles_[0], { numberTensPositionX_, basePositionY_ - i * verticalSpacing_ }));
        numberTensSprites_.back()->SetSize({ 40.0f, 40.0f });

        numberOnesSprites_.emplace_back(Sprite::Create(digitTextureHandles_[0], { numberOnesPositionX_, basePositionY_ - i * verticalSpacing_ }));
        numberOnesSprites_.back()->SetSize({ 40.0f, 40.0f });
	}
}

void InputLog::Update()
{
    int stickInput = -1;
    int buttonInput = -1;

    // スティック入力の検出
    //左
    if (input_->GetLeftStickX() < -value_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT))
    { 
        //左下
        if (input_->GetLeftStickY() < -value_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
        {
            stickInput = 0;
        }
        //左上
        else if (input_->GetLeftStickY() > value_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
        {
            stickInput = 5;
        }
        //左
        else
        {
            stickInput = 3;
        }
    }  
    //右
    else if (input_->GetLeftStickX() > value_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT))
    { 
        //右下
        if (input_->GetLeftStickY() < -value_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
        {
            stickInput = 2;
        }
        //右上
        else if (input_->GetLeftStickY() > value_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
        {
            stickInput = 7;
        }
        //右
        else
        {
            stickInput = 4;
        }
    }  
    //上
    else if (input_->GetLeftStickY() > value_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
    { 
        stickInput = 6;
    }  
    //下
    else if (input_->GetLeftStickY() < -value_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
    { 
        stickInput = 1; 
    } 
    else
    {
        stickInput = 8;
    }

    //ボタン入力の検出
    //Aボタン
    if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
    { 
        buttonInput = 0;
    }
    //Bボタン
    else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
    { 
        buttonInput = 1; 
    }
    //Xボタン
    else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
    {
        buttonInput = 2;
    }
    //Yボタン
    else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y)) 
    { 
        buttonInput = 3;
    }
    //LBボタン
    else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_LEFT_SHOULDER))
    {
        buttonInput = 4;
    }

    //入力が検出されたら履歴に追加
    if (stickInput != -1 || buttonInput != -1)
    {
        //直前の履歴を取得
        auto lastInput = inputHistory_.empty() ? std::pair<int, int>(-1, -1) : inputHistory_.front();

        //スティックまたはボタン入力が前回と異なる場合、履歴を更新
        if (stickInput != lastInput.first || buttonInput != lastInput.second)
        {
            inputHistory_.emplace_front(stickInput, buttonInput);
            frameCounts_.emplace_front(0);
        }
    }
    else
    {
        //入力がない状態の時
        if (inputHistory_.empty() || inputHistory_.front() != std::make_pair(-1, -1))
        {
            //入力履歴を最新のものに更新する
            inputHistory_.emplace_front(-1, -1);
            frameCounts_.emplace_front(0);
        }
    }

    //経過フレームを加算
    for (size_t i = 0; i < frameCounts_.size(); ++i)
    {
        //新しい入力がされた場合、過去の入力の経過フレームは停止
        if (i == 0 || inputHistory_[i] == inputHistory_[i - 1])
        {
            //現在の入力の場合のみ加算
            frameCounts_[i] = std::clamp(frameCounts_[i] + 1, 0, 99);
        }
        else
        {
            //新しい入力がされた場合、過去の入力の経過フレームを保持しつつ加算を停止
            frameCounts_[i] = frameCounts_[i]; 
        }
    }

    //履歴が最大サイズを超えたら古いものを削除
    if (inputHistory_.size() > maxHistorySize_)
    {
        //古い入力を削除
        inputHistory_.pop_back();
        frameCounts_.pop_back();
    }
}

void InputLog::Draw()
{
    //履歴のサイズを取得
    size_t historySize = inputHistory_.size();

    //各入力履歴の描画
    for (size_t i = 0; i < historySize && i < maxHistorySize_; i++)
    {
        const auto& [stick, button] = inputHistory_[i];
        float drawPosY = basePositionY_ + i * verticalSpacing_;

        //スティック入力の描画
        if (stick != -1)
        {
            stickSprites_[i]->SetPosition({ leftPositionX_, drawPosY });
            stickSprites_[i]->SetTexture(stickTextureHandle_[stick]);
            stickSprites_[i]->Draw();
        }

        //ボタン入力の描画
        if (button != -1)
        {
            //ボタンのみの入力があれば左側に表示
            float buttonPosX = (stick == -1) ? leftPositionX_ : rightPositionX_;
            buttonSprites_[i]->SetPosition({ buttonPosX, drawPosY });
            buttonSprites_[i]->SetTexture(buttonTextureHandle_[button]);
            buttonSprites_[i]->Draw();
        }

        //経過フレームの描画
        if (i < frameCounts_.size())
        {
            int frame = frameCounts_[i];
            int tens = frame / 10;  
            int ones = frame % 10; 

            //10の位の数字
            numberTensSprites_[i]->SetTexture(digitTextureHandles_[tens]);
            numberTensSprites_[i]->SetPosition({ numberTensPositionX_, drawPosY });
            numberTensSprites_[i]->Draw();

            //1の位の数字
            numberOnesSprites_[i]->SetTexture(digitTextureHandles_[ones]);
            numberOnesSprites_[i]->SetPosition({ numberOnesPositionX_, drawPosY });
            numberOnesSprites_[i]->Draw();
        }
    }
}
