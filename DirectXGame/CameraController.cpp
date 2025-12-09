#include "CameraController.h"
#include "Player.h"
#include <algorithm>

using namespace KamataEngine;

void CameraController::Initialize(KamataEngine::Camera* camera) {

	camera_ = camera;
}

void CameraController::Update() {

	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	const Vector3& targetVelocity = target_->GetVelocity();


    // 追従対象とオフセットからカメラの目標座標を計算
    destination_ = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;

    // スプリングで自然な追従を行う
    // 位置誤差
    Vector3 error = destination_ - camera_->translation_;

    // 2次系: x'' + 2*zeta*omega*x' + omega^2 * x = 0  ->  accel = omega^2 * error - 2*zeta*omega*vel
    Vector3 accel = error * (kSpringOmega * kSpringOmega) - cameraVelocity_ * (2.0f * kSpringDamping * kSpringOmega);

    // 半陽的オイラーで速度・位置を更新
    cameraVelocity_ += accel * (1.0f / 60.0f);
    camera_->translation_ += cameraVelocity_ * (1.0f / 60.0f);

    // 移動範囲制限: まずはターゲットマージンに基づく制限
    camera_->translation_.x = max(camera_->translation_.x, destination_.x + targetMargin.left);
    camera_->translation_.x = min(camera_->translation_.x, destination_.x + targetMargin.right);
    camera_->translation_.y = max(camera_->translation_.y, destination_.y + targetMargin.bottom);
    camera_->translation_.y = min(camera_->translation_.y, destination_.y + targetMargin.top);

    // ワールドの移動範囲制限
    camera_->translation_.x = max(camera_->translation_.x, movableArea_.left);
    camera_->translation_.x = min(camera_->translation_.x, movableArea_.right);
    camera_->translation_.y = min(camera_->translation_.y, movableArea_.bottom);
    camera_->translation_.y = max(camera_->translation_.y, movableArea_.top);
	
	//行列を更新
	camera_->UpdateMatrix();
}

void CameraController::Reset() {

	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 追従対象とオフセットからカメラの座標を計算
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
}
