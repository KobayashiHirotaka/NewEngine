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
	buttonTextureHandle_[4] = TextureManager::LoadTexture("resource/images/RB.png");
	buttonTextureHandle_[5] = TextureManager::LoadTexture("resource/images/LB.png");

	// スプライトの初期化
	for (int i = 0; i < maxHistorySize_; i++) 
    {
		stickSprites_.emplace_back(Sprite::Create(stickTextureHandle_[0], { leftPositionX_, basePositionY_ - i * verticalSpacing_ }));
		stickSprites_.back()->SetSize({ 55.0f, 55.0f });

		buttonSprites_.emplace_back(Sprite::Create(buttonTextureHandle_[0], { rightPositionX_, basePositionY_ - i * verticalSpacing_ }));
		buttonSprites_.back()->SetSize({ 55.0f, 55.0f });
	}
}

void InputLog::Update()
{
    int stickInput = -1;
    int buttonInput = -1;

    // スティック入力の検出
    //左
    if (input_->GetLeftStickX() < -value_) 
    { 
        //左下
        if (input_->GetLeftStickY() < -value_)
        {
            stickInput = 0;
        }
        //左上
        else if (input_->GetLeftStickY() > value_)
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
    else if (input_->GetLeftStickX() > value_)
    { 
        //右上
        if (input_->GetLeftStickY() < -value_)
        {
            stickInput = 2;
        }
        //右下
        else if (input_->GetLeftStickY() > value_)
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
    else if (input_->GetLeftStickY() > value_)
    { 
        stickInput = 6;
    }  
    //下
    else if (input_->GetLeftStickY() < -value_) 
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

    //入力が検出されたら履歴に追加
    if (stickInput != -1 || buttonInput != -1) 
    {
        //直前の履歴を取得
        auto lastInput = inputHistory_.empty() ? std::pair<int, int>(-1, -1) : inputHistory_.front();

        //ボタンが押された場合は履歴を更新
        if (buttonInput != -1) 
        {
            //操作が変わらない場合でもボタンが押されているなら更新
            if (stickInput == lastInput.first)
            {
                //操作が前回と同じでもボタン入力があれば履歴を追加
                inputHistory_.emplace_front(stickInput, buttonInput);
            }
            else
            {
                //スティック入力が異なる場合は通常通り追加
                inputHistory_.emplace_front(stickInput, buttonInput);
            }
        }
        else if (stickInput != lastInput.first) 
        {
            //ボタンが押されていない場合、スティックが変わったときのみ履歴を追加
            inputHistory_.emplace_front(stickInput, buttonInput);
        }
    }

    //前回がNだった場合
    if (stickInput == -1 && buttonInput == -1)
    {
        //入力がない状態の時
        if (!inputHistory_.empty() && (inputHistory_.front().first != -1 || inputHistory_.front().second != -1)) 
        {
            //入力履歴を最新のものに更新する
            inputHistory_.emplace_front(-1, -1); 
        }
    }

    //履歴が最大サイズを超えたら古いものを削除
    if (inputHistory_.size() > maxHistorySize_)
    {
        //古い入力を削除
        inputHistory_.pop_back();  
    }
}

void InputLog::Draw()
{
    //履歴のサイズを取得
    size_t historySize = inputHistory_.size();

    //各入力履歴の描画
    for (size_t index = 0; index < historySize && index < maxHistorySize_; ++index)
    {
        const auto& [stick, button] = inputHistory_[index];
        float drawPosY = basePositionY_ + index * verticalSpacing_; 

        //スティック入力の描画
        if (stick != -1)
        {
            stickSprites_[index]->SetPosition({ leftPositionX_, drawPosY });
            stickSprites_[index]->SetTexture(stickTextureHandle_[stick]);
            stickSprites_[index]->Draw();
        }

        //ボタン入力の描画
        if (button != -1)
        {
            //ボタンのみの入力があれば左側に表示
            float buttonPosX = (stick == -1) ? leftPositionX_ : rightPositionX_;
            buttonSprites_[index]->SetPosition({ buttonPosX, drawPosY });
            buttonSprites_[index]->SetTexture(buttonTextureHandle_[button]);
            buttonSprites_[index]->Draw();
        }
    }
}

