#ifndef MMD_PMX_FILE_READER_H
#define MMD_PMX_FILE_READER_H

#include <memory>
#include <fstream>
#include "PmxModel.h"
#include "MultiCharaCodeFileReader.h"

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

        /*! @brief デストラクタ
        */
        ~PmxFileReader();

        /*! @brief PMXモデルファイルの読み込み
        * @return モデル
        */
        unique_ptr<PmxModel> readFile();

    private:
        /*! @brief デフォルトコンストラクタの禁止
        */
        PmxFileReader();

        /*! @brief コピーコンストラクタの禁止
        */
        PmxFileReader(const PmxFileReader &);

        /*! @brief 代入演算子の禁止
        */
        PmxFileReader &operator=(const PmxFileReader &);

        /*! @brief ヘッダの読み込み
        * @param[out] model モデル
        * @return 成否
        */
        bool readHeader(std::unique_ptr<PmxModel> &model);

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
        bool readVertices(std::unique_ptr<PmxModel> &model);

        /*! @brief 面の読み込み
        * @param[out] model モデル
        * @return 成否
        */
        bool readSurfaces(std::unique_ptr<PmxModel> &model);

        /*! @brief テクスチャの読み込み
        * @param[out] model モデル
        * @return 成否
        */
        bool readTextures(std::unique_ptr<PmxModel> &model);

        /*! @brief マテリアルの読み込み
        * @param[out] model モデル
        * @return 成否
        */
        bool readMaterials(std::unique_ptr<PmxModel> &model);

        /*! @brief ボーンの読み込み
        * @param[out] model モデル
        * @return 成否
        */
        bool readBones(std::unique_ptr<PmxModel> &model);

        /*! @brief 子ボーンインデックスの算出
        * @param[in, out] model モデル
        * @return 成否
        */
        bool calcChildBoneIndices(std::unique_ptr<PmxModel> &model);

        /*! @brief 子ボーンの探索（再帰関数）
        * @param[out] model モデル
        * @param[in] parentBoneIndices 親ボーンインデックス
        */
        void searchChildBone(std::unique_ptr<PmxModel> &model, const vector<int> &parentBoneIndices);

        /*! @brief 可変サイズデータのファイル入力(unsigned, リトルエンディアン)
        * @param[in] dataSize データバイト数
        * @return データ
        */
        int readVariableSizeUnsignedData(const int dataSize);

        /*! @brief 可変サイズデータのファイル入力(signed, リトルエンディアン)
        * @param[in] dataSize データバイト数
        * @return データ
        */
        int readVariableSizeSignedData(const int dataSize);

        /*! @brief ファイルストリーム
        */
        std::ifstream fileStream_;

        /*! @brief PMXファイルヘッダ情報
        */
        PmxHeaderInfo pmxHeaderInfo_;
    };
}

#endif