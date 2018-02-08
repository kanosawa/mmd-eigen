#include "Vertex.h"

using namespace mmd;

Vertex::Vertex() {
}


Vertex::Vertex(const Eigen::Vector3f &initial_position, const Eigen::Vector2f &uv,
               const vector<int> &refBoneIndices, const vector<float> &refBoneWeightList)
        : initial_position_(initial_position), uv_(uv), refBoneIndices_(refBoneIndices), refBoneWeightList_(refBoneWeightList) {
}


Vertex::~Vertex() {
}


void Vertex::setInitialPosition(const Eigen::Vector3f &initial_position) {
    initial_position_ = initial_position;
}


const Eigen::Vector3f &Vertex::getInitialPosition() const {
    return initial_position_;
}


void Vertex::setTemporalPosition(const Eigen::Vector3f &temporal_position) {
    temporal_position_ = temporal_position;
}


const Eigen::Vector3f &Vertex::getTemporalPosition() const {
    return temporal_position_;
}


void Vertex::setUv(const Eigen::Vector2f &uv) {
    uv_ = uv;
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
