#include "Motion.h"

using namespace mmd;

Motion::Motion() {
}


Motion::Motion(const int boneIndex, const int frameNo, const Eigen::Vector3f &shift,
               const Eigen::Quaternionf &quaternion)
        : boneIndex_(boneIndex), frameNo_(frameNo), shift_(shift), quaternion_(quaternion) {
}


Motion::~Motion() {
}


void Motion::setQuaternion(const Eigen::Quaternionf& quaternion) {
    quaternion_ = quaternion;
}


const int Motion::getBoneIndex() const {
    return boneIndex_;
}


const int Motion::getFrameNo() const {
    return frameNo_;
}


const Eigen::Vector3f& Motion::getShift() const {
    return shift_;
}


const Eigen::Quaternionf& Motion::getQuaternion() const {
    return quaternion_;
}