#ifndef MMD_MOTION_STREAM_GENERATOR_H
#define MMD_MOTION_STREAM_GENERATOR_H

#include "MotionStream.h"
#include "Bone.h"

namespace mmd {
    /*! @brief モーションストリーム生成クラス
    */
    class MotionStreamGenerator {
    public:

        MotionStreamGenerator();
        ~MotionStreamGenerator();

        /*! @brief モーションストリームの生成
         * @param[out] motionStreams モーションストリーム
         * @param[in]  motions vmdファイルから入力したモーションデータ
         * @param[in]  bones pmxファイルから入力したボーンデータ
         */
        void generate(vector<MotionStream>& motionStreams, const vector<Motion>& motions, const vector<Bone>& bones);
    };
}

#endif