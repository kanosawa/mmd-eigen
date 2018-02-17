#ifndef MMD_FILE_READER_H
#define MMD_FILE_READER_H

#include <string>
#include <fstream>

using namespace std;

namespace mmd {
    /*! @brief ファイル入力クラス
    */
    class FileReader {
    public:
        /*! @brief コンストラクタ
        * @param[in] filename
        */
        FileReader(const string &filename);

        ~FileReader();

        const string& getFolderName() const;

        /*! @brief ファイル入力
        * @param[out] data データ
        * @param[in]  size データサイズ
        */
        void read(char *data, const int size);

        /*! @brief UTF8ファイル入力
        * @param[in] size 入力バイト数
        * @return ファイルから入力した文字列
        */
        const string readFromUTF8(const int size);

        /*! @brief UTF16ファイル入力
        * @param[in] size 入力バイト数
        * @return ファイルから入力した文字列
        */
        const string readFromUTF16(const int size, const bool needResize = true);

        /*! @brief CP932ファイル入力
        * @param[in] size 入力バイト数
        * @return ファイルから入力した文字列
        */
        const string readFromCP932(const int size, const bool needResize = true);

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

    private:
        /*! @brief デフォルトコンストラクタの禁止
        */
        FileReader();

        /*! @brief 文字コード変換
        * @param[in] size 入力バイト数
        * @param[in] code 入力文字コード
        * @param[in] needResize リサイズするか否か
        * @return ファイルから入力した文字列
        */
        const string trans(const int size, const string &code, const bool needResize);

        /*! @brief ファイルストリーム
        */
        std::ifstream fileStream_;

        /*! @brief フォルダ名
        */
        string folderName_;
    };
}

#endif