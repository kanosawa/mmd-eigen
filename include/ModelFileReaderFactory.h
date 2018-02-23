#ifndef MMD_MODEL_FILE_READER_FACTORY_H
#define MMD_MODEL_FILE_READER_FACTORY_H

#include <memory>
#include "ModelFileReader.h"

using namespace std;

namespace mmd {
    /*! @brief モデルファイル読み込みクラスのファクトリークラス
    */
    class ModelFileReaderFactory {
    public:
        ModelFileReaderFactory();
        ~ModelFileReaderFactory();

        /*! @brief 全ての親ボーンを探索
         * @param[in] filename モデルファイル名
         * @return モデルファイル読み込みクラス
        */
        unique_ptr<ModelFileReader> create(const string &filename);
    };
}

#endif