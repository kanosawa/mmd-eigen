#ifndef PMX_VERTEX_STREAM_H
#define PMX_VERTEX_STREAM_H

#include "Vertex.h"

namespace mmd {
    /*! @brief 頂点ストリームクラス
    */
    class VertexStream {
    public:
        /*! @brief コンストラクタ
        */
        VertexStream();

        /*! @brief デストラクタ
        */
        ~VertexStream();

        /*! @brief 頂点群の追加
        * @param[in] frameNo フレーム番号
        * @param[in] 頂点群
        */
        void pushBackVertices(const int frameNo, const vector<Vertex> &vertices);

        /*! @brief 頂点群の取得
        * @param[in] frameNo フレーム番号
        * @return 頂点群
        */
        bool getVertices(vector<Vertex> &vertices, const int frameNo) const;

        /*! @brief 頂点リストマップの取得
        * @return 頂点リストマップ
        */
        const map<int, vector<Vertex>> &getVertexListMap() const;

    private:
        /*! @brief コピーコンストラクタの禁止
        */
        VertexStream(const VertexStream &);

        /*! @brief 代入演算子の禁止
        */
        VertexStream &operator=(const VertexStream &);

        /*! @brief 頂点リストマップ
        * pair<フレーム番号, 離散フレームの頂点群>
        */
        map<int, vector<Vertex>> vertexListMap_;
    };
}

#endif