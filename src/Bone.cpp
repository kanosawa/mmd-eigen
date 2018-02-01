#include <Eigen/Dense>
#include "Bone.h"

using namespace mmd;

Bone::Bone() {
}


Bone::~Bone() {
}


void Bone::setBoneName(const string &boneName) {
    boneName_ = boneName;
}


const string &Bone::getBoneName() const {
    return boneName_;
}


void Bone::setInitialPosition(const Eigen::Vector3f &initial_position) {
    initial_position_ = initial_position;
}


const Eigen::Vector3f &Bone::getInitialPosition() const {
    return initial_position_;
}


void Bone::setTemporalPosition(const Eigen::Vector3f &temporal_position) {
    temporal_position_ = temporal_position;
}


const Eigen::Vector3f &Bone::getTemporalPosition() const {
    return temporal_position_;
}


void Bone::setTemporalQuartanion(const Eigen::Quaternionf &temporal_quaternion) {
    temporal_quaternion_ = temporal_quaternion;
}


const Eigen::Quaternionf &Bone::getTemporalQuaternion() const {
    return temporal_quaternion_;
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

