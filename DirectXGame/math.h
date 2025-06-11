#pragma once
#include "KamataEngine.h"

// 算術演算子オーバーロード（非代入系）
KamataEngine::Vector3 operator+(const KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3 operator-(const KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3 operator*(const KamataEngine::Vector3& v, float s);
KamataEngine::Vector3 operator/(const KamataEngine::Vector3& v, float s);

// 代入演算子オーバーロード
KamataEngine::Vector3& operator+=(KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3& operator-=(KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3& operator*=(KamataEngine::Vector3& v, float s);
KamataEngine::Vector3& operator/=(KamataEngine::Vector3& v, float s);

struct AABB {
	KamataEngine::Vector3 min;
	KamataEngine::Vector3 max;
};

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

	// ワールド行列更新関数
	void WorldTransformUpdate(KamataEngine::WorldTransform& worldTransform);

	// イージング(easeInOut)
	float easeInOut(float timer, float start, float end);

	KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t);

	bool IsCollision(const AABB& aabb1, const AABB& aabb2);
};