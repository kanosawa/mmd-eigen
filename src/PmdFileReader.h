#ifndef MMD_PMD_FILE_READER_H
#define MMD_PMD_FILE_READER_H

#include "FileReader.h"
#include "PmxModel.h"

namespace mmd {
    /*! @brief PMDファイル読み込みクラス
    */
    class PmdFileReader {
        /*! @brief PMDモデルファイルのヘッダ情報
        */
        struct PmdHeaderInfo {

            /*! @brief エンコードタイプ
            * 0 : utf-16, 1 : utf-8
            */
            unsigned char encodeType;

            /*! @brief 追加UV数
            */
            unsigned char appendixUVNum;

            /*! @brief 頂点インデックスサイズ
            */
            unsigned char vertexIndexSize;

            /*! @brief テクスチャインデックスサイズ
            */
            unsigned char textureIndexSize;

            /*! @brief 材質インデックスサイズ
            */
            unsigned char materialIndexSize;

            /*! @brief ボーンインデックスサイズ
            */
            unsigned char boneIndexSize;

            /*! @brief モーフインデックスサイズ
            */
            unsigned char morphIndexSize;

            /*! @brief 剛体インデックスサイズ
            */
            unsigned char rigidbodyIndexSize;
        };

    public:
        /*! @brief コンストラクタ
        * @param[in] filename
        */
        PmdFileReader(const string &filename);

        ~PmdFileReader();

        /*! @brief PMDモデルファイルの読み込み
        * @return モデル
        */
        void readFile(PmxModel& model);

    private:
        /*! @brief デフォルトコンストラクタの禁止
        */
        PmdFileReader();

        /*! @brief ヘッダの読み込み
        * @param[out] model モデル
        * @return 成否
        */
        bool readHeader(PmxModel &model);

        /*! @brief モデル情報の読み込み
        * @return 成否
        */
        bool readModelInfo();

        /*! @brief ファイル入力クラス
        */
        FileReader fileReader_;

        /*! @brief PMDファイルヘッダ情報
        */
        PmdHeaderInfo pmdHeaderInfo_;
    };
}

#endif