#include "Material.h"

using namespace mmd;

Material::Material() {
}


Material::~Material() {
}


void Material::setDiffuse(const Eigen::Vector4f &diffuse) {
    diffuse_ = diffuse;
}


const Eigen::Vector4f &Material::getDiffuse() const {
    return diffuse_;
}


void Material::setSpecular(const Eigen::Vector3f &specular) {
    specular_ = specular;
}


const Eigen::Vector3f &Material::getSpecular() const {
    return specular_;
}


void Material::setSpecularCoef(const float specularCoef) {
    specularCoef_ = specularCoef;
}


const float Material::getSpecularCoef() const {
    return specularCoef_;
}


void Material::setAmbient(const Eigen::Vector3f &ambient) {
    ambient_ = ambient;
}


const Eigen::Vector3f &Material::getAmbient() const {
    return ambient_;
}


void Material::setOrdinaryTextureIndex(const unsigned char ordinaryTextureIndex) {
    ordinaryTextureIndex_ = ordinaryTextureIndex;
}


const unsigned char Material::getOrdinaryTextureIndex() const {
    return ordinaryTextureIndex_;
}


void Material::setSurfaceNum(const int surfaceNum) {
    surfaceNum_ = surfaceNum;
}


const int Material::getSurfaceNum() const {
    return surfaceNum_;
}
