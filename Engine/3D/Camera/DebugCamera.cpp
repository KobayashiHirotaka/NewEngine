//#include "DebugCamera.h"
//
//DebugCamera::DebugCamera()
//{
//
//}
//
//DebugCamera::~DebugCamera()
//{
//
//};
//
//void DebugCamera::Initialize(int32_t kClientWidth, int32_t kClientHeight)
//{
//	kClientWidth_ = kClientWidth;
//	kClientHeight_ = kClientHeight;
//
//
//	input_ = Input::GetInstance();
//
//	worldTransform_.translation = { 0.0f,0.0f,-50.0f };
//}
//
//void DebugCamera::Update()
//{
//#ifdef _DEBUG
//	if (DebucCameraFlag)
//	{
//		DebugCameraMove();
//	}
//#endif // _DEBUG
//
//	if (target_) {
//		//追従対象からカメラまでのオフセット(0度の時の値)
//		Vector3 offset = offset_;
//		//オフセットをカメラの回転に合わせて回転させる
//		offset = TransformNormal(offset, matRot_);
//		//座標をコピーしてオフセット分ずらす
//		worldTransform_.translation = Add(target_->translation, offset);
//	}
//	//追従対象からカメラまでのオフセット(0度の時の値)
//	Vector3 offset = offset_;
//	//オフセットをカメラの回転に合わせて回転させる
//	offset = TransformNormal(offset, matRot_);
//	//座標をコピーしてオフセット分ずらす
//	worldTransform_.translation = offset;
//
//	//座標から平行移動行列を計算する
//	Matrix4x4 translateMatrix = MakeTranslateMatrix(worldTransform_.translation);
//	//ワールド行列を計算
//	worldTransform_.matWorld = Multiply(matRot_, translateMatrix);
//	//ビュー行列の計算
//	camera_.matView = Multiply(Inverse(translateMatrix), Inverse(matRot_));
//	//プロジェクション行列の計算
//	camera_.matProjection = MakePerspectiveFovMatrix(camera_.fovAngleY, camera_.aspectRatio, camera_.nearZ, camera_.farZ);
//}
//
//
//#ifdef _DEBUG
//void DebugCamera::DebugCameraMove()
//{
//	//左右移動
//	if (input_->PushKey(DIK_D)) {
//		//速さ
//		const float speed = -0.2f;
//		//カメラの移動ベクトル
//		Vector3 move = { speed,0.0f,0.0f };
//		//移動ベクトル分だけ座標を加算する
//		offset_ = Add(offset_, move);
//	}
//	else if (input_->PushKey(DIK_A)) {
//		//速さ
//		const float speed = 0.2f;
//		//カメラの移動ベクトル
//		Vector3 move = { speed,0.0f,0.0f };
//		//移動ベクトル分だけ座標を加算する
//		offset_ = Add(offset_, move);
//	}
//
//	//上下移動
//	if (input_->PushKey(DIK_W)) {
//		//速さ
//		const float speed = -0.2f;
//		//カメラの移動ベクトル
//		Vector3 move = { 0.0f,speed,0.0f };
//		//移動ベクトル分だけ座標を加算する
//		offset_ = Add(offset_, move);
//	}
//	else if (input_->PushKey(DIK_S)) {
//		//速さ
//		const float speed = 0.2f;
//		//カメラの移動ベクトル
//		Vector3 move = { 0.0f,speed,0.0f };
//		//移動ベクトル分だけ座標を加算する
//		offset_ = Add(offset_, move);
//	}
//
//	//X軸回転
//	if (input_->PushKey(DIK_UP)) {
//		//速さ
//		const float speed = 0.02f;
//		//追加回転分の回転行列を生成
//		Matrix4x4 matRotDelta = MakeIdentity4x4();
//		matRotDelta = Multiply(matRotDelta, MakeRotateXMatrix(speed));
//		//累積の回転行列を合成
//		matRot_ = Multiply(matRotDelta, matRot_);
//	}
//	else if (input_->PushKey(DIK_DOWN)) {
//		//速さ
//		const float speed = -0.02f;
//		//追加回転分の回転行列を生成
//		Matrix4x4 matRotDelta = MakeIdentity4x4();
//		matRotDelta = Multiply(matRotDelta, MakeRotateXMatrix(speed));
//		//累積の回転行列を合成
//		matRot_ = Multiply(matRotDelta, matRot_);
//	}
//
//	//Y軸回転
//	if (input_->PushKey(DIK_LEFT)) {
//		//速さ
//		const float speed = 0.02f;
//		//追加回転分の回転行列を生成
//		Matrix4x4 matRotDelta = MakeIdentity4x4();
//		matRotDelta = Multiply(matRotDelta, MakeRotateYMatrix(speed));
//		//累積の回転行列を合成
//		matRot_ = Multiply(matRotDelta, matRot_);
//	}
//	else if (input_->PushKey(DIK_RIGHT)) {
//		//速さ
//		const float speed = -0.02f;
//		//追加回転分の回転行列を生成
//		Matrix4x4 matRotDelta = MakeIdentity4x4();
//		matRotDelta = Multiply(matRotDelta, MakeRotateYMatrix(speed));
//		//累積の回転行列を合成
//		matRot_ = Multiply(matRotDelta, matRot_);
//	}
//}
//#endif // DEBUG