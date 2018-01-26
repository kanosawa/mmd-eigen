#ifndef MMD_SURFACE_H
#define MMD_SURFACE_H

#include "Vertex.h"

namespace mmd {
    /*! @brief 面クラス
    * T : Eigen::Vector3iを想定
    */
    template<class T>
    class Surface {
    public:
        /*! @brief コンストラクタ
        */
        Surface();

        /*! @brief コンストラクタ
        * @param[in] vertices 面を構成する頂点インデックス群
        */
        Surface(const T &vertexIndexies);

        /*! @brief コピーコンストラクタ
        */
        Surface(const Surface<T> &surface);

        /*! @brief デストラクタ
        */
        ~Surface();

        /*! @brief 代入演算子
        */
        Surface<T> &operator=(const Surface<T> &surface);

        /*! @brief 面を構成する頂点インデックス群の設定
        * @param[in] vertexIndexies 面を構成する頂点インデックス群
        */
        void setVertexIndexies(const T &vertexIndexies);

        /*! @brief 面を構成する頂点インデックス群の取得
        * @return 面を構成する頂点インデックス群
        */
        const T &getVertexIndexies() const;

    private:
        /*! @brief 面を構成する頂点インデックス群
        */
        T vertexIndexies_;
    };

    template<class T>
    Surface<T>::Surface() {
    }


    template<class T>
    Surface<T>::Surface(const T &vertexIndexies)
            : vertexIndexies_(vertexIndexies) {
    }


    template<class T>
    Surface<T>::Surface(const Surface<T> &surface)
            : vertexIndexies_(surface.getVertexIndexies()) {
    }


    template<class T>
    Surface<T>::~Surface() {
    }

    template<class T>
    Surface<T> &Surface<T>::operator=(const Surface<T> &surface) {
        vertexIndexies_ = surface.getVertexIndexies();
    }

    template<class T>
    void Surface<T>::setVertexIndexies(const T &vertexIndexies) {
        vertexIndexies_ = vertexIndexies;
    }


    template<class T>
    const T &Surface<T>::getVertexIndexies() const {
        return vertexIndexies_;
    }
}

#endif