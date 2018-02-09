#include "PmxModel.h"

using namespace mmd;

PmxModel::PmxModel() {
}


PmxModel::~PmxModel() {
}


void PmxModel::pushBackVertex(const Vertex &vertex) {
    vertices_.push_back(vertex);
}


void PmxModel::pushBackSurface(const Surface &surface) {
    surfaces_.push_back(surface);
}


void PmxModel::pushBackTexture(const cv::Mat &texture) {
    textures_.push_back(texture);
}


void PmxModel::pushBackMaterial(const Material &material) {
    materials_.push_back(material);
}


void PmxModel::pushBackBone(const Bone &bone) {
    bones_.push_back(bone);
}


const vector<Vertex> &PmxModel::getVertices() const {
    return vertices_;
}


const vector<Surface> &PmxModel::getSurfaces() const {
    return surfaces_;
}


const vector<cv::Mat> &PmxModel::getTextures() const {
    return textures_;
}


const vector<Material> &PmxModel::getMaterials() const {
    return materials_;
}


const vector<Bone> &PmxModel::getBones() const {
    return bones_;
}


void PmxModel::pushBackChildBoneIndex(const int boneIndex, const int childBoneIndex) {
    bones_[boneIndex].pushBackChildBoneIndex(childBoneIndex);
}


void PmxModel::setBoneTemporalPosition(const int boneIndex, const Eigen::Vector3f &temporalPosition) {
    bones_[boneIndex].setTemporalPosition(temporalPosition);
}


void PmxModel::setBoneTemporalQuaternion(const int boneIndex, const Eigen::Quaternionf &temporalQuaternion) {
    bones_[boneIndex].setTemporalQuaternion(temporalQuaternion);
}


void PmxModel::setVertexTemporalPosition(const int vertexIndex, const Eigen::Vector3f &temporalPosition) {
    vertices_[vertexIndex].setTemporalPosition(temporalPosition);
}
