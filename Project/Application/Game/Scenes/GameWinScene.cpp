/**
 * @file GameWinScene.cpp
 * @brief 勝利シーンの初期化、更新、描画などを行う
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#include "GameWinScene.h"
#include "Engine/Framework/SceneManager.h"
#include "Engine/Components/PostProcess/PostProcess.h"
#include <cassert>

GameWinScene::GameWinScene() {};

GameWinScene::~GameWinScene() {};

void GameWinScene::Initialize()
{
	//TextureManagerのインスタンスの取得
	textureManager_ = TextureManager::GetInstance();

	//ModelManagerのインスタンスの取得
	modelManager_ = ModelManager::GetInstance();

	//Inputのインスタンスの取得
	input_ = Input::GetInstance();

	//Audioのインスタンスの取得
	audio_ = Audio::GetInstance();

	//Skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	//DebugCameraの初期化
	debugCamera_.Initialize();

	//リソース
	winSceneTextureHandle_ = TextureManager::LoadTexture("resource/images/WinScene.png");
	winSceneSprite_.reset(Sprite::Create(winSceneTextureHandle_, { 0.0f,0.0f }));

	//トランジション
	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(transitionTextureSize_);

	//サウンド
	selectSoundHandle_ = audio_->LoadSoundMP3("resource/Sounds/Select.mp3");
};

void GameWinScene::Update()
{
#ifdef _DEBUG

	//デバッグ用のシーン切り替え
	if (input_->PushKey(DIK_SPACE))
	{
		isTransitionStart_ = true;
		audio_->PlaySoundMP3(selectSoundHandle_, false, 1.0f);
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
					audio_->PlaySoundMP3(selectSoundHandle_, false, 1.0f);
				}

				isPlayAudio_ = true;
			}
		}
	}

	//トランジション
	const float deltaTime = 1.0f / kTransitionTime;

	if (!isTransitionEnd_)
	{
		transitionTimer_ += deltaTime;
		transitionColor_.w = Lerp(transitionColor_.w, kTransitionEndAlpha_, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w <= kTransitionEndAlpha_)
		{
			isTransitionEnd_ = true;
			transitionTimer_ = 0.0f;
		}
	}

	if (isTransitionStart_)
	{
		transitionTimer_ += deltaTime;
		transitionColor_.w = Lerp(transitionColor_.w, kTransitionStartAlpha_, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w >= kTransitionStartAlpha_)
		{
			sceneManager_->ChangeScene("GameTitleScene");
		}
	}

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

void GameWinScene::Draw()
{
	PostProcess::GetInstance()->PreDraw();

	Model::PreDraw();

	//Skydomeの描画
	skydome_->Draw(camera_);

	Model::PostDraw();

	ParticleModel::PreDraw();

	ParticleModel::PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	//Winの表示
	winSceneSprite_->Draw();

	Sprite::PostDraw();

	PostProcess::GetInstance()->PostDraw();

	Sprite::PreDraw(Sprite::kBlendModeNormal);

	//トランジション
	transitionSprite_->Draw();

	Sprite::PostDraw();
};

void GameWinScene::Finalize()
{

}

void GameWinScene::ImGui()
{
	ImGui::Begin("WinScene");
	ImGui::Text("Abutton or SpaceKey : TitleScene");
	ImGui::End();
}
