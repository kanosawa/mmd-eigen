#ifndef MMD_VMD_FILE_READER_H
#define MMD_VMD_FILE_READER_H

#include <memory>
#include <fstream>
#include "Bone.h"
#include "Motion.h"
#include "FileReader.h"

namespace mmd {
    /*! @brief VMDファイル読み込みクラス
    */
    class VmdFileReader {
    public:
        /*! @brief コンストラクタ
        * @param[in] filename VMDファイル名
        * @param[in] 初期ボーン
        */
        VmdFileReader(const string &filename, const vector<Bone> &initialBones);

        /*! @brief デストラクタ
        */
        ~VmdFileReader();

        /*! @brief VMDファイルの読み込み
        * @param[out] motions VMDデータストリーム
        */
        void readFile(vector<Motion>& motions);

    private:
        /*! @brief デフォルトコンストラクタの禁止
        */
        VmdFileReader();

        /*! @brief ヘッダの読み込み
        * @param[out] vmdDataStream VMDデータストリーム
        * @return 成否
        */
        bool readHeader();

        /*! @brief ファイル入力クラス
        */
        FileReader fileReader_;

        /*! @brief ボーン名リスト
        */
        vector<string> boneNames_;

        /*! @brief モーションデータ数
        */
        unsigned int motionDataNum_;
    };
}

#endif