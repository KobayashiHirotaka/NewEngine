/**
 * @file GameManager.cpp
 * @brief ゲーム全体の管理（初期化、描画など）を行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "GameManager.h"

void GameManager::Initialize()
{
	//EngineCore初期化
	EngineCore::Initialize();

	//シーンファクトリーの作成と設定
	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());

	//初期シーンの設定
	sceneManager_->ChangeScene("GameTitleScene");

	//AttackEditorのインスタンスを取得、初期化
	attackEditor_ = AttackEditor::GetInstance();
	attackEditor_->Initialize();
}

void GameManager::Draw()
{
	//EngineCoreの描画処理
	EngineCore::Draw();
}


