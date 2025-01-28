/**
 * @file GameLoseScene.cpp
 * @brief 敗北シーンの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "GameLoseScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameLoseScene::GameLoseScene() {};

GameLoseScene::~GameLoseScene() {};

void GameLoseScene::Initialize()
{
	//TextureManagerのインスタンスの取得
	textureManager_ = Engine::TextureManager::GetInstance();

	//ModelManagerのインスタンスの取得
	modelManager_ = ModelManager::GetInstance();

	//Inputのインスタンスの取得
	input_ = Engine::Input::GetInstance();

	//Audioのインスタンスの取得
	audio_ = Engine::Audio::GetInstance();

	//PostEffectの切り替え
	Engine::PostProcess::GetInstance()->SetIsGrayScaleActive(false);
	Engine::PostProcess::GetInstance()->SetIsVignetteActive(false);

	//DebugCameraの初期化
	debugCamera_.Initialize();

	//Skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	//UI生成、初期化
	gameLoseSceneUI_ = std::make_unique<GameLoseSceneUI>();
	gameLoseSceneUI_->Initialize();

	//サウンド
	selectSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/Select.mp3");

	//Transition生成、初期化
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();
};

void GameLoseScene::Update()
{
#ifdef _DEBUG

	//デバッグ用のシーン切り替え
	if (input_->PushKey(DIK_SPACE))
	{
		isTransitionStart_ = true;
		audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
	}

#endif 

	//Skydomeの更新
	skydome_->Update();

	//シーン切り替え
	if (input_->GetJoystickState())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			if (isTransitionEnd_)
			{
				isTransitionStart_ = true;

				if (!isPlayAudio_)
				{
					audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
				}

				isPlayAudio_ = true;
			}
		}
	}

	//Transition終了処理
	transition_->EndSceneTransition(isTransitionEnd_);

	//Transition開始処理
	transition_->StartSceneTransition(isTransitionStart_, sceneManager_, "GameTitleScene");

	//Camera、DebugCameraの処理
	debugCamera_.Update();

	if (input_->PushKey(DIK_K))
	{
		isDebugCamera_ = true;
	}
	else if (input_->PushKey(DIK_L))
	{
		isDebugCamera_ = false;
	}

	if (isDebugCamera_)
	{
		camera_.matView_ = debugCamera_.GetCamera().matView_;
		camera_.matProjection_ = debugCamera_.GetCamera().matProjection_;
		camera_.TransferMatrix();
	}
	else
	{
		camera_.UpdateMatrix();
	}
};

void GameLoseScene::Draw()
{
	Engine::PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	//Skydomeの描画
	skydome_->Draw(camera_);

	Model::PostDraw();

	ParticleModel::PreDraw();

	ParticleModel::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	//UIの描画
	gameLoseSceneUI_->Draw();

	Sprite::PostDraw();

	Engine::PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	//Transition用Spriteの描画
	transition_->Draw();

	Sprite::PostDraw();
};

void GameLoseScene::Finalize()
{

}

void GameLoseScene::ImGui()
{
	ImGui::Begin("LoseScene");
	ImGui::Text("Abutton or SpaceKey : TitleScene");
	ImGui::End();
}