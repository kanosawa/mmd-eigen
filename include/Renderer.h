#ifndef MMD_RENDERER_H
#define MMD_RENDERER_H

#include <GL/glut.h>
#include "PmxModel.h"
#include "MotionStream.h"
#include "ModelUpdater.h"

namespace mmd {
    /*! @brief レンダラークラス
    */
    class Renderer {
    public:
        /*! @brief インスタンスの取得
         */
        static Renderer& getInstance();

        /*! @brief パラメータの設定
         */
        void setParam(const PmxModel& model, const vector<MotionStream>& motionStreams);

        /*! @brief スタート
         */
        void start();

        /*! @brief DisplayFunc
        */
        void display();

        /*! @brief MouseFunc
        */
        void mouse(int button, int state, int x, int y);

        /*! @brief MotionFunc
        */
        void motion(int x, int y);

        /*! @brief ResizeFunc
        */
        void resize(int w, int h);

        /*! @brief IdleFunc
        */
        void idle();

    private:
        Renderer();

        ~Renderer();

        /*! @brief モデル更新クラス
        */
        ModelUpdater updater_;

        vector<GLuint> texname_;

        /*! @brief ウィンドウ幅
        */
        int width_;

        /*! @brief ウィンドウ高さ
        */
        int height_;

        /*! @brief ベース時間
        */
        int time_base_;

        /*! @brief マウスボタン
        */
        int mouse_l_, mouse_m_, mouse_r_;

        /*! @brief マウス位置
        */
        int mouse_pos_[2];

        /*! @brief 視点情報
        */
        double theta_[3], angle_[3];
    };
}

#endif