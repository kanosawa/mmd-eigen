#ifndef MMD_MOTION_H
#define MMD_MOTION_H

#include <Eigen/Dense>

namespace mmd {
    /*! @brief モーションクラス
    */
    class Motion {
    public:
        /*! @brief コンストラクタ
        */
        Motion();

        /*! @brief コンストラクタ
         * @param[in] boneIndex ボーンインデックス
         * @param[in] frameNo フレーム番号
         * @param[in] shift シフト
         * @param[in] quaternion クォータニオン
        */
        Motion(const int boneIndex, const int frameNo, const Eigen::Vector3f& shift, const Eigen::Quaternionf& quaternion);

        /*! @brief デストラクタ
        */
        ~Motion();

        void setQuaternion(const Eigen::Quaternionf& quaternion);

        /*! @brief ボーンインデックスの取得
        * @return ボーンインデックス
        */
        const int getBoneIndex() const;

        /*! @brief フレーム番号の取得
        * @return フレーム番号
        */
        const int getFrameNo() const;

        /*! @brief シフトの取得
        * @return シフト
        */
        const Eigen::Vector3f& getShift() const;

        /*! @brief クォータニオンの取得
        * @return クォータニオン
        */
        const Eigen::Quaternionf& getQuaternion() const;

    private:
        /*! @brief ボーンインデックス
        */
        int boneIndex_;

        /*! @brief フレーム番号
        */
        int frameNo_;

        /*! @brief シフト
        */
        Eigen::Vector3f shift_;

        /*! @brief クォータニオン
        */
        Eigen::Quaternionf quaternion_;
    };
}

#endif
