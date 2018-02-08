#ifndef MMD_MODEL_UPDATER_H
#define MMD_MODEL_UPDATER_H

#include "PmxModel.h"
#include "MotionStream.h"

namespace mmd {
    /*! @brief モデル更新クラス
    */
    class ModelUpdater {
    public:
        /*! @brief コンストラクタ
        */
        ModelUpdater();

        /*! @brief デストラクタ
        */
        ~ModelUpdater();

        /*! @brief モデルを更新
        */
        void update();

        /*! @brief パラメータを設定
         * @param[in] model モデル
         * @param[in] motionStreams モーションストリーム
        */
        void setParam(const PmxModel& model, const vector<MotionStream>& motionStreams);

        /*! @brief モデルを取得
         * @return モデル
        */
        const PmxModel& getModel() const;

    private:
        /*! @brief 全ての親ボーンを探索
         * @param[in] bones ボーン群
         * @return 全ての親ボーンのインデックス
        */
        int searchSuperParentBone(const vector<mmd::Bone> &bones);

        /*! @brief 全ての親ボーンを探索
         * @param[in] bones ボーン群
         * @param[in] parentBoneIndex 親ボーンインデックス
         * @param[in] frameMotions フレームモーション
        */
        void moveChildBones(vector<Bone> &bones, const int parentBoneIndex,
                            const vector<Motion> &frameMotions);

        /*! @brief 現在フレーム番号
        */
        int currentFrameNo_;

        /*! @brief 全ての親ボーンのインデックス
        */
        int superParentIndex_;

        /*! @brief モデル
        */
        PmxModel model_;

        /*! @brief モーションストリーム
        */
        vector<MotionStream> motionStreams_;
    };
}

#endif