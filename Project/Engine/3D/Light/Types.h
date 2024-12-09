/**
 * @file Types.h
 * @brief ライティングタイプとモデルタイプの列挙型
 * @author  KOBAYASHI HIROTAKA
 * @date 未記録
 */

#pragma once

enum class LightingType
{
	LambertianReflectance,
	HalfLambert,
};

enum class ModelType
{
	PhongReflectionModel,
	BlinnPhongReflectionModel,
};