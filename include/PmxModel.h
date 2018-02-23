#ifndef MMD_PMX_MODEL_H
#define MMD_PMX_MODEL_H

#include <opencv2/opencv.hpp>
#include "Vertex.h"
#include "Surface.h"
#include "Material.h"
#include "Bone.h"

namespace mmd {
    /*! @brief PMXモデルクラス
    */
    class PmxModel {
    public:
        PmxModel();
        ~PmxModel();

        void pushBackVertex(const Vertex &vertex);
        void pushBackSurface(const Surface &surface);
        void pushBackTexture(const cv::Mat &texture);
        void pushBackMaterial(const Material &material);
        void pushBackBone(const Bone &bone);

        const vector<Vertex> &getVertices() const;
        const vector<Surface> &getSurfaces() const;
        const vector<cv::Mat> &getTextures() const;
        const vector<Material> &getMaterials() const;
        const vector<Bone> &getBones() const;

        /*! @brief 子ボーンインデックスを追加
         * @param[in] boneIndex 子ボーンインデックスを追加する対象ボーンのインデックス
         * @param[in] childBoneIndex 子ボーンインデックス
         */
        void pushBackChildBoneIndex(const int boneIndex, const int childBoneIndex);

        /*! @brief ボーンの三次元座標(temporal)を設定
         * @param[in] boneIndex 設定対象ボーンのインデックス
         * @param[in] temporalPosition 三次元座標(temporal)
         */
        void setBoneTemporalPosition(const int boneIndex, const Eigen::Vector3f &temporalPosition);

        /*! @brief ボーンのクォータニオン(temporal)を設定
         * @param[in] boneIndex 設定対象ボーンのインデックス
         * @param[in] temporalQuaternion クォータニオン(temporal)
         */
        void setBoneTemporalQuaternion(const int boneIndex, const Eigen::Quaternionf &temporalQuaternion);

        /*! @brief 頂点の三次元座標(temporal)を設定
         * @param[in] vertexIndex 設定対象頂点のインデックス
         * @param[in] temporalPosition 三次元座標(temporal)
         */
        void setVertexTemporalPosition(const int vertexIndex, const Eigen::Vector3f &temporalPosition);

    private:
        /*! @brief モデルを構成する頂点
        */
        std::vector<Vertex> vertices_;

        /*! @brief モデルを構成するサーフェス
        */
        std::vector<Surface> surfaces_;

        /*! @brief モデルを構成するテクスチャ
        */
        std::vector<cv::Mat> textures_;

        /*! @brief モデルを構成するマテリアル
        */
        std::vector<Material> materials_;

        /*! @brief モデルを構成するボーン
        */
        std::vector<Bone> bones_;
    };
}

#endif