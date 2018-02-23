#include "ModelFileReader.h"

using namespace std;
using namespace mmd;


ModelFileReader::ModelFileReader(const string &filename)
        : fileReader_(filename)
        , pmxHeaderInfo_({2, 2, 2, 2, 2, 2, 2, 2}) {
}


ModelFileReader::~ModelFileReader() {
}



bool ModelFileReader::calcChildBoneIndices(PmxModel &model) {
    // 全ての親ボーンの探索
    vector<unsigned int> parentIndices;
    for (unsigned int boneIndex = 0; boneIndex < model.getBones().size(); ++boneIndex) {
        if (model.getBones()[boneIndex].getParentBoneIndex() == -1) {
            parentIndices.push_back(boneIndex);
        }
    }

    // 子ボーンを登録していく
    searchChildBone(model, parentIndices);

    return true;
}


void ModelFileReader::searchChildBone(PmxModel &model, const vector<unsigned int> &parentBoneIndices) {
    for (unsigned int parent = 0; parent < parentBoneIndices.size(); ++parent) {
        vector<unsigned int> childBoneIndices;
        for (unsigned int child = 0; child < model.getBones().size(); ++child) {
            // もし、あるボーンにparentIndices[i]が親ボーンとして登録されていたら
            if (model.getBones()[child].getParentBoneIndex() == parentBoneIndices[parent]) {
                // そのボーンインデックスを子ボーンインデックスとして親ボーンに登録
                model.pushBackChildBoneIndex(parentBoneIndices[parent], child);
                // 登録した子ボーンは後で親ボーンとして参照するのでvectorに突っ込んでおく
                childBoneIndices.push_back(child);
            }
        }
        // 子ボーンを親ボーンとして再帰的に探索
        searchChildBone(model, childBoneIndices);
    }
}
