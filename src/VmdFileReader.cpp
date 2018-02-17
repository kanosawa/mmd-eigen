#include <iostream>
#include "VmdFileReader.h"
#include "Motion.h"

using namespace mmd;
using namespace std;

VmdFileReader::VmdFileReader(const string &filename, const vector<Bone> &initialBones)
        : fileReader_(filename){

    // ボーン名の登録
    for (unsigned int i = 0; i < initialBones.size(); ++i) {
        boneNames_.push_back(initialBones[i].getBoneName());
    }
}


VmdFileReader::~VmdFileReader() {
}


void VmdFileReader::readFile(vector<Motion>& motions) {

    // ヘッダの読み込み
    if (!readHeader()) {
        cout << "VmdFileReader::readFile() : Header Error\n";
        exit(0);
    }

    // フレームデータの読み込み
    const int BONE_NAME_LENGTH = 15;
    for (unsigned int i = 0; i < motionDataNum_; ++i) {
        // ボーン名
        string boneName = fileReader_.readFromCP932(BONE_NAME_LENGTH, false);

        // ボーンインデックスの算出
        int boneIndex = distance(boneNames_.begin(), find(boneNames_.begin(), boneNames_.end(), boneName));

        // フレーム番号
        unsigned int frameNo;
        fileReader_.read(reinterpret_cast<char *>(&frameNo), 4);

        // ボーンのシフト
        Eigen::Vector3f shift;
        fileReader_.read(reinterpret_cast<char *>(&shift), 12);

        // ボーンのクォータニオン
        Eigen::Vector4f quatTmp;
        fileReader_.read(reinterpret_cast<char *>(&quatTmp), 16);
        Eigen::Quaternionf quaternion(quatTmp[3], quatTmp[0], quatTmp[1], quatTmp[2]);

        // 補間パラメータ
        char param[64];
        fileReader_.read(reinterpret_cast<char *>(&param), 64);

        // VMDデータストリームへの挿入
        if (boneIndex != static_cast<int>(boneNames_.size())) {
            motions.push_back(Motion(boneIndex, frameNo, shift, quaternion));
        }
    }
}


bool VmdFileReader::readHeader() {
    // "Vocaloid Motion Data 0002\n"の確認
    const int VMD_LENGTH = 30;
    char charVmd[VMD_LENGTH];
    fileReader_.read(reinterpret_cast<char *>(charVmd), VMD_LENGTH);
    if (strncmp(charVmd, "Vocaloid Motion Data 0002\n", 25) != 0) {
        return false;
    }

    // モデル名
    const int MODEL_NAME_LENGTH = 20;
    string modelName = fileReader_.readFromCP932(MODEL_NAME_LENGTH, false);

    // モーションデータ数
    fileReader_.read(reinterpret_cast<char *>(&motionDataNum_), 4);

    return true;
}