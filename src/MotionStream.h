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
        */
        MotionStream();

        /*! @brief デストラクタ
        */
        ~MotionStream();

        /*! @brief モーションの設定
        * @param[in] motions モーション
        */
        void setMotions(const vector<Motion> &motions);

        /*! @brief モーションの取得
        * @return モーション
        */
        const vector<Motion>& getMotions();

        /*! @brief ポインタの取得
        * @return ポインタ
        */
        const int getPointer() const;

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

    private:
        vector<Motion> motions_;
        int pointer_;
    };
}

#endif
