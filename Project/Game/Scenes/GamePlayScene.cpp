#include "GamePlayScene.h"
#include "Engine/Utility/GlobalVariables.h"

GamePlayScene::GamePlayScene()
{

}

GamePlayScene::~GamePlayScene()
{
	audio_->xAudio2.Reset();
	audio_->SoundUnload(&audio_->soundDatas[0]);

	delete sprite_;
}

void GamePlayScene::Initialize()
{
	debugCamera_ = new DebugCamera();
	debugCamera_->Initialize(1280, 720);

	dxCore_ = DirectXCore::GetInstance();
	
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureManager_ = TextureManager::GetInstance();

	light_ = Light::GetInstance();
	light_->Initialize();

	game3dObjectManager_ = Game3dObjectManager::GetInstance();
	game3dObjectManager_->Initialize();

	white_ = textureManager_->LoadTexture("resource/white.png");
	uvChecker_ = textureManager_->LoadTexture("resource/uvChecker.png");
	monsterBall_ = textureManager_->LoadTexture("resource/monsterBall.png");

	texture_ = monsterBall_;

	sprite_ = new Sprite();
	sprite_->Initialize(LeftTop_[0], LeftBottom_[0], RightTop_[1], RightBottom_[1], texture_);
	worldTransformSprite_.Initialize();

	for (int i = 0; i < 2; i++ )
	{
		sphere_[i] = new Sphere();
		sphere_[i]->Initialize();
		worldTransform_[i].Initialize();
	}

	audio_->soundDatas[0] = audio_->SoundLoadWave("resource/mokugyo.wav");

	worldTransform_[0].translation.x = 5.0f;
	worldTransform_[1].translation.z = -10.0f;

	model_ = model_->CreateModelFromObj("resource", "plane.obj");

	camera_.Initialize();
	worldTransformModel_.Initialize();

	float s = 10;
	GlobalVariables* globalVariables{};
	globalVariables = GlobalVariables::GetInstance();

	const char* groupName = "Player";

	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "Test", s);
}

void GamePlayScene::Update()
{
	debugCamera_->Update();

	game3dObjectManager_->Update();

	sprite_->Update(LeftTop_[0], LeftBottom_[0], RightTop_[1], RightBottom_[1]);

#ifdef _DEBUG
	if (input_->PushKey(DIK_1))
	{
		debugCamera_->SwitchDebugCamera(true);
	}

	if (input_->PushKey(DIK_2))
	{
		debugCamera_->SwitchDebugCamera(false);
	}
#endif // _DEBUG

	if (input_->PushKey(DIK_SPACE))
	{
		count_ += 1;
	}

	if (input_->PushKey(DIK_RETURN))
	{
		audio_->Play(audio_->xAudio2.Get(), audio_->soundDatas[0]);
	}

	if (input_->PressKey(DIK_W))
	{
		worldTransformModel_.translation.y += 0.05f;
	}

	if (input_->PressKey(DIK_S))
	{
		worldTransformModel_.translation.y -= 0.05f;
	}

	if (input_->PressKey(DIK_A))
	{
		worldTransformModel_.translation.x -= 0.05f;
	}

	if (input_->PressKey(DIK_D))
	{
		worldTransformModel_.translation.x += 0.05f;
	}

	if (input_->PressKey(DIK_Q))
	{
		worldTransformModel_.translation.z += 0.05f;
	}

	if (input_->PressKey(DIK_E))
	{
		worldTransformModel_.translation.z -= 0.05f;
	}


	if (input_->PressKey(DIK_UP))
	{
		worldTransform_[0].rotation.x += 0.05f;
	}

	if (input_->PressKey(DIK_DOWN))
	{
		worldTransform_[0].rotation.x -= 0.05f;
	}

	if (input_->PressKey(DIK_LEFT))
	{
		worldTransform_[0].rotation.y += 0.05f;
	}

	if (input_->PressKey(DIK_RIGHT))
	{
		worldTransform_[0].rotation.y -= 0.05f;
	}

	camera_.UpdateMatrix();
	worldTransform_[0].UpdateMatrix();
	worldTransformModel_.UpdateMatrix();
}

void GamePlayScene::Draw()
{
	/*sphere_[0]->Draw(worldTransform_[0], viewProjection_, texture_);
	sphere_[1]->Draw(worldTransform_[1], viewProjection_, texture_);*/

	//sprite_->Draw(worldTransformSprite_);

	//game3dObjectManager_->Draw(camera_);

	model_->Draw(worldTransformModel_, camera_);

	ImGui::Begin("sphereTexture");
	ImGui::Checkbox("texture", &changeTexture_);
	ImGui::End();

	ImGui::Begin("count");
	ImGui::Text("count %d", count_);
	ImGui::End();

	ImGui::Begin("Camera");
	ImGui::SliderFloat3("rotation", &camera_.rotation.x, 1.0f, -1.0f);
	ImGui::SliderFloat3("transform", &camera_.translation.x, 10.0f, -10.0f);
	ImGui::End();

	engine_->ImGui();

	//sprite_->ImGui("Sprite");

	model_->ImGui("Model");

	light_->ImGui("Light");

	if (changeTexture_ == true)
	{
		texture_ = monsterBall_;

	}else {
		texture_ = uvChecker_;
	}
}