#ifndef MMD_BONE_H
#define MMD_BONE_H

#include <string>
#include <Eigen/Dense>

using namespace std;

namespace mmd {
    /*! @brief ボーンクラス
    */
    class Bone {
    public:
        Bone();
        ~Bone();

        /*! @brief コンストラクタ
         * @param[in] boneName ボーン名（日本語）
         * @param[in] initialPosition 三次元座標(initial)
         * @param[in] parentBoneIndex 親ボーンインデックス
        */
        Bone(const string &boneName, const Eigen::Vector3f &initialPosition, const int parentBoneIndex);

        void setDestinationFlag(const bool destinationFlag);
        void setOffset(const Eigen::Vector3f &offset);
        void setDestinationBoneIndex(const int destinationBoneIndex);
        void setTemporalPosition(const Eigen::Vector3f &temporalPosition);
        void setTemporalQuaternion(const Eigen::Quaternionf &temporalQuaternion);

        const string &getBoneName() const;
        const Eigen::Vector3f &getInitialPosition() const;
        const int getParentBoneIndex() const;
        const bool getDestinationFlag() const;
        const Eigen::Vector3f &getOffset() const;
        const int getDestinationBoneIndex() const;
        const vector<int> &getChildBoneIndices() const;
        const Eigen::Vector3f &getTemporalPosition() const;
        const Eigen::Quaternionf &getTemporalQuaternion() const;

        /*! @brief 子ボーンインデックスの追加
        * @param[in] 子ボーンインデックス
        */
        void pushBackChildBoneIndex(const int childBoneIndex);

    private:
        /*! @brief ボーン名（日本語）
        */
        string boneName_;

        /*! @brief 三次元座標(initial)
        */
        Eigen::Vector3f initialPosition_;

        /*! @brief 親ボーンのインデックス
        */
        int parentBoneIndex_;

        /*! @brief 接続先フラグ(0:座標オフセットで指定, 1:ボーンで指定)
        */
        bool destinationFlag_;

        /*! @brief 座標オフセット
        */
        Eigen::Vector3f offset_;

        /*! @brief 接続先ボーンインデックス
        */
        int destinationBoneIndex_;

        /*! @brief 子ボーンのインデックス
        */
        vector<int> childBoneIndices_;

        /*! @brief 三次元座標(temporal)
        */
        Eigen::Vector3f temporalPosition_;

        /*! @brief クォータニオン(temporal)
        */
        Eigen::Quaternionf temporalQuaternion_;
    };
}

#endif