#include <iostream>
#include <fstream>
#include "MultiCharaCodeFileReader.h"

using namespace std;

const string trans(ifstream& fileStream, const int size, const bool needResize, const string code)
{
    iconv_t conv = iconv_open("UTF-8", code.c_str());

    char inbuf[BUFSIZ], outbuf[BUFSIZ];
    char *inbuf_p, *outbuf_p;
    inbuf_p = inbuf;
    outbuf_p = outbuf;

    // ファイル入力
    fileStream.read(inbuf_p, size);

    // 文字コード変換
    size_t size_in = size;
    size_t size_out = BUFSIZ;
    iconv(conv, &inbuf_p, &size_in, &outbuf_p, &size_out);

    string str = outbuf;
    if (needResize) {
        str.resize(BUFSIZ - size_out);
    }

    iconv_close(conv);

    return str;
}


const string mmd::readFromUTF(ifstream &fileStream, const int size, const char encodeType)
{
    if (encodeType == 0) {
        return readFromUTF16(fileStream, size);
    } else {
        return readFromUTF8(fileStream, size);
    }
}


const string mmd::readFromUTF8(ifstream &fileStream, const int size)
{
    char charTmp[BUFSIZ];
    fileStream.read(reinterpret_cast<char *>(&charTmp), size);
    string str = charTmp;
    return str;
}


const string mmd::readFromUTF16(ifstream &fileStream, const int size)
{
    return trans(fileStream, size, true, "UTF-16");
}


const string mmd::readFromCP932(ifstream &fileStream, const int size)
{
    return trans(fileStream, size, false, "CP932");
}
