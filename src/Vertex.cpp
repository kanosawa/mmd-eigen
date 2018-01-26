#include "Vertex.h"

using namespace mmd;

Vertex::Vertex() {
}


Vertex::Vertex(const Eigen::Vector3f &position, const Eigen::Vector2f &uv,
               const vector<int> &refBoneIndices, const vector<float> &refBoneWeightList)
        : position_(position), uv_(uv), refBoneIndices_(refBoneIndices), refBoneWeightList_(refBoneWeightList) {
}


Vertex::Vertex(const Vertex &vertex) {
    copy(vertex);
}


Vertex::~Vertex() {
}


Vertex &Vertex::operator=(const Vertex &vertex) {
    copy(vertex);
    return *this;
}


void Vertex::copy(const Vertex &vertex) {
    position_ = vertex.position_;
    uv_ = vertex.uv_;
    refBoneIndices_ = vertex.refBoneIndices_;
    refBoneWeightList_ = vertex.refBoneWeightList_;
}


void Vertex::setPosition(const Eigen::Vector3f &position) {
    position_ = position;
}


const Eigen::Vector3f &Vertex::getPosition() const {
    return position_;
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
