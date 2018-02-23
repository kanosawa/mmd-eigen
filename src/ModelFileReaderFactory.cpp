#include "ModelFileReaderFactory.h"
#include "PmxModelFileReader.h"
#include "PmdModelFileReader.h"

using namespace mmd;

ModelFileReaderFactory::ModelFileReaderFactory() {
}


ModelFileReaderFactory::~ModelFileReaderFactory() {
}


unique_ptr<ModelFileReader> ModelFileReaderFactory::create(const string &filename) {
    // 拡張子取得
    auto ext_pos = filename.find_last_of(".");
    std::string ext_name = filename.substr(ext_pos, filename.size() - ext_pos);

    if (ext_name == ".pmx") {
        return make_unique<PmxModelFileReader>(filename);
    } else if (ext_name == ".pmd") {
        return make_unique<PmdModelFileReader>(filename);
    } else {
        cout << "file extension is something wrong\n";
    }

    return nullptr;
}



