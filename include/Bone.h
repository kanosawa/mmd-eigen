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

        struct IK {
            /*! @brief ターゲットボーンインデックス
            */
            int targetIndex;

            /*! @brief ループ回数
            */
            int loopNum;

            /*! @brief 単位角
            */
            float unitAngle;

            /*! @brief リンクボーンインデックス
            */
            vector<int> linkIndices;

            /*! @brief 角度制限フラグ
            */
            vector<bool> angleLimitFlags;
        };

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
        void setIKFlag(const bool IKFlag);
        void setAssignRotFlag(const bool assignRotFlag);
        void setAssignShiftFlag(const bool assignShiftFlag);
        void setAssignParentIndex(const int assignParentIndex);
        void setAssignRatio(const float assignRatio);
        void setIK(const IK &ik);
        void setTemporalPosition(const Eigen::Vector3f &temporalPosition);
        void setTemporalQuaternion(const Eigen::Quaternionf &temporalQuaternion);

        const string &getBoneName() const;
        const Eigen::Vector3f &getInitialPosition() const;
        const int getParentBoneIndex() const;
        const bool getDestinationFlag() const;
        const Eigen::Vector3f &getOffset() const;
        const int getDestinationBoneIndex() const;
        const vector<int> &getChildBoneIndices() const;
        const bool getIKFlag() const;
        const bool getAssignRotFlag() const;
        const bool getAssignShiftFlag() const;
        const int getAssignParentIndex() const;
        const float getAssignRatio() const;
        const IK &getIK() const;
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

        /*! @brief IKFlag
        */
        bool IKFlag_;

        /*! @brief 回転付与フラグ
        */
        bool assignRotFlag_;

        /*! @brief 移動付与フラグ
        */
        bool assignShiftFlag_;

        /*! @brief 付与親インデックス
        */
        int assignParentIndex_;

        /*! @brief 付与率
        */
        float assignRatio_;

        /*! @brief IK
        */
        IK ik_;

        /*! @brief 三次元座標(temporal)
        */
        Eigen::Vector3f temporalPosition_;

        /*! @brief クォータニオン(temporal)
        */
        Eigen::Quaternionf temporalQuaternion_;
    };
}

#endif