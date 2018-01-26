#include "Surface.h"

using namespace mmd;

Surface::Surface(const unsigned int vertexNumPerSurface)
	: mVertexNumPerSurface(vertexNumPerSurface)
	, mVertices(mVertexNumPerSurface)
{
}


Surface::Surface(const unsigned int vertexNumPerSurface, const std::vector<Vertex>& vertices)
	: mVertexNumPerSurface(vertexNumPerSurface)
{
	setVertices(vertices);
}


Surface::~Surface()
{
}


void Surface::copy(const Surface& surface)
{
	setVertices(surface.getVertices());
}


void Surface::setVertices(const std::vector<Vertex>& vertices)
{
	assert(vertices.size() == mVertexNumPerSurface);
	mVertices = vertices;
}


const std::vector<Vertex>& Surface::getVertices() const
{
	return mVertices;
}


const unsigned int Surface::getVertexNumPerSurface() const
{
	return mVertexNumPerSurface;
}