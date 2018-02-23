#ifndef MMD_PMD_MODEL_FILE_READER_H
#define MMD_PMD_MODEL_FILE_READER_H

#include "ModelFileReader.h"

namespace mmd {
    /*! @brief PMDファイル読み込みクラス
    */
    class PmdModelFileReader : public ModelFileReader {

    public:
        /*! @brief コンストラクタ
        * @param[in] filename
        */
        PmdModelFileReader(const string &filename);

        ~PmdModelFileReader();

        /*! @brief PMDモデルファイルの読み込み
        * @return モデル
        */
        void readFile(PmxModel& model);

    private:
        /*! @brief デフォルトコンストラクタの禁止
        */
        PmdModelFileReader();

        /*! @brief ヘッダの読み込み
        * @param[out] model モデル
        * @return 成否
        */
        bool readHeader(PmxModel &model);

        /*! @brief モデル情報の読み込み
        * @return 成否
        */
        bool readModelInfo();

        /*! @brief 頂点の読み込み
        * @param[out] model モデル
        * @return 成否
        */
        bool readVertices(PmxModel &model);

        /*! @brief 面の読み込み
        * @param[out] model モデル
        * @return 成否
        */
        bool readSurfaces(PmxModel &model);

        /*! @brief マテリアルの読み込み
        * @param[out] model モデル
        * @return 成否
        */
        bool readMaterials(PmxModel &model);

        /*! @brief ボーンの読み込み
        * @param[out] model モデル
        * @return 成否
        */
        bool readBones(PmxModel &model);
    };
}

#endif