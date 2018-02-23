#ifndef MMD_PMX_FILE_READER_H
#define MMD_PMX_FILE_READER_H

#include "FileReader.h"
#include "PmxModel.h"

namespace mmd {
    /*! @brief PMXファイル読み込みクラス
    */
    class PmxFileReader {
        /*! @brief PMXモデルファイルのヘッダ情報
        */
        struct PmxHeaderInfo {

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
        PmxFileReader(const string &filename);

        ~PmxFileReader();

        /*! @brief PMXモデルファイルの読み込み
        * @return モデル
        */
        void readFile(PmxModel& model);

    private:
        /*! @brief デフォルトコンストラクタの禁止
        */
        PmxFileReader();

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

        /*! @brief ファイル入力
        * @param[in] size 入力バイト数
        * @param[in] encodeType エンコードタイプ
        * @return ファイルから入力した文字列
        */
        const string readFromUTF(const int size, const char encodeType);

        /*! @brief ファイル入力クラス
        */
        FileReader fileReader_;

        /*! @brief PMXファイルヘッダ情報
        */
        PmxHeaderInfo pmxHeaderInfo_;
    };
}

#endif