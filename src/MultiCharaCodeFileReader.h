#ifndef MMD_MULTI_CHARA_CODE_FILE_READER_H
#define MMD_MULTI_CHARA_CODE_FILE_READER_H

#include <string>
#include <iconv.h>

using namespace std;

namespace mmd {
    const string readFromUTF(ifstream &fileStream, const int size, const char encodeType, const bool needResize = true);

    /*! @brief ファイル入力
    * @param[in] fileStream ファイルストリーム
    * @param[in] size 入力バイト数
    * @return ファイルから入力した文字列
    */
    const string readFromUTF8(ifstream &fileStream, const int size);

    /*! @brief ファイル入力
    * @param[in] fileStream ファイルストリーム
    * @param[in] size 入力バイト数
    * @param[in] needResize リサイズが必要か
    * @return ファイルから入力した文字列
    */
    const string readFromUTF16(ifstream &fileStream, const int size, const bool needResize);

    /*! @brief ファイル入力
    * @param[in] fileStream ファイルストリーム
    * @param[in] size 入力バイト数
    * @param[in] needResize リサイズが必要か
    * @return ファイルから入力した文字列
    */
    const string readFromCP932(ifstream &fileStream, const int size, const bool needResize);
}

#endif