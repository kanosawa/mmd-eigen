#include <iostream>
#include <fstream>
#include "MultiCharaCodeFileReader.h"

using namespace std;
using namespace mmd;

MultiCharaCodeFileReader::MultiCharaCodeFileReader(const string code_in)
{
    // 変換記述子の生成
    conv_ = iconv_open("UTF-8", code_in.c_str());
	if (conv_ == (iconv_t)-1) {
		cout << "iconv_open error\n";
		exit(0);
	}
}


MultiCharaCodeFileReader::~MultiCharaCodeFileReader()
{
    iconv_close(conv_);
}


const string MultiCharaCodeFileReader::fread(std::ifstream& fileStream, const int size, const bool needTrans, const bool needResize) const
{
	if (needTrans) {

		char inbuf[BUFSIZ], outbuf[BUFSIZ];
		char *inbuf_p, *outbuf_p;
		inbuf_p = inbuf;
		outbuf_p = outbuf;

		// ファイル入力
		fileStream.read(inbuf_p, size);

		// 文字コード変換
		size_t size_in = size;
		size_t size_out = BUFSIZ;
		iconv(conv_, &inbuf_p, &size_in, &outbuf_p, &size_out);

		string str = outbuf;
		if (needResize) {
			str.resize(BUFSIZ - size_out);
		}
		return str;
	}
	else {
		char charTmp[BUFSIZ];
		fileStream.read(reinterpret_cast<char*>(&charTmp), size);
		string str = charTmp;
		return str;
	}
}