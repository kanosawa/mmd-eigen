#include "Surface.h"

using namespace mmd;

Surface::Surface() {
}


Surface::Surface(const Eigen::Vector3i &vertexIndexies)
        : vertexIndexies_(vertexIndexies) {
}


Surface::Surface(const Surface &surface)
        : vertexIndexies_(surface.getVertexIndexies()) {
}


Surface::~Surface() {
}


Surface &Surface::operator=(const Surface &surface) {
    vertexIndexies_ = surface.getVertexIndexies();
}


void Surface::setVertexIndexies(const Eigen::Vector3i &vertexIndexies) {
    vertexIndexies_ = vertexIndexies;
}


const Eigen::Vector3i &Surface::getVertexIndexies() const {
    return vertexIndexies_;
}