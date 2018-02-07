#include "MotionStream.h"

using namespace mmd;

MotionStream::MotionStream() {
    pointer_ = 0;
}


MotionStream::~MotionStream() {

}


void MotionStream::setMotions(const vector<Motion> &motions) {
    motions_ = motions;
}


const vector<Motion>& MotionStream::getMotions() {
    return motions_;
}


const int MotionStream::getPointer() const {
    return pointer_;
}


void MotionStream::incrementPointer() {
    pointer_++;
}


const Motion& MotionStream::getLatestMotion() const {
    return motions_[pointer_];
}


const Motion& MotionStream::getNextMotion() const {
    return motions_[pointer_ + 1];
}