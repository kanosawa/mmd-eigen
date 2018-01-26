#include <iostream>
#include "VmdFileReader.h"

using namespace mmd;
using namespace std;

VmdFileReader::VmdFileReader(const string &filename, const vector<Bone> &initialBones) {
    fileStream_.open(filename.c_str(), ios::binary);
    if (!fileStream_) {
        cout << "File Open Error in VmdFileReader()\n";
        exit(0);
    }

    boneIndexCalculator_ = make_unique<BoneIndexCalculator>(initialBones);
}


VmdFileReader::~VmdFileReader() {
}


std::unique_ptr<VmdDataStream> VmdFileReader::readFile() {
    // VMDデータストリームの生成
    std::unique_ptr<VmdDataStream> vmdDataStream = make_unique<VmdDataStream>();

    // ヘッダの読み込み
    if (!readHeader(vmdDataStream)) {
        cout << "VmdFileReader::readFile() : Header Error\n";
        exit(0);
    }

    // フレームデータの読み込み
    MultiCharaCodeFileReader multiCharaCodeFileReader("CP932");
    const int BONE_NAME_LENGTH = 15;
    for (unsigned int i = 0; i < frameDataNum_; ++i) {
        // ボーン名
        string boneName = multiCharaCodeFileReader.fread(fileStream_, BONE_NAME_LENGTH, true, false);

        // ボーンインデックスの算出
        int index;
        bool res = boneIndexCalculator_->process(index, boneName);

        // フレーム番号
        unsigned int frameNo;
        fileStream_.read(reinterpret_cast<char *>(&frameNo), 4);

        // ボーンの位置
        Eigen::Vector3f pos;
        fileStream_.read(reinterpret_cast<char *>(&pos), 12);

        // ボーンのクォータニオン
        Eigen::Vector4f quatTmp;
        fileStream_.read(reinterpret_cast<char *>(&quatTmp), 16);
        Eigen::Quaternionf quaternion(quatTmp[3], quatTmp[0], quatTmp[1], quatTmp[2]);

        // 補間パラメータ
        char param[64];
        fileStream_.read(reinterpret_cast<char *>(&param), 64);

        // VMDデータストリームへの挿入
        if (res && frameNo == 0) {
            VmdDataStream::BoneInfo boneInfo = {boneName, index, pos, quaternion};
            vmdDataStream->insertBoneInfoList(frameNo, boneInfo);
        }
    }

    /*
    multimap<int, VmdDataStream::BoneInfo> boneInfoListMap = vmdDataStream->getBoneInfoList();
    auto range = boneInfoListMap.equal_range(0);
    for (auto itr = range.first; itr != range.second; ++itr) {
        cout << itr->first << "," << itr->second.boneIndex << "," << itr->second.boneName << endl;
        cout << itr->second.shift << endl;
        cout << itr->second.quarternion.x() << "," << itr->second.quarternion.y() << ",";
        cout << itr->second.quarternion.z() << "," << itr->second.quarternion.w() << endl << endl;
    }
    */

    return std::move(vmdDataStream);
}


bool VmdFileReader::readHeader(std::unique_ptr<VmdDataStream> &vmdDataStream) {
    // "Vocaloid Motion Data 0002\n"の確認
    const int VMD_LENGTH = 30;
    unsigned char charVmd[VMD_LENGTH];
    fileStream_.read(reinterpret_cast<char *>(charVmd), VMD_LENGTH);

    if ((charVmd[0] != 0x56) || (charVmd[1] != 0x6f) || (charVmd[2] != 0x63) || (charVmd[3] != 0x61) ||
        (charVmd[4] != 0x6c) || (charVmd[5] != 0x6f) || (charVmd[6] != 0x69) || (charVmd[7] != 0x64) ||
        (charVmd[8] != 0x20) || (charVmd[9] != 0x4d) || (charVmd[10] != 0x6f) || (charVmd[11] != 0x74) ||
        (charVmd[12] != 0x69) || (charVmd[13] != 0x6f) || (charVmd[14] != 0x6e) || (charVmd[15] != 0x20) ||
        (charVmd[16] != 0x44) || (charVmd[17] != 0x61) || (charVmd[18] != 0x74) || (charVmd[19] != 0x61) ||
        (charVmd[20] != 0x20) || (charVmd[21] != 0x30) || (charVmd[22] != 0x30) || (charVmd[23] != 0x30) ||
        (charVmd[24] != 0x32))
        return false;

    // モデル名
    const int MODEL_NAME_LENGTH = 20;
    unsigned char modelName[MODEL_NAME_LENGTH];
    fileStream_.read(reinterpret_cast<char *>(modelName), MODEL_NAME_LENGTH);

    // フレームデータ数
    fileStream_.read(reinterpret_cast<char *>(&frameDataNum_), 4);
    cout << "frameDataNum = " << frameDataNum_ << endl;

    return true;
}