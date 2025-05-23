#pragma once
#include "KamataEngine.h"
class Math {
public:
	// 1.平行移動行列
	KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate);

	// 2.拡大縮小行列
	KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale);

	// 3.座標変換
	KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix);

	// 3.行列の積
	KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);

	// 1.X軸回転行列
	KamataEngine::Matrix4x4 MakeRotateXMatrix(float radian);

	// 2.Y軸回転行列
	KamataEngine::Matrix4x4 MakeRotateYMatrix(float radian);

	// 3.Z軸回転行列
	KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian);

	// 3次元アフィン変換行列
	KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);

	//ワールド行列更新関数
	void WorldTransformUpdate(KamataEngine::WorldTransform& worldTransform);

	// イージング(easeInOut)
	float easeInOut(float timer, float start, float end);
};
