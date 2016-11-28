#include "MeshHelpers.h"

using namespace ci;

GLenum bmeshPrimitive(bmesh::Primitive prim) {
	switch (prim) {
		case bmesh::Primitive::Lines: return GL_LINES;
		case bmesh::Primitive::Triangles: return GL_TRIANGLES;
		default: return GL_TRIANGLES; // Default to triangles because meh
	}
}

gl::VboMeshRef bmeshToVBOMesh(bmesh::MeshRef theMesh) {
	static size_t vertexSize = bmesh::Mesh::getVertexSize();
	static size_t indexSize = bmesh::Mesh::getIndexSize();
	static geom::BufferLayout bmeshBufferLayout = geom::BufferLayout({
		geom::AttribInfo(geom::Attrib::POSITION, 3, vertexSize, offsetof(bmesh::Vertex, mPosition)),
		geom::AttribInfo(geom::Attrib::NORMAL, 3, vertexSize, offsetof(bmesh::Vertex, mNormal)),
		geom::AttribInfo(geom::Attrib::COLOR, 3, vertexSize, offsetof(bmesh::Vertex, mColor)),
		geom::AttribInfo(geom::Attrib::TEX_COORD_0, 2, vertexSize, offsetof(bmesh::Vertex, mTexCoord0)),
		geom::AttribInfo(geom::Attrib::TANGENT, 3, vertexSize, offsetof(bmesh::Vertex, mTangent)),
		geom::AttribInfo(geom::Attrib::BITANGENT, 3, vertexSize, offsetof(bmesh::Vertex, mBitangent))
	});

	auto numVerts = theMesh->getNumVertices();
	auto primitive = bmeshPrimitive(theMesh->getPrimitive());
	auto vertexVBO = gl::Vbo::create(GL_ARRAY_BUFFER, numVerts * vertexSize, theMesh->vertexData(), GL_STATIC_DRAW);
	auto numIndices = theMesh->getNumIndices();
	auto indexVBO = gl::Vbo::create(GL_ELEMENT_ARRAY_BUFFER, numIndices * indexSize, theMesh->indexData());

	return gl::VboMesh::create(numVerts, primitive, { { bmeshBufferLayout, vertexVBO } }, numIndices, GL_UNSIGNED_INT, indexVBO);
}