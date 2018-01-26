#ifndef MMD_PMX_MODEL_H
#define MMD_PMX_MODEL_H

#include <opencv2/opencv.hpp>
#include "Common.h"
#include "Material.h"
#include "Bone.h"

namespace mmd {
    /*! @brief PMXモデルクラス
    */
    class PmxModel {
    public:
        /*! @brief コンストラクタ
        */
        PmxModel();

        /*! @brief デストラクタ
        */
        ~PmxModel();

        /*! @brief 頂点の追加
        * @param[in] vertex 追加する頂点
        */
        void pushBackVertex(const Vertex &vertex);

        /*! @brief 面の追加
        * @param[in] surface 追加する面
        */
        void pushBackSurface(const TriangleSurface &surface);

        /*! @brief テクスチャの追加
        * @param[in] texture 追加するテクスチャ
        */
        void pushBackTexture(const cv::Mat &texture);

        /*! @brief マテリアルの追加
        * @param[in] material 追加するマテリアル
        */
        void pushBackMaterial(const Material &material);

        /*! @brief ボーンの追加
        * @param[in] bone 追加するボーン
        */
        void pushBackBone(const Bone &bone);

        /*! @brief 頂点の取得
        * @param[in] vertexNo 頂点番号
        * @return 頂点
        */
        const Vertex &getVertex(const unsigned int vertexNo) const;

        /*! @brief 頂点数の取得
        * @return 頂点数
        */
        const int getVertexNum() const;

        /*! @brief 頂点群の取得
        * @return 頂点群
        */
        const vector<Vertex> &getVertices() const;

        /*! @brief 面の取得
        * @param[in] surfaceNo 面番号
        * @return 面
        */
        const TriangleSurface &getSurface(const unsigned int surfaceNo) const;

        /*! @brief 面数の取得
        * @return 面数
        */
        const int getSurfaceNum() const;

        /*! @brief テクスチャの取得
        * @param[in] textureNo テクスチャ番号
        * @return テクスチャ
        */
        const cv::Mat &getTexture(const unsigned int textureNo) const;

        /*! @brief テクスチャ数の取得
        * @return テクスチャ数
        */
        const int getTextureNum() const;

        /*! @brief マテリアルの取得
        * @param[in] materialNo マテリアル番号
        * @return マテリアル
        */
        const Material &getMaterial(const unsigned int materialNo) const;

        /*! @brief マテリアル数の取得
        * @return マテリアル数
        */
        int getMaterialNum() const;

        /*! @brief ボーンの取得
        * @param[in] boneNo ボーン番号
        * @return ボーン
        */
        const Bone &getBone(const unsigned int boneNo) const;

        /*! @brief ボーンの取得（非const, 子ボーンインデックス追加のため）
        * @param[in] boneNo ボーン番号
        * @return ボーン
        */
        Bone &getBone(const unsigned int boneNo);

        /*! @brief ボーン数の取得
        * @return ボーン数
        */
        const int getBoneNum() const;

        /*! @brief ボーン群の取得
        * @return ボーン群
        */
        const vector<Bone> &getBones() const;

    private:
        /*! @brief コピーコンストラクタの禁止
        */
        PmxModel(const PmxModel &);

        /*! @brief 代入演算子の禁止
        */
        PmxModel &operator=(const PmxModel &);

        /*! @brief モデルを構成する頂点群
        */
        std::vector<Vertex> vertices_;

        /*! @brief モデルを構成する面群
        */
        std::vector<TriangleSurface> surfaces_;

        /*! @brief モデルを構成するテクスチャ群
        */
        std::vector<cv::Mat> textures_;

        /*! @brief モデルを構成するマテリアル群
        */
        std::vector<Material> materials_;

        /*! @brief モデルを構成するボーン群
        */
        std::vector<Bone> bones_;
    };
}

#endif