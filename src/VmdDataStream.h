#ifndef MMD_VMD_DATA_STREAM_H
#define MMD_VMD_DATA_STREAM_H

#include "BoneStream.h"
#include "VertexStream.h"

namespace mmd
{
	/*! @brief VMDデータストリームクラス
	* ファイルもしくはコンストラクタ引数からVMDデータリストを取得。
	* そして、離散フレームのボーン群を算出する。
	*/
	class VmdDataStream
	{
	public:
		/*! @brief ボーン位置姿勢情報
		*/
		struct BoneInfo {
			string boneName; // ボーン名
			int boneIndex; // ボーンインデックス
			Eigen::Vector3f shift; // シフト量
			Eigen::Quaternionf quarternion; // クォータニオン
		};

		/*! @brief コンストラクタ
		*/
		VmdDataStream();

		/*! @brief デストラクタ
		*/
		~VmdDataStream();
		
		/*! @brief ボーン情報の挿入
		* @param[in] framaNo フレーム番号
		* @param[in] boneInfo ボーン情報
		*/
		void insertBoneInfoList(const int frameNo, const BoneInfo& boneInfo);

		/*! @brief ボーン情報リストの取得
		* @return ボーン情報リスト
		*/
		const multimap<int, BoneInfo>& getBoneInfoList() const;

		/*! @brief 離散フレームのボーン・頂点ストリームを算出
		* @param[out] boneStream ボーンストリーム
		* @param[out] vertexStream 頂点ストリーム
		* @param[in]  initialBones 初期ボーン群
		* @param[in]  initialVertices 初期頂点群
		*/
		bool calcStream(BoneStream& boneStream, VertexStream& vertexStream,
			const vector<Bone>& initialBones, const vector<Vertex>& initialVertices);

	private:
		/*! @brief コピーコンストラクタの禁止
		*/
		VmdDataStream(const VmdDataStream&);

		/*! @brief 代入演算子の禁止
		*/
		VmdDataStream& operator=(const VmdDataStream&);

		/*! @brief ボーン情報リストの分割
		* @param[out] boneInfoListVec ボーン情報リストベクタ
		*/
		void splitBoneInfoList(vector<pair<int, vector<BoneInfo>>>& boneInfoListVec);

		/*! @brief 離散フレームのボーンストリームを算出
		* @param[out] boneStream ボーンストリーム
		* @param[in]  initialBones 初期ボーン群
		* @param[in]  boneInfoList ボーン情報リスト
		* @param[in]  frameNo フレーム番号
		* @param[in]  parentIndex 親インデックス
		*/
		void calcBoneStream(BoneStream& boneStream, vector<Bone>& bones,
			const vector<BoneInfo>& boneInfoList, const int frameNo, const int parentIndex);

		/*! @brief 離散フレームの頂点ストリームを算出
		* @param[out] vertexStream 頂点ストリーム
		* @param[in]  boneStream ボーンストリーム
		* @param[in]  initialVertices 初期頂点群
		* @param[in]  initialBones 初期ボーン群
		* @param[in]  frameNo フレーム番号
		*/
		void calcVertexStream(VertexStream& vertexStream, const BoneStream& boneStream,
			const vector<Vertex>& initialVertices, const vector<Bone>& initialBones, const int frameNo);

		void transChildBone(vector<Bone>& afterBones, const vector<Bone>& beforeBones,
			const vector<BoneInfo>& boneInfoList, const vector<int>&parentIndices);

		void moveChildBones(vector<Bone>& bones, const int parentBoneIndex,
			const vector<BoneInfo>& boneInfoList, const Eigen::Vector3f& parentBeforePos);

		/*! @brief ボーン情報リストマップ
		* map<フレーム番号, ボーン位置姿勢情報>
		*/
		multimap<int, BoneInfo> boneInfoListMap_;
	};
}

#endif