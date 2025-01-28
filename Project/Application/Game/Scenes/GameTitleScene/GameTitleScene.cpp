/**
 * @file GameTitleScene.cpp
 * @brief タイトルシーンの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "GameTitleScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameTitleScene::GameTitleScene() {};

GameTitleScene::~GameTitleScene() {};

void GameTitleScene::Initialize()
{
	//TextureManagerのインスタンスの取得
	textureManager_ = Engine::TextureManager::GetInstance();

	//ModelManagerのインスタンスの取得
	modelManager_ = ModelManager::GetInstance();

	//Inputのインスタンスの取得
	input_ = Engine::Input::GetInstance();

	//Audioのインスタンスの取得
	audio_ = Engine::Audio::GetInstance();

	//PostProcessのインスタンスの取得
	Engine::PostProcess::GetInstance()->SetIsPostProcessActive(true);

	//PostEffectの切り替え
	Engine::PostProcess::GetInstance()->SetIsBloomActive(true);
	Engine::PostProcess::GetInstance()->SetIsGaussianFilterActive(true);
	Engine::PostProcess::GetInstance()->SetIsLuminanceBasedOutlineActive(true);

	//DebugCameraの初期化
	debugCamera_.Initialize();

	//Skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	//UI生成、初期化
	gameTitleSceneUI_ = std::make_unique<GameTitleSceneUI>();
	gameTitleSceneUI_->Initialize();

	//操作説明の生成、初期化
	guide_ = std::make_unique<Guide>();
	guide_->Initialize();

	//BGM,SEの読み込み
	titleSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/BGM.mp3");
	selectSoundHandle_ = audio_->LoadSoundMP3("Resource/Sounds/Select.mp3");

	//BGMの再生,停止
	if (!audio_->IsAudioPlaying(titleSoundHandle_))
	{
		const float bgmVolume = 0.2f;
		audio_->PlaySoundMP3(titleSoundHandle_, true, bgmVolume);
	}

	//Transition生成、初期化
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();
};

void GameTitleScene::Update()
{
#ifdef _ADJUSTMENT

	//デバッグ用のシーン切り替え
	if (input_->PushKey(DIK_SPACE))
	{
		isTransitionStart_ = true;
		audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
	}

#endif 

	//UIの更新
	gameTitleSceneUI_->Update();

	//操作説明の更新
	guide_->Update();

	//Skydomeの更新
	skydome_->Update();

	if (guide_->GetIsChangedSprite())
	{
		audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
		guide_->SetIsChangedSprite(false);
	}

	//シーン切り替え
	if (input_->GetJoystickState() && !guide_->GetIsOpen())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A) && isTransitionEnd_)
		{
			isTransitionStart_ = true;

			if (!isPlayAudio_)
			{
				audio_->PlaySoundMP3(selectSoundHandle_, false, volume_);
			}

			isPlayAudio_ = true;
		}
	}

	//Transition終了処理
	transition_->EndSceneTransition(isTransitionEnd_);

	//Transition開始処理
	transition_->StartSceneTransition(isTransitionStart_, sceneManager_, "GamePlayScene");

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

void GameTitleScene::Draw()
{
	Model::PreDraw();

	Model::PostDraw();

	Engine::PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	//Skydomeの描画
	skydome_->Draw(camera_);

	Model::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	//タイトルのUI描画
	if (!guide_->GetIsOpen())
	{
		gameTitleSceneUI_->Draw();
	}
	else
	{
		guide_->Draw();
	}

	Sprite::PostDraw();

	ParticleModel::PreDraw();

	ParticleModel::PostDraw();

	Engine::PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	//Transition用Spriteの描画
	transition_->Draw();

	Sprite::PostDraw();
};

void GameTitleScene::Finalize()
{

}

void GameTitleScene::ImGui()
{
	ImGui::Begin("TitleScene");
	ImGui::End();
}
