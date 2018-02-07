#include "MotionStream.h"

using namespace mmd;

MotionStream::MotionStream(const vector<Motion> &motions) {
    motions_ = motions;
    pointer_ = 0;
}


MotionStream::~MotionStream() {
}


void MotionStream::incrementPointer() {
    ++pointer_;
}


const Motion& MotionStream::getLatestMotion() const {
    return motions_[pointer_];
}


const Motion& MotionStream::getNextMotion() const {
    return motions_[pointer_ + 1];
}


bool MotionStream::isLastMotion() {
    if (pointer_ != motions_.size() - 1) {
        return false;
    }
    return true;
}