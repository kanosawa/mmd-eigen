#include <map>
#include "VertexStream.h"

using namespace mmd;

VertexStream::VertexStream() {
}


VertexStream::~VertexStream() {
}


void VertexStream::pushBackVertices(const int frameNo, const vector<Vertex> &vertices) {
    vertexListMap_[frameNo] = vertices;
}


bool VertexStream::getVertices(vector<Vertex> &bones, const int frameNo) const {
    auto itr = vertexListMap_.find(frameNo);
    if (itr == vertexListMap_.end()) {
        return false;
    }
    bones = itr->second;
    return true;
}


const map<int, vector<Vertex>> &VertexStream::getVertexListMap() const {
    return vertexListMap_;
}