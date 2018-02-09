#ifndef MMD_SURFACE_H
#define MMD_SURFACE_H

#include <Eigen/Dense>
#include "Vertex.h"

namespace mmd {
    /*! @brief 面クラス
    */
    class Surface {
    public:
        /*! @brief コンストラクタ
        */
        Surface();

        /*! @brief コンストラクタ
        * @param[in] vertices 面を構成する頂点インデックス群
        */
        Surface(const Eigen::Vector3i &vertexIndexies);

        /*! @brief コピーコンストラクタ
        */
        Surface(const Surface &surface);

        /*! @brief デストラクタ
        */
        ~Surface();

        /*! @brief 代入演算子
        */
        Surface &operator=(const Surface &surface);

        /*! @brief 面を構成する頂点インデックス群の設定
        * @param[in] vertexIndexies 面を構成する頂点インデックス群
        */
        void setVertexIndexies(const Eigen::Vector3i &vertexIndexies);

        /*! @brief 面を構成する頂点インデックス群の取得
        * @return 面を構成する頂点インデックス群
        */
        const Eigen::Vector3i &getVertexIndexies() const;

    private:
        /*! @brief 面を構成する頂点インデックス群
        */
        Eigen::Vector3i vertexIndexies_;
    };
}

#endif