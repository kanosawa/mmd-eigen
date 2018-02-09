#include "Vertex.h"

using namespace mmd;

Vertex::Vertex() {
}


Vertex::Vertex(const Eigen::Vector3f &initialPosition, const Eigen::Vector2f &uv,
               const vector<int> &refBoneIndices, const vector<float> &refBoneWeightList)
        : initialPosition_(initialPosition), uv_(uv), refBoneIndices_(refBoneIndices),
          refBoneWeightList_(refBoneWeightList) {
}


Vertex::~Vertex() {
}


void Vertex::setTemporalPosition(const Eigen::Vector3f &temporalPosition) {
    temporalPosition_ = temporalPosition;
}


const Eigen::Vector3f &Vertex::getInitialPosition() const {
    return initialPosition_;
}


const Eigen::Vector3f &Vertex::getTemporalPosition() const {
    return temporalPosition_;
}


const Eigen::Vector2f &Vertex::getUv() const {
    return uv_;
}


const vector<int> &Vertex::getRefBoneIndices() const {
    return refBoneIndices_;
}


const vector<float> &Vertex::getRefBoneWeightList() const {
    return refBoneWeightList_;
}
