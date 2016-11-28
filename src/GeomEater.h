#pragma once

#include "cinder/GeomIo.h"
#include "cinder/TriMesh.h"

namespace cinder {

class GeomEater;
typedef std::unique_ptr<GeomEater> GeomEaterBox;
typedef std::shared_ptr<GeomEater> GeomEaterRef;

class GeomEater : public geom::Target {
public:
	GeomEater(TriMesh::Format format=TriMesh::Format().positions().normals().texCoords());

	uint8_t getAttribDims(geom::Attrib attr) const override;
	void copyAttrib(geom::Attrib attr, uint8_t dims, size_t strideBytes, float const * srcData, size_t count) override;
	void copyIndices(geom::Primitive primitive, uint32_t const * source, size_t numIndices, uint8_t numBytesPerIndex) override;

	geom::AttribSet getSupportedAttribs();
	void eat(geom::Source const & src);
	TriMesh const & getMesh();
	TriMeshRef getMeshRef();

private:
	TriMeshRef mMainMesh;
	uint32_t mBaseNumVertices;
};

} // namespace cinder