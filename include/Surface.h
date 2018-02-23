#ifndef MMD_SURFACE_H
#define MMD_SURFACE_H

#include <Eigen/Dense>

namespace mmd {
    /*! @brief サーフェスクラス
    */
    class Surface {
    public:
        Surface();
        ~Surface();

        /*! @brief コンストラクタ
        * @param[in] vertices サーフェスを構成する頂点インデックス
        */
        Surface(const Eigen::Vector3i &vertexIndexies);

        const Eigen::Vector3i &getVertexIndexies() const;

    private:
        /*! @brief サーフェスを構成する頂点インデックス
        */
        Eigen::Vector3i vertexIndexies_;
    };
}

#endif