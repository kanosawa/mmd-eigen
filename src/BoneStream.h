#ifndef PMX_BONE_STREAM_H
#define PMX_BONE_STREAM_H

#include <map>
#include "Bone.h"

namespace mmd {
    /*! @brief ボーンストリームクラス
    */
    class BoneStream {
    public:
        /*! @brief コンストラクタ
        */
        BoneStream();

        /*! @brief デストラクタ
        */
        ~BoneStream();

        /*! @brief ボーンリストの追加
        * @param[in] frameNo フレーム番号
        * @param[in] bones ボーン群
        */
        void pushBackBones(const int frameNo, const vector<Bone> &bones);

        /*! @brief ボーンリストの取得
        * @param[in] frameNo フレーム番号
        * @return ボーン群
        */
        bool getBones(vector<Bone> &bones, const int frameNo) const;

        /*! @brief ボーンリストマップの取得
        * @return ボーンリストマップ
        */
        const map<int, vector<Bone>> &getBoneListMap() const;

    private:
        /*! @brief コピーコンストラクタの禁止
        */
        BoneStream(const BoneStream &);

        /*! @brief 代入演算子の禁止
        */
        BoneStream &operator=(const BoneStream &);

        /*! @brief ボーンリストマップ
        * pair<フレーム番号, 離散フレームのボーン群>
        */
        map<int, vector<Bone>> boneListMap_;
    };
}

#endif