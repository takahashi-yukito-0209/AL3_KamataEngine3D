#include "Math.h"
#include <algorithm>
#include <assert.h>
#include <cmath>

using namespace KamataEngine;

// Vector3加算
KamataEngine::Vector3 operator+(const KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv) { return {lhv.x + rhv.x, lhv.y + rhv.y, lhv.z + rhv.z}; }

// Vector3減算
KamataEngine::Vector3 operator-(const KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv) { return {lhv.x - rhv.x, lhv.y - rhv.y, lhv.z - rhv.z}; }

// Vector3スカラー乗算
KamataEngine::Vector3 operator*(const KamataEngine::Vector3& v, float s) { return {v.x * s, v.y * s, v.z * s}; }

// Vector3スカラー除算
KamataEngine::Vector3 operator/(const KamataEngine::Vector3& v, float s) { return {v.x / s, v.y / s, v.z / s}; }

// 加算代入
KamataEngine::Vector3& operator+=(KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv) {
	lhv = lhv + rhv;
	return lhv;
}

// 減算代入
KamataEngine::Vector3& operator-=(KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv) {
	lhv = lhv - rhv;
	return lhv;
}

// スカラー乗算代入
KamataEngine::Vector3& operator*=(KamataEngine::Vector3& v, float s) {
	v = v * s;
	return v;
}

// スカラー除算代入
KamataEngine::Vector3& operator/=(KamataEngine::Vector3& v, float s) {
	v = v / s;
	return v;
}

// 平行移動行列
KamataEngine::Matrix4x4 Math::MakeTranslateMatrix(const KamataEngine::Vector3& translate) {
	Matrix4x4 result = {};

	result = {
	    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, translate.x, translate.y, translate.z, 1.0f,
	};

	return result;
};

// 拡大縮小行列
KamataEngine::Matrix4x4 Math::MakeScaleMatrix(const KamataEngine::Vector3& scale) {
	Matrix4x4 result = {};

	result = {
	    scale.x, 0.0f, 0.0f, 0.0f, 0.0f, scale.y, 0.0f, 0.0f, 0.0f, 0.0f, scale.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	return result;
}

// X軸回転行列
KamataEngine::Matrix4x4 Math::MakeRotateXMatrix(float radian) {
	Matrix4x4 result = {};

	result = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, std::cos(radian), std::sin(radian), 0.0f, 0.0f, std::sin(-radian), std::cos(radian), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	return result;
}

// Y軸回転行列
KamataEngine::Matrix4x4 Math::MakeRotateYMatrix(float radian) {
	Matrix4x4 result = {};

	result = {std::cos(radian), 0.0f, std::sin(-radian), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, std::sin(radian), 0.0f, std::cos(radian), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	return result;
}

// Z軸回転行列
KamataEngine::Matrix4x4 Math::MakeRotateZMatrix(float radian) {
	Matrix4x4 result = {};

	result = {std::cos(radian), std::sin(radian), 0.0f, 0.0f, std::sin(-radian), std::cos(radian), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	return result;
}

// 行列の積
KamataEngine::Matrix4x4 Math::Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			for (int k = 0; k < 4; ++k) {
				result.m[row][col] += m1.m[row][k] * m2.m[k][col];
			}
		}
	}
	return result;
}

// 3次元アフィン変換行列
KamataEngine::Matrix4x4 Math::MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate) {
	
	// 回転軸をひとつにまとめる（合成）
	Matrix4x4 rotateXYZMatrix = Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));

	// 回転と拡縮、移動をすべて合成
	Matrix4x4 result = Multiply(Multiply(MakeScaleMatrix(scale), rotateXYZMatrix), MakeTranslateMatrix(translate));

	return result;
}

// ベクトル座標変換
KamataEngine::Vector3 Math::Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

// ワールド変換行列の更新
void Math::WorldTransformUpdate(KamataEngine::WorldTransform& worldTransform) {
	// スケール、回転、平行移動を合成して行列を計算する
	worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
	// 定数バッファに転送
	worldTransform.TransferMatrix();
}

// イージング関数（easeInOut）
float Math::easeInOut(float timer, float start, float end) {
	float t = timer * 2.0f;
	if (t < 1.0f) {
		return (end - start) / 2.0f * t * t + start;
	} else {
		t -= 1.0f;
		return (end - start) / -2.0f * (t * (t - 2.0f) - 1.0f) + start;
	}
}

// 線形補間（Lerp）
KamataEngine::Vector3 Math::Lerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t) { return v1 + (v2 - v1) * t; }

// AABB衝突判定
bool Math::IsCollision(const AABB& aabb1, const AABB& aabb2) {
	return (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && // x軸
	       (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && // y軸
	       (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z);   // z軸
}

// ビュー行列
KamataEngine::Matrix4x4 Math::MakeViewMatrix(const KamataEngine::Vector3& eye, const KamataEngine::Vector3& target, const KamataEngine::Vector3& up) {
	using namespace KamataEngine;
	Vector3 zAxis = Normalize(target - eye);
	Vector3 xAxis = Normalize(Cross(up, zAxis));
	Vector3 yAxis = Cross(zAxis, xAxis);

	Matrix4x4 view = {};
	view = {xAxis.x, xAxis.y, xAxis.z, -Dot(xAxis, eye), yAxis.x, yAxis.y, yAxis.z, -Dot(yAxis, eye), zAxis.x, zAxis.y, zAxis.z, -Dot(zAxis, eye), 0.0f, 0.0f, 0.0f, 1.0f};

	return view;
}

// 射影行列
KamataEngine::Matrix4x4 Math::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result = {};
	float f = 1.0f / tanf(fovY / 2.0f);
	result.m[0][0] = f / aspectRatio;
	result.m[1][1] = f;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	return result;
}

// 内積
float Math::Dot(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

// 外積
KamataEngine::Vector3 Math::Cross(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2) { return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x}; }

// 正規化
KamataEngine::Vector3 Math::Normalize(const KamataEngine::Vector3& v) {
	float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	assert(length != 0.0f);
	return v / length;
}
