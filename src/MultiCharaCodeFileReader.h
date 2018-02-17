#ifndef MMD_MULTI_CHARA_CODE_FILE_READER_H
#define MMD_MULTI_CHARA_CODE_FILE_READER_H

#include <string>
#include <iconv.h>

using namespace std;

namespace mmd {
    /*! @brief ファイル入力
    * @param[in] fileStream ファイルストリーム
    * @param[in] size 入力バイト数
    * @param[in] encodeType エンコードタイプ
    * @return ファイルから入力した文字列
    */
    const string readFromUTF(ifstream &fileStream, const int size, const char encodeType);

    /*! @brief UTF8ファイル入力
    * @param[in] fileStream ファイルストリーム
    * @param[in] size 入力バイト数
    * @return ファイルから入力した文字列
    */
    const string readFromUTF8(ifstream &fileStream, const int size);

    /*! @brief UTF16ファイル入力
    * @param[in] fileStream ファイルストリーム
    * @param[in] size 入力バイト数
    * @return ファイルから入力した文字列
    */
    const string readFromUTF16(ifstream &fileStream, const int size);

    /*! @brief CP932ファイル入力
    * @param[in] fileStream ファイルストリーム
    * @param[in] size 入力バイト数
    * @return ファイルから入力した文字列
    */
    const string readFromCP932(ifstream &fileStream, const int size);
}

#endif