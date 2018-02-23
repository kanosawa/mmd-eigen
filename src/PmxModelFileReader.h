#ifndef MMD_PMX_MODEL_FILE_READER_H
#define MMD_PMX_MODEL_FILE_READER_H

#include "ModelFileReader.h"

namespace mmd {
    /*! @brief PMXファイル読み込みクラス
    */
    class PmxModelFileReader : ModelFileReader {
    public:
        /*! @brief コンストラクタ
        * @param[in] filename
        */
        PmxModelFileReader(const string &filename);

        ~PmxModelFileReader();

        /*! @brief PMXモデルファイルの読み込み
        * @return モデル
        */
        void readFile(PmxModel& model);

    private:
        /*! @brief デフォルトコンストラクタの禁止
        */
        PmxModelFileReader();

        /*! @brief ヘッダの読み込み
        * @param[out] model モデル
        * @return 成否
        */
        bool readHeader(PmxModel &model);

        /*! @brief 全インデックスサイズの読み込み
        * @return 成否
        */
        bool readAllIndexSize();

        /*! @brief 各インデックスサイズの読み込み
        * @param[out] indexSize インデックスサイズ
        * @return 成否
        */
        bool readIndexSize(unsigned char &indexSize);

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

        /*! @brief テクスチャの読み込み
        * @param[out] model モデル
        * @return 成否
        */
        bool readTextures(PmxModel &model);

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

        /*! @brief ファイル入力
        * @param[in] size 入力バイト数
        * @param[in] encodeType エンコードタイプ
        * @return ファイルから入力した文字列
        */
        const string readFromUTF(const int size, const char encodeType);
    };
}

#endif