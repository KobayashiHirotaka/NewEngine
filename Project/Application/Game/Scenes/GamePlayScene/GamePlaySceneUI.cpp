/**
 * @file GamePlaySceneUI.cpp
 * @brief プレイシーンのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 2024/01/16
 */

#include "GamePlaySceneUI.h"

void GamePlaySceneUI::Initialize()
{
	//UIの初期化
	//ラウンド表示
	roundTextureHandle_[0] = Engine::TextureManager::LoadTexture("Resource/Images/Round1.png");
	roundTextureHandle_[1] = Engine::TextureManager::LoadTexture("Resource/Images/Round2.png");
	roundTextureHandle_[2] = Engine::TextureManager::LoadTexture("Resource/Images/Round3.png");

	roundSprite_[0].reset(Sprite::Create(roundTextureHandle_[0], { 0.0f, 0.0f }));
	roundSprite_[1].reset(Sprite::Create(roundTextureHandle_[1], { 0.0f, 0.0f }));
	roundSprite_[2].reset(Sprite::Create(roundTextureHandle_[2], { 0.0f, 0.0f }));

	//ラウンド取得数表示
	roundGetTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/RoundGet.png");

	const Vector2 kRoundGetPosition[4] = { { 400.0f, 70.0f }, { 480.0f, 70.0f }, { 800.0f, 70.0f }, { 720.0f, 70.0f } };

	roundGetSprite_[0].reset(Sprite::Create(roundGetTextureHandle_, kRoundGetPosition[0]));
	roundGetSprite_[1].reset(Sprite::Create(roundGetTextureHandle_, kRoundGetPosition[1]));
	roundGetSprite_[2].reset(Sprite::Create(roundGetTextureHandle_, kRoundGetPosition[2]));
	roundGetSprite_[3].reset(Sprite::Create(roundGetTextureHandle_, kRoundGetPosition[3]));

	//ラウンド開始時
	fightTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/FIGHT.png");

	fightSprite_.reset(Sprite::Create(fightTextureHandle_, { 0.0f, 0.0f }));

	//KO表示
	koTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/KO.png");

	koSprite_.reset(Sprite::Create(koTextureHandle_, { 0.0f, 0.0f }));

	//勝敗表示
	winTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/WIN.png");
	loseTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/LOSE.png");
	timeOverTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/TIMEOVER.png");

	winSprite_.reset(Sprite::Create(winTextureHandle_, { 0.0f, 0.0f }));
	loseSprite_.reset(Sprite::Create(loseTextureHandle_, { 0.0f, 0.0f }));
	timeOverSprite_.reset(Sprite::Create(timeOverTextureHandle_, { 0.0f, 0.0f }));

	//UIの枠
	frameUITextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Images/FrameUI.png");
	frameUISprite_.reset(Sprite::Create(frameUITextureHandle_, { 0.0f, 0.0f }));

	//数字
	tensTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Number/0.png");
	onesTextureHandle_ = Engine::TextureManager::LoadTexture("Resource/Number/0.png");

	const Vector2 kNumberTensPosition = { 590.0f, 0.0f };
	const Vector2 kNumberOnesPosition = { 630.0f, 0.0f };

	numberTensSprite_.reset(Sprite::Create(tensTextureHandle_, kNumberTensPosition));
	numberOnesSprite_.reset(Sprite::Create(onesTextureHandle_, kNumberOnesPosition));
}

void GamePlaySceneUI::Update()
{
	
}

void GamePlaySceneUI::Draw()
{
	//Ui用の枠の描画
	frameUISprite_->Draw();

	//タイマー用の数字の描画
	numberOnesSprite_->Draw();
	numberTensSprite_->Draw();
}

void GamePlaySceneUI::RoundGetDraw(int playerWinCount, int enemyWinCount)
{
	//ラウンド取得時の描画
	if (playerWinCount >= kCharacterFirstWinCount_)
	{
		roundGetSprite_[1]->Draw();
	}

	if (playerWinCount >= kCharacterSecondWinCount_)
	{
		roundGetSprite_[0]->Draw();
	}


	if (enemyWinCount >= kCharacterFirstWinCount_)
	{
		roundGetSprite_[3]->Draw();
	}

	if (enemyWinCount >= kCharacterSecondWinCount_)
	{
		roundGetSprite_[2]->Draw();
	}
}

void GamePlaySceneUI::RoundNumberDraw(int round)
{
	//ラウンド数字の描画
	if (round == kRoundOne_)
	{
		roundSprite_[0]->Draw();
	}
	else if (round == kRoundTwo_)
	{
		roundSprite_[1]->Draw();
	}
	else if (round == kRoundThree_)
	{
		roundSprite_[2]->Draw();
	}
}

void GamePlaySceneUI::RoundStartDraw()
{
	//ラウンド開始時の描画
	fightSprite_->Draw();
}

void GamePlaySceneUI::RoundEndDraw(bool isTimeOver, bool isPlayerWin)
{
	//ラウンドが終わった時の描画
	if (!isTimeOver)
	{
		if (isPlayerWin)
		{
			koSprite_->Draw();
		}
		else
		{
			koSprite_->Draw();
		}
	}
	else
	{
		timeOverSprite_->Draw();
	}
}

void GamePlaySceneUI::UpdateNumberSprite(int currentSeconds)
{
	//時間表示の更新
	const int kDecimalBase = 10;
	int tensDigit = currentSeconds / kDecimalBase;
	int onesDigit = currentSeconds % kDecimalBase;

	tensTextureHandle_ = Engine::TextureManager::LoadTexture("resource/number/" + std::to_string(tensDigit) + ".png");
	onesTextureHandle_ = Engine::TextureManager::LoadTexture("resource/number/" + std::to_string(onesDigit) + ".png");

	numberTensSprite_->SetTexture(tensTextureHandle_);
	numberOnesSprite_->SetTexture(onesTextureHandle_);
}