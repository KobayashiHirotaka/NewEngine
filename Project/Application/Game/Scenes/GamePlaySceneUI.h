/**
 * @file GamePlaySceneUI.h
 * @brief プレイシーンのUIの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 2024/01/16
 */

#pragma once
#include "Application/Game/Scenes/IGameSceneUI.h"
#include "Application/Game/GuideUI/GuideUI.h"

class GamePlaySceneUI : public IGameSceneUI
{
public:
	/// <summary>初期化</summary>
	void Initialize()override;

	/// <summary>更新</summary>
	void Update()override;

	/// <summary>通常時の描画</summary>
	void Draw()override;

	/// <summary>ラウンド取得の描画</summary>
	void RoundGetDraw(int playerWinCount, int enemyWinCount);

	/// <summary>ラウンド数の描画</summary>
	void RoundNumberDraw(int round);

	/// <summary>ラウンド開始時の描画</summary>
	void RoundStartDraw();

	/// <summary>ラウンド終了時の描画</summary>
	void RoundEndDraw(bool isTimeOver, bool isPlayerWin);

	/// <summary>操作説明の描画</summary>
	void GuideDraw();

	/// <summary>数字の更新</summary>
	void UpdateNumberSprite(int currentSeconds);

	//Getter
	bool GetIsOpen() { return guideUI_->GetIsOpen(); };

	bool GetIsChangedSprite() { return  guideUI_->GetIsChangedSprite(); };

	//Setter
	void SetIsChangedSprite(bool isChangedSprite) { guideUI_->SetIsChangedSprite(isChangedSprite); };

private:
	//Timer用のSprite
	std::unique_ptr<Sprite>numberTensSprite_ = nullptr;
	std::unique_ptr<Sprite>numberOnesSprite_ = nullptr;
	uint32_t tensTextureHandle_;
	uint32_t onesTextureHandle_;

	//Round表示のSprite
	std::unique_ptr<Sprite> roundSprite_[3];
	uint32_t roundTextureHandle_[3];

	std::unique_ptr<Sprite> roundGetSprite_[4];
	uint32_t roundGetTextureHandle_;

	//試合開始時用のSprite(Fightの文字)
	std::unique_ptr<Sprite> fightSprite_ = nullptr;
	uint32_t fightTextureHandle_;

	//KO表示用Sprite
	std::unique_ptr<Sprite> koSprite_ = nullptr;
	uint32_t koTextureHandle_;

	//勝敗に関するSprite
	std::unique_ptr<Sprite> winSprite_ = nullptr;
	uint32_t winTextureHandle_;

	std::unique_ptr<Sprite> loseSprite_ = nullptr;
	uint32_t loseTextureHandle_;

	std::unique_ptr<Sprite> timeOverSprite_ = nullptr;
	uint32_t timeOverTextureHandle_;

	//操作説明用のSprite
	std::unique_ptr<Sprite> UICommandListSprite_ = nullptr;
	uint32_t UICommandListTextureHandle_ = 0;

	//基本操作説明用のSprite
	std::unique_ptr<Sprite> generalCommandListSprite_ = nullptr;
	uint32_t generalCommandListTextureHandle_ = 0;

	//攻撃操作説明用のSprite
	std::unique_ptr<Sprite> attackCommandListSprite_[2];
	uint32_t attackCommandListTextureHandle_[2];

	//UI枠のSprite
	std::unique_ptr<Sprite> frameUISprite_ = nullptr;
	uint32_t frameUITextureHandle_ = 0;

	//操作説明
	std::unique_ptr<GuideUI> guideUI_ = nullptr;

	//ラウンド数
	static const int kRoundOne_ = 1;
	static const int kRoundTwo_ = 2;
	static const int kRoundThree_ = 3;

	//勝利カウント
	const int kCharacterFirstWinCount_ = 1;
	const int kCharacterSecondWinCount_ = 2;
};

