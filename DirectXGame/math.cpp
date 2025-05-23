#include "math.h"
#include <assert.h>
#include <cmath>
#include <algorithm>

using namespace KamataEngine;

// 1.平行移動行列
KamataEngine::Matrix4x4 Math::MakeTranslateMatrix(const KamataEngine::Vector3& translate) {
	Matrix4x4 result = {};

	result = {
	    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, translate.x, translate.y, translate.z, 1.0f,
	};

	return result;
};

// 2.拡大縮小行列
KamataEngine::Matrix4x4 Math::MakeScaleMatrix(const KamataEngine::Vector3& scale) {
	Matrix4x4 result = {};

	result = {
	    scale.x, 0.0f, 0.0f, 0.0f, 0.0f, scale.y, 0.0f, 0.0f, 0.0f, 0.0f, scale.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	return result;
};

// 3.座標変換
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
};

// 3.行列の積
KamataEngine::Matrix4x4 Math::Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2) {
	Matrix4x4 result = {};

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {

			result.m[row][column] = 0;

			for (int k = 0; k < 4; k++) {
				result.m[row][column] += m1.m[row][k] * m2.m[k][column];
			}
		}
	}

	return result;
}

// 1.X軸回転行列
KamataEngine::Matrix4x4 Math::MakeRotateXMatrix(float radian) {
	Matrix4x4 result = {};

	result = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, std::cos(radian), std::sin(radian), 0.0f, 0.0f, std::sin(-radian), std::cos(radian), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	return result;
};

// 2.Y軸回転行列
KamataEngine::Matrix4x4 Math::MakeRotateYMatrix(float radian) {
	Matrix4x4 result = {};

	result = {std::cos(radian), 0.0f, std::sin(-radian), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, std::sin(radian), 0.0f, std::cos(radian), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	return result;
};

// 3.Z軸回転行列
KamataEngine::Matrix4x4 Math::MakeRotateZMatrix(float radian) {
	Matrix4x4 result = {};

	result = {std::cos(radian), std::sin(radian), 0.0f, 0.0f, std::sin(-radian), std::cos(radian), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	return result;
};

// 3次元アフィン変換行列
KamataEngine::Matrix4x4 Math::MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate) {

	// 回転軸をひとつにまとめる（合成）
	Matrix4x4 rotateXYZMatrix = Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));

	// 回転と拡縮、移動をすべて合成
	Matrix4x4 result = Multiply(Multiply(MakeScaleMatrix(scale), rotateXYZMatrix), MakeTranslateMatrix(translate));

	return result;
}

// ワールド行列更新関数
void Math::WorldTransformUpdate(KamataEngine::WorldTransform& worldTransform) {
	// スケール、回転、平行移動を合成して行列を計算する
	worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
	// 定数バッファに転送
	worldTransform.TransferMatrix();
}

// イージング(easeInOut)
float Math::easeInOut(float timer, float start, float end) {

	// t を時間の進行として設定し、最大値を1.0fに制限
	timer = std::clamp(timer, 0.0f, 1.0f); 

	// イーズイン・イーズアウト補間を計算
	float easedT = timer * timer * (3.0f - 2.0f * timer);

	// 補間された位置を計算
	return (1.0f - easedT) * start + easedT * end;
}


