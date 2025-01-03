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
	textureManager_ = TextureManager::GetInstance();

	//ModelManagerのインスタンスの取得
	modelManager_ = ModelManager::GetInstance();

	//Inputのインスタンスの取得
	input_ = Input::GetInstance();

	//Audioのインスタンスの取得
	audio_ = Audio::GetInstance();

	//PostEffectの切り替え
	PostProcess::GetInstance()->SetIsGrayScaleActive(false);
	PostProcess::GetInstance()->SetIsVignetteActive(false);

	//Skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	//DebugCameraの初期化
	debugCamera_.Initialize();

	//リソース
	loseSceneTextureHandle_ = TextureManager::LoadTexture("resource/images/LoseScene.png");
	loseSceneSprite_.reset(Sprite::Create(loseSceneTextureHandle_, { 0.0f,0.0f }));

	//Transition生成、初期化
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

	//サウンド
	selectSoundHandle_ = audio_->LoadSoundMP3("resource/Sounds/Select.mp3");
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
	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	//Skydomeの描画
	skydome_->Draw(camera_);

	Model::PostDraw();

	ParticleModel::PreDraw();

	ParticleModel::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	//Loseの表示
	loseSceneSprite_->Draw();

	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

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