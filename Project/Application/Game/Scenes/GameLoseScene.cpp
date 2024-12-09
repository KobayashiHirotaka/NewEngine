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
	//TextureManagerのinstance
	textureManager_ = TextureManager::GetInstance();

	//ModelManagerのinstance
	modelManager_ = ModelManager::GetInstance();

	//Inputのinstance
	input_ = Input::GetInstance();

	//Audioのinstance
	audio_ = Audio::GetInstance();

	//Skydomeの生成、初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	//Skyboxの生成、初期化
	skybox_.reset(Skybox::Create());

	skyboxWorldTransform_.Initialize();
	skyboxWorldTransform_.scale = { 500.0f, 500.0f, 500.0f };

	//DebugCameraの初期化
	debugCamera_.Initialize();

	//リソース
	loseSceneTextureHandle_ = TextureManager::LoadTexture("resource/images/LoseScene.png");
	loseSceneSprite_.reset(Sprite::Create(loseSceneTextureHandle_, { 0.0f,0.0f }));

	transitionSprite_.reset(Sprite::Create(transitionTextureHandle_, { 0.0f,0.0f }));
	transitionSprite_->SetColor(transitionColor_);
	transitionSprite_->SetSize(Vector2{ 1280.0f,720.0f });

	selectSoundHandle_ = audio_->SoundLoadMP3("resource/Sounds/Select.mp3");
};

void GameLoseScene::Update()
{
#ifdef _DEBUG

	//デバッグ用のシーン切り替え
	if (input_->PushKey(DIK_SPACE))
	{
		isTransitionStart_ = true;
		audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
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
					audio_->SoundPlayMP3(selectSoundHandle_, false, 1.0f);
				}

				isPlayAudio_ = true;
			}
		}
	}

	skyboxWorldTransform_.UpdateMatrixEuler();

	//トランジション
	if (!isTransitionEnd_)
	{
		transitionTimer_ += 1.0f / kTransitionTime;
		transitionColor_.w = Lerp(transitionColor_.w, 0.0f, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w <= 0.0f)
		{
			isTransitionEnd_ = true;
			transitionTimer_ = 0.0f;
		}
	}

	if (isTransitionStart_)
	{
		transitionTimer_ += 1.0f / kTransitionTime;
		transitionColor_.w = Lerp(transitionColor_.w, 1.0f, transitionTimer_);
		transitionSprite_->SetColor(transitionColor_);

		if (transitionColor_.w >= 1.0f)
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

void GameLoseScene::Draw()
{
	PostProcess::GetInstance()->PreDraw();

	Skybox::PreDraw();

	//skybox_->Draw(skyboxWorldTransform_, camera_);

	Skybox::PostDraw();

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

	transitionSprite_->Draw();

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