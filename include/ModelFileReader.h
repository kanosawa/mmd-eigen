#ifndef MMD_MODEL_FILE_READER_H
#define MMD_MODEL_FILE_READER_H

#include "FileReader.h"
#include "PmxModel.h"

namespace mmd {
    /*! @brief モデルファイル読み込みクラス
    */
    class ModelFileReader {
    public:
        /*! @brief ヘッダ情報
        */
        struct HeaderInfo {

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

        /*! @brief コンストラクタ
        * @param[in] filename
        */
        ModelFileReader(const string &filename);

        virtual ~ModelFileReader();

        /*! @brief モデルファイルの読み込み
        * @return モデル
        */
        virtual void readFile(PmxModel &model) = 0;

    protected:
        /*! @brief 子ボーンインデックスの算出
        * @param[in, out] model モデル
        * @return 成否
        */
        bool calcChildBoneIndices(PmxModel &model);

        /*! @brief 子ボーンの探索（再帰関数）
        * @param[out] model モデル
        * @param[in] parentBoneIndices 親ボーンインデックス
        */
        void searchChildBone(PmxModel &model, const vector<unsigned int> &parentBoneIndices);

        /*! @brief ファイル入力クラス
        */
        FileReader fileReader_;

        /*! @brief PMXファイルヘッダ情報
        */
        HeaderInfo headerInfo_;
    };
}

#endif