#include "Surface.h"

using namespace mmd;

Surface::Surface() {
}


Surface::~Surface() {
}


Surface::Surface(const Eigen::Vector3i &vertexIndexies)
        : vertexIndexies_(vertexIndexies) {
}


const Eigen::Vector3i &Surface::getVertexIndexies() const {
    return vertexIndexies_;
}
