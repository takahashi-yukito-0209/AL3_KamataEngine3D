#pragma once
#include "KamataEngine.h"
#include "math.h"
#include <vector>

// 単純なゴースト（位置を記録して再生する）
class Ghost {
public:
    struct Frame {
        KamataEngine::Vector3 position;
        float rotY = 0.0f;
    };
    // 初期化（フレーム配列を受け取る）
    void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const std::vector<Frame>& frames);

    // 更新
    void Update();

    // 描画
    void Draw();

    // 表示向きを直接設定（Y軸回転）
    void SetFacingY(float ry);

    // ワールド座標取得
    KamataEngine::Vector3 GetWorldPosition() const;

    // AABB取得
    AABB GetAABB() const;

    // 再生終了フラグ
    bool IsFinished() const { return finished_; }

    // リセットして再利用可能にする
    void Reset();

private:
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Model* model_ = nullptr;
    KamataEngine::Camera* camera_ = nullptr;

    std::vector<Frame> frames_;
    size_t frameIndex_ = 0;
    // 再生速度係数 (1.0 = 録画時と同じ速度)
    float playbackRate_ = 1.0f;
    // 再生経過時間(秒)
    float playbackElapsed_ = 0.0f;
    bool finished_ = false;

    // 当たり判定サイズ（Player/Enemy と合わせる）
    static inline const float kWidth = 0.8f;
    static inline const float kHeight = 0.8f;

    Math math_;
    // 表示色・透明度
    KamataEngine::ObjectColor objectColor_;
};
