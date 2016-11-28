#include "GeomEater.h"

namespace cinder {

GeomEater::GeomEater(TriMesh::Format format)
: mMainMesh(TriMesh::create(format)), mBaseNumVertices(0) {

}

TriMesh const & GeomEater::getMesh() {
	return *mMainMesh;
}

TriMeshRef GeomEater::getMeshRef() {
	return mMainMesh;
}

uint8_t GeomEater::getAttribDims(geom::Attrib attr) const {
	return mMainMesh->getAttribDims(attr);
}

void copyFloatDataVector(uint8_t srcDims, float const * srcData, size_t numVerts, uint8_t destDims, std::vector<float> * targetVector) {
	std::vector<float> temp(destDims * numVerts);
	geom::copyData(srcDims, srcData, numVerts, destDims, 0, temp.data());
	targetVector->insert(targetVector->end(), temp.begin(), temp.end());
}

void copyVectorDataVector(uint8_t srcDims, float const * srcData, size_t numVerts, uint8_t destDims, std::vector<vec3> * targetVector) {
	std::vector<vec3> temp(numVerts);
	geom::copyData(srcDims, srcData, numVerts, destDims, 0, glm::value_ptr(*temp.data()));
	targetVector->insert(targetVector->end(), temp.begin(), temp.end());
}

void GeomEater::copyAttrib(geom::Attrib attr, uint8_t srcAttrDims, size_t strideBytes, float const * srcData, size_t numVertices) {
	uint8_t destAttrDims = getAttribDims(attr);

	if (destAttrDims == 0) {
		// Do nothing if the base mesh is not interested in this attribute
		return;
	}

	switch (attr) {
		// Vectors of floats
		case geom::Attrib::POSITION:
			copyFloatDataVector(srcAttrDims, srcData, numVertices, destAttrDims, & mMainMesh->getBufferPositions());
		break;
		case geom::Attrib::COLOR:
			copyFloatDataVector(srcAttrDims, srcData, numVertices, destAttrDims, & mMainMesh->getBufferColors());
		break;
		case geom::Attrib::TEX_COORD_0:
			copyFloatDataVector(srcAttrDims, srcData, numVertices, destAttrDims, & mMainMesh->getBufferTexCoords0());
		break;
		case geom::Attrib::TEX_COORD_1:
			copyFloatDataVector(srcAttrDims, srcData, numVertices, destAttrDims, & mMainMesh->getBufferTexCoords1());
		break;
		case geom::Attrib::TEX_COORD_2:
			copyFloatDataVector(srcAttrDims, srcData, numVertices, destAttrDims, & mMainMesh->getBufferTexCoords2());
		break;
		case geom::Attrib::TEX_COORD_3:
			copyFloatDataVector(srcAttrDims, srcData, numVertices, destAttrDims, & mMainMesh->getBufferTexCoords3());
		break;
		// Vectors of vec3
		case geom::Attrib::NORMAL:
			copyVectorDataVector(srcAttrDims, srcData, numVertices, destAttrDims, & mMainMesh->getNormals());
		break;
		case geom::Attrib::TANGENT:
			copyVectorDataVector(srcAttrDims, srcData, numVertices, destAttrDims, & mMainMesh->getTangents());
		break;
		case geom::Attrib::BITANGENT:
			copyVectorDataVector(srcAttrDims, srcData, numVertices, destAttrDims, & mMainMesh->getBitangents());
		break;
		// No such thing!
		default:
			throw geom::ExcMissingAttrib();
	}
}

void GeomEater::copyIndices(geom::Primitive primitive, uint32_t const * sourceData, size_t inputNumIndices, uint8_t numBytesPerIndex) {
	size_t targetNumIndices = inputNumIndices;
	if (primitive == geom::Primitive::TRIANGLE_STRIP || primitive == geom::Primitive::TRIANGLE_FAN) {
		// Own indices need to be for triangles, so there are many more of them
		targetNumIndices = (inputNumIndices - 2) * 3;
	}

	std::vector<uint32_t> offsetIndices(inputNumIndices);
	for (size_t idx = 0; idx < inputNumIndices; idx++) {
		offsetIndices[idx] = mBaseNumVertices + sourceData[idx];
	}

	// Copy over the indices from the buffer with offsets, making any necessary adjustments to convert the
	// primitive to triangles
	std::vector<uint32_t> targetBuffer(targetNumIndices);
	copyIndexDataForceTriangles(primitive, offsetIndices.data(), targetNumIndices, 0, targetBuffer.data());
	
	// Copy indexes into the space starting at the end of the index buffer
	std::vector<uint32_t> & indexBuffer = mMainMesh->getIndices();
	indexBuffer.insert(indexBuffer.end(), targetBuffer.begin(), targetBuffer.end());

	// Each time this function is called, it's assumed that a new mesh has been copied in.
	// This updates the base vertex count for the next time indexes are added to the mesh.
	// This is a terrible, regrettable hack, but it's necessary to account for the way that cinder
	// geom::Source objects perform the copying and loading of vertex data, without regard for the timing
	// of the call to copyIndices
	mBaseNumVertices = mMainMesh->getNumVertices();
}

geom::AttribSet GeomEater::getSupportedAttribs() {
	static std::array<geom::Attrib, 9> possibleAttribs = {
		geom::Attrib::POSITION, geom::Attrib::NORMAL, geom::Attrib::TANGENT, geom::Attrib::BITANGENT, geom::Attrib::COLOR, geom::Attrib::TEX_COORD_0, geom::Attrib::TEX_COORD_1, geom::Attrib::TEX_COORD_2, geom::Attrib::TEX_COORD_3
	};

	geom::AttribSet supported;
	for (auto attr : possibleAttribs) {
		if (getAttribDims(attr)) {
			supported.insert(attr);
		}
	}

	return supported;
}

void GeomEater::eat(geom::Source const & src) {
	src.loadInto(this, getSupportedAttribs());
}

} // namespace cinder