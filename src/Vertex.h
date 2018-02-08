#ifndef MMD_VERTEX_H
#define MMD_VERTEX_H

#include <Eigen/Dense>

using namespace std;

namespace mmd {
    /*! @brief 頂点クラス
    */
    class Vertex {
    public:
        /*! @brief コンストラクタ
        */
        Vertex();

        /*! @brief コンストラクタ
        * @param[in] position モデル座標系における三次元座標
        * @param[in] uv UV座標
        * @param[in] refBoneIndices 参照ボーンインデックス
        * @param[in] refBoneWeightList 参照ボーン重み
        */
        Vertex(const Eigen::Vector3f &position, const Eigen::Vector2f &uv,
               const vector<int> &refBoneIndices, const vector<float> &refBoneWeightList);

        /*! @brief デストラクタ
        */
        ~Vertex();

        /*! @brief 三次元座標(initial)の設定
        * @param[in] initial_position モデル座標系における三次元座標(initial)
        */
        void setInitialPosition(const Eigen::Vector3f &intiail_position);

        /*! @brief 三次元座標(initial)の取得
        * @return モデル座標系における三次元座標(initial)
        */
        const Eigen::Vector3f &getInitialPosition() const;

        /*! @brief 三次元座標(temporal)の設定
        * @param[in] temporal_position モデル座標系における三次元座標(temporal)
        */
        void setTemporalPosition(const Eigen::Vector3f &temporal_position);

        /*! @brief 三次元座標(temporal)の取得
        * @return モデル座標系における三次元座標(temporal)
        */
        const Eigen::Vector3f &getTemporalPosition() const;

        /*! @brief UV座標の設定
        * @param[in] uv UV座標
        */
        void setUv(const Eigen::Vector2f &uv);

        /*! @brief UV座標の取得
        * @return UV座標
        */
        const Eigen::Vector2f &getUv() const;

        /*! @brief 参照ボーンインデックスの取得
        * @return 参照ボーンインデックス
        */
        const vector<int> &getRefBoneIndices() const;

        /*! @brief 参照ボーン重みの取得
        * @return 参照ボーン重み
        */
        const vector<float> &getRefBoneWeightList() const;

    private:
        /*! @brief モデル座標系における三次元座標(initial)
        */
        Eigen::Vector3f initial_position_;

        /*! @brief モデル座標系における三次元座標(temporal)
        */
        Eigen::Vector3f temporal_position_;

        /*! @brief UV座標
        */
        Eigen::Vector2f uv_;

        /*! @brief 参照ボーンインデックス
        */
        vector<int> refBoneIndices_;

        /*! @brief 参照ボーン重み
        */
        vector<float> refBoneWeightList_;
    };

}

#endif