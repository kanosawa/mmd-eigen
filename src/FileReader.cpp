#include <iostream>
#include <math.h>
#include <iconv.h>
#include "FileReader.h"

using namespace mmd;

FileReader::FileReader(const string &filename) {

    size_t pos = filename.rfind('/');
    if (pos != string::npos) {
        folderName_ = filename.substr(0, pos + 1);
    }

    fileStream_.open(filename.c_str(), ios::binary);
    if (!fileStream_) {
        cout << "File Open Error in FileReader()\n";
        exit(0);
    }
}


FileReader::~FileReader() {
}


const string& FileReader::getFolderName() const {
    return folderName_;
}


void FileReader::read(char *data, const int size) {
    fileStream_.read(data, size);
}


const string FileReader::readFromUTF8(const int size)
{
    char charTmp[BUFSIZ];
    fileStream_.read(reinterpret_cast<char *>(&charTmp), size);
    string str = charTmp;
    return str;
}


const string FileReader::readFromUTF16(const int size, const bool needResize)
{
    return trans(size, "UTF-16", needResize);
}


const string FileReader::readFromCP932(const int size, const bool needResize)
{
    return trans(size, "CP932", needResize);
}


int FileReader::readVariableSizeUnsignedData(const int dataSize) {
    int data = 0;
    for (int b = 0; b < dataSize; ++b) {
        unsigned char tmp;
        fileStream_.read(reinterpret_cast<char *>(&tmp), 1);
        data += tmp * static_cast<int>(pow(256, b));
    }
    return data;
}

int FileReader::readVariableSizeSignedData(const int dataSize) {
    int data = readVariableSizeUnsignedData(dataSize);

    int max = static_cast<int>(pow(256, dataSize));
    if (data >= max / 2) {
        data = -(max - data);
    }

    return data;
}


const string FileReader::trans(const int size, const string &code, const bool needResize)
{
    iconv_t conv = iconv_open("UTF-8", code.c_str());

    char inbuf[BUFSIZ], outbuf[BUFSIZ];
    char *inbuf_p, *outbuf_p;
    inbuf_p = inbuf;
    outbuf_p = outbuf;

    // ファイル入力
    fileStream_.read(inbuf_p, size);

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