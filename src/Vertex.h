#ifndef MMD_VERTEX_H
#define MMD_VERTEX_H

#include <Eigen/Dense>

using namespace std;

namespace mmd {
    /*! @brief 頂点クラス
    */
    class Vertex {
    public:
        Vertex();
        ~Vertex();

        /*! @brief コンストラクタ
        * @param[in] initialPosition モデル座標系における三次元座標(initial)
        * @param[in] uv UV座標
        * @param[in] refBoneIndices 参照ボーンインデックス
        * @param[in] refBoneWeightList 参照ボーン重み
        */
        Vertex(const Eigen::Vector3f &initialPosition, const Eigen::Vector2f &uv,
               const vector<int> &refBoneIndices, const vector<float> &refBoneWeightList);

        void setTemporalPosition(const Eigen::Vector3f &temporal_position);

        const Eigen::Vector3f &getInitialPosition() const;
        const Eigen::Vector3f &getTemporalPosition() const;
        const Eigen::Vector2f &getUv() const;
        const vector<int> &getRefBoneIndices() const;
        const vector<float> &getRefBoneWeightList() const;

    private:
        /*! @brief モデル座標系における三次元座標(initial)
        */
        Eigen::Vector3f initialPosition_;

        /*! @brief UV座標
        */
        Eigen::Vector2f uv_;

        /*! @brief 参照ボーンインデックス
        */
        vector<int> refBoneIndices_;

        /*! @brief 参照ボーン重み
        */
        vector<float> refBoneWeightList_;

        /*! @brief モデル座標系における三次元座標(temporal)
        */
        Eigen::Vector3f temporalPosition_;
    };
}

#endif