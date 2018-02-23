#ifndef MMD_MATERIAL_H
#define MMD_MATERIAL_H

#include <Eigen/Dense>

namespace mmd {
    /*! @brief マテリアルクラス
    */
    class Material {
    public:
        Material();
        ~Material();

        /*! @brief コンストラクタ
         * @param[in] diffuse ディフューズ
         * @param[in] specular スペキュラー
         * @param[in] specularCoef スペキュラー係数
         * @param[in] ambient アンビエント
         * @param[in] ordinaryTextureIndex 通常テクスチャインデックス
         * @param[in] surfaceNum マテリアルに対応するサーフェス数
        */
        Material(const Eigen::Vector4f &diffuse, const Eigen::Vector3f &specular, const float specularCoef,
                 const Eigen::Vector3f &ambient, const int ordinaryTextureIndex, const int surfaceNum);

        const Eigen::Vector4f &getDiffuse() const;
        const Eigen::Vector3f &getSpecular() const;
        const float getSpecularCoef() const;
        const Eigen::Vector3f &getAmbient() const;
        const int getOrdinaryTextureIndex() const;
        const int getSurfaceNum() const;

    private:
        /*! @brief ディフューズ
        */
        Eigen::Vector4f diffuse_;

        /*! @brief スペキュラー
        */
        Eigen::Vector3f specular_;

        /*! @brief スペキュラー係数
        */
        float specularCoef_;

        /*! @brief アンビエント
        */
        Eigen::Vector3f ambient_;

        /*! @brief 通常テクスチャインデックス
        */
        int ordinaryTextureIndex_;

        /*! @brief マテリアルに対応するサーフェス数
        */
        int surfaceNum_;
    };
}

#endif