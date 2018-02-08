#include "PmxModel.h"

using namespace mmd;

PmxModel::PmxModel() {
}


PmxModel::~PmxModel() {
}


void PmxModel::pushBackVertex(const Vertex &vertex) {
    vertices_.push_back(vertex);
}


void PmxModel::pushBackSurface(const TriangleSurface &surface) {
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


const Vertex &PmxModel::getVertex(const unsigned int vertexNo) const {
    assert(vertexNo < vertices_.size());
    return vertices_[vertexNo];
}


const int PmxModel::getVertexNum() const {
    return static_cast<int>(vertices_.size());
}


const vector<Vertex> &PmxModel::getVertices() const {
    return vertices_;
}


vector<Vertex> &PmxModel::getVertices() {
    return vertices_;
}


const TriangleSurface &PmxModel::getSurface(const unsigned int surfaceNo) const {
    assert(surfaceNo < surfaces_.size());
    return surfaces_[surfaceNo];
}


const int PmxModel::getSurfaceNum() const {
    return static_cast<int>(surfaces_.size());
}


const cv::Mat &PmxModel::getTexture(const unsigned int textureNo) const {
    assert(textureNo < textures_.size());
    return textures_[textureNo];
}


const int PmxModel::getTextureNum() const {
    return static_cast<int>(textures_.size());
}


const Material &PmxModel::getMaterial(const unsigned int materialNo) const {
    assert(materialNo < materials_.size());
    return materials_[materialNo];
}


int PmxModel::getMaterialNum() const {
    return static_cast<int>(materials_.size());
}


const Bone &PmxModel::getBone(const unsigned int boneNo) const {
    assert(boneNo < bones_.size());
    return bones_[boneNo];
}


Bone &PmxModel::getBone(const unsigned int boneNo) {
    assert(boneNo < bones_.size());
    return bones_[boneNo];
}


const int PmxModel::getBoneNum() const {
    return static_cast<int>(bones_.size());
}


const vector<Bone> &PmxModel::getBones() const {
    return bones_;
}


vector<Bone> &PmxModel::getBones() {
    return bones_;
}

