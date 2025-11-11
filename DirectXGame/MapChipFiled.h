#pragma once
#include "KamataEngine.h"

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

// マップチップ番号
struct IndexSet {
	uint32_t xIndex;
	uint32_t yIndex;
};

//範囲矩形
struct Rect {
	float left;//左端
	float right;//右端
	float bottom;//下端
	float top;//上端
};

/// <summary>
/// マップチップフィールド
/// </summary>
class MapChipField {
private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	// マップチップデータ
	MapChipData mapChipData_;

public:

	// 読み込み済みデータリセット関数
	void ResetMapChipData();

	// CSVファイル読み込み関数
	void LoadMapChipCsv(const std::string& filePath);

	// マップチップ種別の取得関数
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	// マップチップ座標取得関数
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex,uint32_t yIndex);

	//ブロックのアクセッサー
	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }

	//座標からマップチップ番号を計算
	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);

	//ブロックの範囲取得関数
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

};
