#ifndef MMD_MOTION_STREAM_H
#define MMD_MOTION_STREAM_H

#include <vector>
#include "Motion.h"

using namespace std;

namespace mmd {
    /*! @brief モーションストリームクラス
    */
    class MotionStream {
    public:
        /*! @brief コンストラクタ
         * @param[in] motions モーション
        */
        MotionStream(const vector<Motion> &motions);

        /*! @brief デストラクタ
        */
        ~MotionStream();

        /*! @brief ポインタを進める
        */
        void incrementPointer();

        /*! @brief 直近モーションの取得
        * @return 直近モーション
        */
        const Motion& getLatestMotion() const;

        /*! @brief 次モーションの取得
        * @return 次モーション
        */
        const Motion& getNextMotion() const;

        /*! @brief 最後のモーションか否か
        * @return 最後のモーションならtrue
        */
        const bool isLastMotion() const;

    private:
        /*! @brief デフォルトコンストラクタの禁止
        */
        MotionStream();

        /*! @brief モーション
        */
        vector<Motion> motions_;

        /*! @brief ポインタ
        */
        unsigned int pointer_;
    };
}

#endif
