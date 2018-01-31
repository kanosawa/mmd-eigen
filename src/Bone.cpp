#include <Eigen/Dense>
#include "Bone.h"

using namespace mmd;

Bone::Bone()
        : quaternion_(Eigen::Quaternionf(1.0f, 0.0f, 0.0f, 0.0f)) {
}


Bone::Bone(const Bone &bone) {
    copy(bone);
}


Bone::~Bone() {
}


Bone &Bone::operator=(const Bone &bone) {
    copy(bone);
    return *this;
}


void Bone::copy(const Bone &bone) {
    boneName_ = bone.boneName_;
    position_ = bone.position_;
    quaternion_ = bone.quaternion_;
    parentBoneIndex_ = bone.parentBoneIndex_;
    childBoneIndices_ = bone.childBoneIndices_;
    destinationFlag_ = bone.destinationFlag_;
    offset_ = bone.offset_;
    destinationBoneIndex_ = bone.destinationBoneIndex_;

    position_tmp_ = bone.position_tmp_;
    quaternion_tmp_ = bone.quaternion_tmp_;
}


void Bone::setBoneName(const string &boneName) {
    boneName_ = boneName;
}


const string &Bone::getBoneName() const {
    return boneName_;
}


void Bone::setPosition(const Eigen::Vector3f &position) {
    position_ = position;
}


const Eigen::Vector3f &Bone::getPosition() const {
    return position_;
}


void Bone::setQuartanion(const Eigen::Quaternionf &quaternion) {
    quaternion_ = quaternion;
}


const Eigen::Quaternionf &Bone::getQuaternion() const {
    return quaternion_;
}


void Bone::setParentBoneIndex(const int parentBoneIndex) {
    parentBoneIndex_ = parentBoneIndex;
}


const int Bone::getParentBoneIndex() const {
    return parentBoneIndex_;
}


void Bone::pushBackChildBoneIndex(const int childBoneIndex) {
    childBoneIndices_.push_back(childBoneIndex);
}


const vector<int> &Bone::getChildBoneIndices() const {
    return childBoneIndices_;
}


void Bone::setDestinationFlag(const bool &destinationFlag) {
    destinationFlag_ = destinationFlag;
}


const bool Bone::getDestinationFlag() const {
    return destinationFlag_;
}


void Bone::setOffset(const Eigen::Vector3f &offset) {
    offset_ = offset;
}


const Eigen::Vector3f &Bone::getOffset() const {
    return offset_;
}


void Bone::setDestinationBoneIndex(const int &destinationBoneIndex) {
    destinationBoneIndex_ = destinationBoneIndex;
}


int Bone::getDestinationBoneIndex() {
    return destinationBoneIndex_;
}

