#ifndef MMD_VMD_DATA_STREAM_H
#define MMD_VMD_DATA_STREAM_H

#include "BoneStream.h"
#include "VertexStream.h"
#include "Motion.h"

namespace mmd {
    /*! @brief VMDデータストリームクラス
    * ファイルもしくはコンストラクタ引数からVMDデータリストを取得。
    * そして、離散フレームのボーン群を算出する。
    */

    class VmdDataStream {
    public:
        /*! @brief ボーン位置姿勢情報
        */
        class BoneInfo {
        public:
            int boneIndex; // ボーンインデックス
            Eigen::Vector3f shift; // シフト量
            Eigen::Quaternionf quaternion; // クォータニオン

            bool operator<(const BoneInfo& rhs) const {
                return (boneIndex < rhs.boneIndex);
            }

            bool operator>(const BoneInfo& rhs) const {
                return (boneIndex > rhs.boneIndex);
            }
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
        void insertBoneInfoList(const int frameNo, const Motion &motion);

        /*! @brief 離散フレームのボーン・頂点ストリームを算出
        * @param[out] boneStream ボーンストリーム
        * @param[out] vertexStream 頂点ストリーム
        * @param[in]  initialBones 初期ボーン群
        * @param[in]  initialVertices 初期頂点群
        */
        bool calcStream(BoneStream &boneStream, VertexStream &vertexStream,
                        const vector<Bone> &initialBones, const vector<Vertex> &initialVertices);

    private:
        /*! @brief コピーコンストラクタの禁止
        */
        VmdDataStream(const VmdDataStream &);

        /*! @brief 代入演算子の禁止
        */
        VmdDataStream &operator=(const VmdDataStream &);

        /*! @brief ボーン情報リストの分割
        * @param[out] boneInfoListVec ボーン情報リストベクタ
        */
        void splitBoneInfoList(vector<pair<int, vector<Motion>>> &boneInfoListVec);

        /*! @brief 離散フレームのボーンストリームを算出
        * @param[out] boneStream ボーンストリーム
        * @param[in, out]  bones ボーン群
        * @param[in]  boneInfoList ボーン情報リスト
        * @param[in]  frameNo フレーム番号
        * @param[in]  parentIndex 親インデックス
        */
        void calcBoneStream(BoneStream &boneStream, vector<Bone> &bones,
                            const vector<Motion> &frameMotions, const int frameNo, const int parentIndex);

        /*! @brief 離散フレームの頂点ストリームを算出
        * @param[out] vertexStream 頂点ストリーム
        * @param[in]  initialVertices 初期頂点群
        * @param[in]  initialBones 初期ボーン群
        * @param[in]  frameNo フレーム番号
        */
        void calcVertexStream(VertexStream &vertexStream,
                              const vector<Vertex> &initialVertices, const vector<Bone> &initialBones,
                              const int frameNo);

        /*! @brief 子ボーンの移動
         * @param[in, out] bones ボーン群
         * @param[in] parentBoneIndex 親ボーンインデックス
         * @param[in] boneInfoList ボーン情報リスト
         */
        void moveChildBones(vector<Bone> &bones, const int parentBoneIndex,
                            const vector<Motion> &frameMotions);

        /*! @brief 全ての親ボーンの探索
         * @param[in, out] bones ボーン群
         */
        int searchSuperParentBone(const vector<Bone> &bones);

        /*! @brief ボーン情報リストマップ
        * map<フレーム番号, ボーン位置姿勢情報>
        */
        multimap<int, Motion> boneInfoListMap_;
    };
}

#endif