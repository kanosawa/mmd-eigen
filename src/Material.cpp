#include "Material.h"

using namespace mmd;

Material::Material() {
}


Material::~Material() {
}


Material::Material(const Eigen::Vector4f &diffuse, const Eigen::Vector3f &specular, const float specularCoef,
                   const Eigen::Vector3f &ambient, const unsigned char ordinaryTextureIndex, const int surfaceNum)
        : diffuse_(diffuse), specular_(specular), specularCoef_(specularCoef), ambient_(ambient),
          ordinaryTextureIndex_(ordinaryTextureIndex), surfaceNum_(surfaceNum) {
}


const Eigen::Vector4f &Material::getDiffuse() const {
    return diffuse_;
}


const Eigen::Vector3f &Material::getSpecular() const {
    return specular_;
}


const float Material::getSpecularCoef() const {
    return specularCoef_;
}


const Eigen::Vector3f &Material::getAmbient() const {
    return ambient_;
}


const unsigned char Material::getOrdinaryTextureIndex() const {
    return ordinaryTextureIndex_;
}


const int Material::getSurfaceNum() const {
    return surfaceNum_;
}
