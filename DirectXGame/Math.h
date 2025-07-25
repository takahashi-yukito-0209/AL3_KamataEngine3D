#pragma once
#include "KamataEngine.h"
#include <algorithm>
#include <cassert>
#include <cmath>

// Vector3型の基本演算子（加算・減算・スカラー演算）
KamataEngine::Vector3 operator+(const KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3 operator-(const KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3 operator*(const KamataEngine::Vector3& v, float s);
KamataEngine::Vector3 operator/(const KamataEngine::Vector3& v, float s);

// Vector3型の基本代入演算子（+=、-=、*=、/=）
KamataEngine::Vector3& operator+=(KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3& operator-=(KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3& operator*=(KamataEngine::Vector3& v, float s);
KamataEngine::Vector3& operator/=(KamataEngine::Vector3& v, float s);

struct AABB {
	KamataEngine::Vector3 min; // 最小点
	KamataEngine::Vector3 max; // 最大点
};

class Math {
public:
	// 平行移動行列
	static KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate);
	
	// 拡大縮小行列
	static KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale);
	
	// X軸回転行列
	static KamataEngine::Matrix4x4 MakeRotateXMatrix(float radian);
	
	// Y軸回転行列
	static KamataEngine::Matrix4x4 MakeRotateYMatrix(float radian);
	
	// Z軸回転行列
	static KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian);

	// 行列の積
	static KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);
	
	// アフィン変換行列
	static KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);

	// 座標変換
	static KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix);

	// ワールド行列更新
	static void WorldTransformUpdate(KamataEngine::WorldTransform& worldTransform);

	// イージング関数（easeInOut）
	static float easeInOut(float timer, float start, float end);
	
	// 線形補間（Lerp）
	static KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t);

	// AABB同士の交差判定（trueなら衝突）
	static bool IsCollision(const AABB& aabb1, const AABB& aabb2);

	// ビュー行列
	static KamataEngine::Matrix4x4 MakeViewMatrix(const KamataEngine::Vector3& eye, const KamataEngine::Vector3& target, const KamataEngine::Vector3& up);
	
	// 射影行列
	static KamataEngine::Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

	// ベクトルの内積
	static float Dot(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);
	
	// ベクトルの外積
	static KamataEngine::Vector3 Cross(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);
	
	// ベクトルの正規化
	static KamataEngine::Vector3 Normalize(const KamataEngine::Vector3& v);
};

struct ViewProjection {
	KamataEngine::Vector3 eye;    // 視点
	KamataEngine::Vector3 target; // 注視点
	KamataEngine::Vector3 up;     // 上方向

	KamataEngine::Matrix4x4 viewMatrix;       // ビュー行列
	KamataEngine::Matrix4x4 projectionMatrix; // プロジェクション行列

	void Initialize(float fovY, float aspect, float nearClip, float farClip) {
		// カメラのデフォルト設定
		eye = {0.0f, 0.0f, -5.0f};
		target = {0.0f, 0.0f, 0.0f};
		up = {0.0f, 1.0f, 0.0f};

		// ビュー行列の生成
		viewMatrix = Math::MakeViewMatrix(eye, target, up);
		// 射影行列の生成
		projectionMatrix = Math::MakePerspectiveFovMatrix(fovY, aspect, nearClip, farClip);
	}
};
