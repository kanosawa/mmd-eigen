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
        /*! @brief コンストラクタ
        */
        Bone();

        /*! @brief デストラクタ
        */
        ~Bone();

        /*! @brief ボーン名（日本語）の設定
        * @param[in] boneName ボーン名（日本語）
        */
        void setBoneName(const string &boneName);

        /*! @brief ボーン名（日本語）の取得
        * @return ボーン名（日本語）
        */
        const string &getBoneName() const;

        /*! @brief 三次元座標(initial)の設定
        * @param[in] initial_position 三次元座標(initial)
        */
        void setInitialPosition(const Eigen::Vector3f &initial_position);

        /*! @brief 三次元座標(initial)の取得
        * @return 三次元座標(initial)
        */
        const Eigen::Vector3f &getInitialPosition() const;

        /*! @brief 三次元座標(temporal)の設定
        * @param[in] temporal_position 三次元座標
        */
        void setTemporalPosition(const Eigen::Vector3f &temporal_position);

        /*! @brief 三次元座標(temporal)の取得
        * @return 三次元座標(temporal)
        */
        const Eigen::Vector3f &getTemporalPosition() const;

        /*! @brief クォータニオン(temporal)の設定
        * @param[in] quartanion クォータニオン(temporal)
        */
        void setTemporalQuartanion(const Eigen::Quaternionf &temporal_quaternion);

        /*! @brief クォータニオンの取得
        * @return クォータニオン
        */
        const Eigen::Quaternionf &getTemporalQuaternion() const;

        /*! @brief 親ボーンインデックスの設定
        * @param[in] parentBoneIndex 親ボーンインデックス
        */
        void setParentBoneIndex(const int parentBoneIndex);

        /*! @brief 親ボーンインデックスの取得
        * @return 親ボーンインデックス
        */
        const int getParentBoneIndex() const;

        /*! @brief 子ボーンインデックスの追加
        * @param[in] 子ボーンインデックス
        */
        void pushBackChildBoneIndex(const int childBoneIndex);

        /*! @brief 子ボーンインデックスの取得
        * @return 子ボーンインデックス
        */
        const vector<int> &getChildBoneIndices() const;

        /*! @brief 接続先フラグの設定
        * @param[in] destinationFlag 接続先フラグ
        */
        void setDestinationFlag(const bool &destinationFlag);

        /*! @brief 接続先フラグの取得
        * @return 接続先フラグ
        */
        const bool getDestinationFlag() const;

        /*! @brief 座標オフセットの設定
        * @param[in] offset 座標オフセット
        */
        void setOffset(const Eigen::Vector3f &offset);

        /*! @brief 座標オフセットの取得
        * @return 座標オフセット
        */
        const Eigen::Vector3f &getOffset() const;

        /*! @brief 接続先ボーンインデックスの設定
        * @param[in] destinationBoneIndex 接続先ボーンインデックス
        */
        void setDestinationBoneIndex(const int &destinationBoneIndex);

        /*! @brief 接続先ボーンインデックスの取得
        * @return 接続先ボーンインデックス
        */
        int getDestinationBoneIndex();

    private:

        /*! ボーン名（日本語）
        */
        string boneName_;

        /*! @brief 親ボーンのインデックス
        */
        int parentBoneIndex_;

        /*! @brief 子ボーンのインデックス
        */
        vector<int> childBoneIndices_;

        /*! @brief 接続先フラグ(0:座標オフセットで指定, 1:ボーンで指定)
        */
        bool destinationFlag_;

        /*! @brief 座標オフセット
        */
        Eigen::Vector3f offset_;

        /*! @brief 接続先ボーンインデックス
        */
        int destinationBoneIndex_;

        /*! @brief 三次元座標(initial)
        */
        Eigen::Vector3f initial_position_;

        /*! @brief 三次元座標(temporal)
        */
        Eigen::Vector3f temporal_position_;

        /*! @brief クォータニオン(temporal)
        */
        Eigen::Quaternionf temporal_quaternion_;
    };
}

#endif