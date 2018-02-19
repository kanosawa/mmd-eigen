#include "PmdFileReader.h"

using namespace std;
using namespace mmd;


PmdFileReader::PmdFileReader(const string &filename)
        : fileReader_(filename) {
}


PmdFileReader::~PmdFileReader() {
}


void PmdFileReader::readFile(PmxModel& model) {

    // ヘッダの読み込み
    if (!readHeader(model)) {
        cout << "PmdFileReader::readFile() : Header Error\n";
        exit(0);
    }

    // モデル情報の読み込み
    if (!readModelInfo()) {
        cout << "PmxFileReader::readFile() : ModelInfo Error\n";
        exit(0);
    }
}


bool PmdFileReader::readHeader(PmxModel &model) {
    // "Pmd"の確認(ASCII)
    const int PMD_LENGTH = 3;
    char charPmd[PMD_LENGTH];

    fileReader_.read(reinterpret_cast<char *>(charPmd), PMD_LENGTH);
    if (strncmp(charPmd, "Pmd", PMD_LENGTH) != 0) {
        return false;
    }

    // バージョンの確認
    float pmdVersion;
    fileReader_.read(reinterpret_cast<char *>(&pmdVersion), 4);
    if (pmdVersion != 1) return false;

    // データ列のバイトサイズの確認
    unsigned char byteSize;
    fileReader_.read(reinterpret_cast<char *>(&byteSize), 1);
    if (byteSize != 8) return false;

    // エンコード方式
    fileReader_.read(reinterpret_cast<char *>(&pmdHeaderInfo_.encodeType), 1);
    if (pmdHeaderInfo_.encodeType != 0 && pmdHeaderInfo_.encodeType != 1) return false;

    // 追加UV数
    fileReader_.read(reinterpret_cast<char *>(&pmdHeaderInfo_.appendixUVNum), 1);
    if (pmdHeaderInfo_.appendixUVNum != 0) {
        std::cout << "Thie code does not support appndix uv\n";
        return false;
    }

    return true;
}


bool PmdFileReader::readModelInfo() {
    fileReader_.readFromUTF8(20); // モデル名
    fileReader_.readFromUTF8(256); // コメント
    return true;
}
