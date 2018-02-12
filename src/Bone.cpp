#include "Bone.h"

using namespace mmd;


Bone::Bone() {
}


Bone::~Bone() {
}


Bone::Bone(const string &boneName, const Eigen::Vector3f &initialPosition, const int parentBoneIndex)
        : boneName_(boneName), initialPosition_(initialPosition), parentBoneIndex_(parentBoneIndex) {
}


void Bone::setDestinationFlag(const bool destinationFlag) {
    destinationFlag_ = destinationFlag;
}


void Bone::setOffset(const Eigen::Vector3f &offset) {
    offset_ = offset;
}


void Bone::setDestinationBoneIndex(const int destinationBoneIndex) {
    destinationBoneIndex_ = destinationBoneIndex;
}


void Bone::setIKFlag(const bool IKFlag) {
    IKFlag_ = IKFlag;
}


void Bone::setIK(const Bone::IK &ik) {
    ik_ = ik;
}


void Bone::setTemporalPosition(const Eigen::Vector3f &temporalPosition) {
    temporalPosition_ = temporalPosition;
}


void Bone::setTemporalQuaternion(const Eigen::Quaternionf &temporalQuaternion) {
    temporalQuaternion_ = temporalQuaternion;
}


const string &Bone::getBoneName() const {
    return boneName_;
}


const Eigen::Vector3f &Bone::getInitialPosition() const {
    return initialPosition_;
}


const int Bone::getParentBoneIndex() const {
    return parentBoneIndex_;
}


const bool Bone::getDestinationFlag() const {
    return destinationFlag_;
}


const Eigen::Vector3f &Bone::getOffset() const {
    return offset_;
}


const int Bone::getDestinationBoneIndex() const {
    return destinationBoneIndex_;
}


const vector<int> &Bone::getChildBoneIndices() const {
    return childBoneIndices_;
}


const bool Bone::getIKFlag() const {
    return IKFlag_;
}


const Bone::IK &Bone::getIK() const {
    return ik_;
}


const Eigen::Vector3f &Bone::getTemporalPosition() const {
    return temporalPosition_;
}


const Eigen::Quaternionf &Bone::getTemporalQuaternion() const {
    return temporalQuaternion_;
}


void Bone::pushBackChildBoneIndex(const int childBoneIndex) {
    childBoneIndices_.push_back(childBoneIndex);
}