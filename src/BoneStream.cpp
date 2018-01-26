#include "BoneStream.h"

using namespace mmd;

BoneStream::BoneStream() {
}


BoneStream::~BoneStream() {
}


void BoneStream::pushBackBones(const int frameNo, const vector<Bone> &bones) {
    boneListMap_[frameNo] = bones;
}


bool BoneStream::getBones(vector<Bone> &bones, const int frameNo) const {
    auto itr = boneListMap_.find(frameNo);
    if (itr == boneListMap_.end()) {
        return false;
    }
    bones = itr->second;
    return true;
}

const map<int, vector<Bone>> &BoneStream::getBoneListMap() const {
    return boneListMap_;
}