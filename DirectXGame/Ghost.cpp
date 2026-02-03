#include "Ghost.h"
#include <cassert>
#include <algorithm>

using namespace KamataEngine;

void Ghost::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const std::vector<Frame>& frames) {
    assert(model);
    model_ = model;
    camera_ = camera;
    // 入力ワールドトランスフォーム配列をコピー
    frames_.clear();
    frames_.reserve(frames.size());
    for (const auto& f : frames) {
        frames_.push_back(f);
    }
    worldTransform_.Initialize();
    if (!frames_.empty()) {
        worldTransform_.translation_ = frames_[0].position;
        worldTransform_.rotation_.y = frames_[0].rotY;
    }
    frameIndex_ = 0;
    finished_ = frames_.empty();
    // 再生長さを目標秒数に合わせる（録画フレーム数に基づき自動計算）
    if (!frames_.empty()) {
        const float desiredSeconds = 3.5f; // 3～4秒の範囲（ここを変えれば調整可能）
        playbackRate_ = static_cast<float>(frames_.size()) / (60.0f * desiredSeconds);
    } else {
        playbackRate_ = 1.0f;
    }
    playbackElapsed_ = 0.0f;
    // 初期カラー(少し暗めでやや半透明)
    objectColor_.Initialize();
    objectColor_.SetColor(KamataEngine::Vector4{0.4f, 0.4f, 0.4f, 0.5f});

    // ワールド行列を初期転送しておく（生成直後に描画される可能性があるため）
    math_.WorldTransformUpdate(worldTransform_);
}

void Ghost::Update() {
    if (finished_) return;

    // 時間ベース再生: playbackRate_ は録画時と同じ速度が 1.0f
    // 1 録画フレームの秒長は録画時のフレーム時間(1/60)と仮定
    const float kRecordedFrameSeconds = 1.0f / 60.0f;
    playbackElapsed_ += playbackRate_ * kRecordedFrameSeconds;
    // 進むべきフレーム数を計算
    size_t targetIndex = frameIndex_;
    while (!finished_ && playbackElapsed_ >= kRecordedFrameSeconds) {
        playbackElapsed_ -= kRecordedFrameSeconds;
        ++targetIndex;
        if (targetIndex >= frames_.size()) {
            finished_ = true;
            return;
        }
    }

    if (targetIndex != frameIndex_) {
        frameIndex_ = targetIndex;
    }

    // 滑らかな補間（線形）: 現在のフレームと次のフレームの間を補間する
    const size_t nextIndex = (frameIndex_ + 1 < frames_.size()) ? frameIndex_ + 1 : frameIndex_;
    if (nextIndex == frameIndex_) {
        // 最終フレーム
        worldTransform_.translation_ = frames_[frameIndex_].position;
        worldTransform_.rotation_.y = frames_[frameIndex_].rotY;
    } else {
        float t = playbackElapsed_ / kRecordedFrameSeconds;
        t = std::clamp(t, 0.0f, 1.0f);

        // 位置は Catmull-Rom スプラインで補間して滑らかにする
        // P0,P1,P2,P3 を用意（端はクランプ）
        size_t i0 = (frameIndex_ == 0) ? 0 : frameIndex_ - 1;
        size_t i1 = frameIndex_;
        size_t i2 = nextIndex;
        size_t i3 = (nextIndex + 1 < frames_.size()) ? nextIndex + 1 : nextIndex;

        KamataEngine::Vector3 P0 = frames_[i0].position;
        KamataEngine::Vector3 P1 = frames_[i1].position;
        KamataEngine::Vector3 P2 = frames_[i2].position;
        KamataEngine::Vector3 P3 = frames_[i3].position;

        float tt = t * t;
        float ttt = tt * t;

        KamataEngine::Vector3 pos = (P1 * 2.0f) + (P2 - P0) * t + (P0 * 2.0f - P1 * 5.0f + P2 * 4.0f - P3) * tt + ((P0 * -1.0f) + P1 * 3.0f - P2 * 3.0f + P3) * ttt;
        pos *= 0.5f;
        worldTransform_.translation_ = pos;

        // 回転はクォータニオンで補間（最短経路）
        float a0 = frames_[frameIndex_].rotY;
        float a1 = frames_[nextIndex].rotY;
        // use ease for interpolation parameter
        float te = math_.easeInOut(t, 0.0f, 1.0f);
        Math::Quaternion q0 = math_.FromYaw(a0);
        Math::Quaternion q1 = math_.FromYaw(a1);
        Math::Quaternion q = math_.Slerp(q0, q1, te);
        worldTransform_.rotation_.y = math_.ToYaw(q);
    }

    // ワールド行列更新
    math_.WorldTransformUpdate(worldTransform_);
}

void Ghost::Draw() {
    if (!model_ || !camera_ || finished_) return;
    model_->Draw(worldTransform_, *camera_, &objectColor_);
}

void Ghost::SetFacingY(float ry) {
    worldTransform_.rotation_.y = ry;
    math_.WorldTransformUpdate(worldTransform_);
}

void Ghost::Reset() {
    frames_.clear();
    frameIndex_ = 0;
    playbackElapsed_ = 0.0f;
    finished_ = true;
}

KamataEngine::Vector3 Ghost::GetWorldPosition() const {
    Vector3 p;
    p.x = worldTransform_.matWorld_.m[3][0];
    p.y = worldTransform_.matWorld_.m[3][1];
    p.z = worldTransform_.matWorld_.m[3][2];
    return p;
}

AABB Ghost::GetAABB() const {
    Vector3 wp = GetWorldPosition();
    AABB aabb;
    aabb.min = {wp.x - kWidth / 2.0f, wp.y - kHeight / 2.0f, wp.z - kWidth / 2.0f};
    aabb.max = {wp.x + kWidth / 2.0f, wp.y + kHeight / 2.0f, wp.z + kWidth / 2.0f};
    return aabb;
}
