/**
 * @file Types.h
 * @brief ライティングタイプとモデルタイプの列挙型
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once

//ライティングタイプ
enum class LightingType
{
	LambertianReflectance,
	HalfLambert,
};

//モデルタイプ
enum class ModelType
{
	PhongReflectionModel,
	BlinnPhongReflectionModel,
};