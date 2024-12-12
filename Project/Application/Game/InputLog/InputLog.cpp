/**
 * @file InputLog.cpp
 * @brief 入力履歴表示、経過フレームの表示を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "InputLog.h"

void InputLog::Initialize()
{
    //Inputのインスタンスの取得
	input_ = Input::GetInstance();

	//リソース
	//操作(矢印)
    const std::vector<int> arrowIndices = { 1, 2, 3, 4, 6, 7, 8, 9 };  

    for (size_t i = 0; i < arrowIndices.size(); ++i)
    {
        std::string path = "resource/images/arrow_" + std::to_string(arrowIndices[i]) + ".png";
        stickTextureHandle_[i] = TextureManager::LoadTexture(path.c_str());
    }

    //操作(N)
    stickTextureHandle_[8] = TextureManager::LoadTexture("resource/images/N.png");

	//ボタン
	buttonTextureHandle_[0] = TextureManager::LoadTexture("resource/images/A.png");
	buttonTextureHandle_[1] = TextureManager::LoadTexture("resource/images/B.png");
	buttonTextureHandle_[2] = TextureManager::LoadTexture("resource/images/X.png");
	buttonTextureHandle_[3] = TextureManager::LoadTexture("resource/images/Y.png");
	buttonTextureHandle_[4] = TextureManager::LoadTexture("resource/images/LB.png");
	buttonTextureHandle_[5] = TextureManager::LoadTexture("resource/images/RB.png");

    //数字
    for (int i = 0; i < kMaxNum_; ++i)
    {
        std::string path = "resource/number/" + std::to_string(i) + ".png";
        digitTextureHandle_[i] = TextureManager::LoadTexture(path.c_str());
    }

	//スプライトの初期化
	for (int i = 0; i < kMaxHistorySize_; i++) 
    {
		stickSprites_.emplace_back(Sprite::Create(stickTextureHandle_[0], { kLeftPositionX_, kBasePositionY_ - i * kVerticalSpacing_ }));
		stickSprites_.back()->SetSize({ textureSize_.x, textureSize_.y });

		buttonSprites_.emplace_back(Sprite::Create(buttonTextureHandle_[0], { kRightPositionX_, kBasePositionY_ - i * kVerticalSpacing_ }));
		buttonSprites_.back()->SetSize({ textureSize_.x, textureSize_.y });

        numberTensSprites_.emplace_back(Sprite::Create(digitTextureHandle_[0], { kNumberTensPositionX_, kBasePositionY_ - i * kVerticalSpacing_ }));
        numberTensSprites_.back()->SetSize({ textureSize_.x, textureSize_.y });

        numberOnesSprites_.emplace_back(Sprite::Create(digitTextureHandle_[0], { kNumberTensPositionX_, kBasePositionY_ - i * kVerticalSpacing_ }));
        numberOnesSprites_.back()->SetSize({ textureSize_.x, textureSize_.y });
	}
}

void InputLog::Update()
{
    int stickInput = kNoInput_;
    int buttonInput = kNoInput_;

    // スティック入力の検出
    //左
    if (input_->GetLeftStickX() < -kValue_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_LEFT))
    { 
        //左下
        if (input_->GetLeftStickY() < -kValue_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
        {
            stickDirection_ = StickDirection::DownLeft;
        }
        //左上
        else if (input_->GetLeftStickY() > kValue_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
        {
            stickDirection_ = StickDirection::UpLeft;
        }
        //左
        else
        {
            stickDirection_ = StickDirection::Left;
        }
    }  
    //右
    else if (input_->GetLeftStickX() > kValue_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_RIGHT))
    { 
        //右下
        if (input_->GetLeftStickY() < -kValue_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
        {
            stickDirection_ = StickDirection::DownRight;
        }
        //右上
        else if (input_->GetLeftStickY() > kValue_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
        {
            stickDirection_ = StickDirection::UpRight;
        }
        //右
        else
        {
            stickDirection_ = StickDirection::Right;
        }
    }  
    //上
    else if (input_->GetLeftStickY() > kValue_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_UP))
    { 
        stickDirection_ = StickDirection::Up;
    }  
    //下
    else if (input_->GetLeftStickY() < -kValue_ || input_->IsPressButton(XINPUT_GAMEPAD_DPAD_DOWN))
    { 
        stickDirection_ = StickDirection::Down;
    } 
    else
    {
        stickDirection_ = StickDirection::Neutral;
    }

    stickInput = static_cast<int>(stickDirection_);

    //ボタン入力の検出
    //Aボタン
    if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
    { 
        button_ = Button::A;
        buttonInput = static_cast<int>(button_);
    }
    //Bボタン
    else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
    { 
        button_ = Button::B;
        buttonInput = static_cast<int>(button_);
    }
    //Xボタン
    else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
    {
        button_ = Button::X;
        buttonInput = static_cast<int>(button_);
    }
    //Yボタン
    else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_Y)) 
    { 
        button_ = Button::Y;
        buttonInput = static_cast<int>(button_);
    }
    //LBボタン
    else if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_LEFT_SHOULDER))
    {
        button_ = Button::LB;
        buttonInput = static_cast<int>(button_);
    }
    else
    {
        buttonInput = kNoInput_;
    }

    //入力が検出されたら履歴に追加
    if (stickInput != kNoInput_ || buttonInput != kNoInput_)
    {
        //直前の履歴を取得
        auto lastInput = inputHistory_.empty() ? std::pair<int, int>(kNoInput_, kNoInput_) : inputHistory_.front();

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
        if (inputHistory_.empty() || inputHistory_.front() != std::make_pair(kNoInput_, kNoInput_))
        {
            //入力履歴を最新のものに更新する
            inputHistory_.emplace_front(kNoInput_, kNoInput_);
            frameCounts_.emplace_front(0);
        }
    }

    //最小フレーム数
    const int kMinFrameCount = 0;

    //最大フレーム数
    const int kMaxFrameCount = 99;  

    //フレームの加算値
    const int kFrameIncrement = 1;  

    //経過フレームを加算
    for (size_t i = 0; i < frameCounts_.size(); ++i)
    {
        //新しい入力がされた場合、過去の入力の経過フレームはそのまま保持
        if (i == 0 || inputHistory_[i] == inputHistory_[i - kFrameIncrement])
        {
            //現在の入力の場合のみ加算
            frameCounts_[i] = std::clamp(frameCounts_[i] + kFrameIncrement, kMinFrameCount, kMaxFrameCount);
        }
        else
        {
            //新しい入力がされた場合、過去の入力の経過フレームをそのまま保持
            frameCounts_[i] = frameCounts_[i]; 
        }
    }

    //履歴が最大サイズを超えたら古いものを削除
    if (inputHistory_.size() > kMaxHistorySize_)
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
    for (size_t i = 0; i < historySize && i < kMaxHistorySize_; i++)
    {
        const auto& [stick, button] = inputHistory_[i];
        float drawPosY = kBasePositionY_ + i * kVerticalSpacing_;

        //スティック入力の描画
        if (stick != kNoInput_)
        {
            stickSprites_[i]->SetPosition({ kLeftPositionX_, drawPosY });
            stickSprites_[i]->SetTexture(stickTextureHandle_[stick]);
            stickSprites_[i]->Draw();
        }

        //ボタン入力の描画
        if (button != kNoInput_)
        {
            //ボタンのみの入力があれば左側に表示
            float buttonPosX = (stick == kNoInput_) ? kLeftPositionX_ : kRightPositionX_;
            buttonSprites_[i]->SetPosition({ buttonPosX, drawPosY });
            buttonSprites_[i]->SetTexture(buttonTextureHandle_[button]);
            buttonSprites_[i]->Draw();
        }

        //10進数の桁を分けるための定数
        const int kBaseForDecimal = 10;

        //経過フレームの描画
        if (i < frameCounts_.size())
        {
            int frame = frameCounts_[i];
            int tens = frame / kBaseForDecimal;
            int ones = frame % kBaseForDecimal;

            //10の位の数字
            numberTensSprites_[i]->SetTexture(digitTextureHandle_[tens]);
            numberTensSprites_[i]->SetPosition({ kNumberTensPositionX_, drawPosY });
            numberTensSprites_[i]->Draw();

            //1の位の数字
            numberOnesSprites_[i]->SetTexture(digitTextureHandle_[ones]);
            numberOnesSprites_[i]->SetPosition({ kNumberOnesPositionX_, drawPosY });
            numberOnesSprites_[i]->Draw();
        }
    }
}
