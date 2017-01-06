#include "MeshHelpers.h"

using namespace ci;
using std::vector;

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

void addNormalizedFace(vec3 origin, vec3 uVec, vec3 vVec, uint subdivisions, int faceNum, vector<vec3> * positions, vector<vec3> * normals, vector<int> * faces, vector<uint> * indices) {
	vec3 uInc = uVec / (float) subdivisions;
	vec3 vInc = vVec / (float) subdivisions;
	for (uint vCnt = 0; vCnt <= subdivisions; vCnt++) {
		float vMult = vCnt;
		for (uint uCnt = 0; uCnt <= subdivisions; uCnt++) {
			uint vertIndex = positions->size();
			float uMult = uCnt;

			// This is where the magic happens
			vec3 spherePoint = normalize(origin + uMult * uInc + vMult * vInc);

			positions->push_back(spherePoint);
			normals->push_back(spherePoint);
			faces->push_back(faceNum);

			if (uCnt != 0 && vCnt != 0) {
				uint prevRowVert = vertIndex - (subdivisions + 1);
				// Adding a new vertex creates a new "square" on the surface of the sphere
				indices->push_back(vertIndex - 1);
				indices->push_back(prevRowVert - 1);
				indices->push_back(prevRowVert);

				indices->push_back(vertIndex);
				indices->push_back(vertIndex - 1);
				indices->push_back(prevRowVert);
			}
		}
	}
}

gl::VboMeshRef makeNormalizedCubeSphere() {
	vector<vec3> positions;
	vector<vec3> normals;
	vector<int> faces;
	vector<uint> indices;
	uint subdivisions = 12;

	// positive X
	addNormalizedFace(vec3(1, -1, 1), vec3(0, 0, -2), vec3(0, 2, 0), subdivisions, 0, & positions, & normals, & faces, & indices);
	// negative X
	addNormalizedFace(vec3(-1, -1, -1), vec3(0, 0, 2), vec3(0, 2, 0), subdivisions, 1, & positions, & normals, & faces, & indices);

	// positive Y
	addNormalizedFace(vec3(-1, 1, 1), vec3(2, 0, 0), vec3(0, 0, -2), subdivisions, 2, & positions, & normals, & faces, & indices);
	// negative Y
	addNormalizedFace(vec3(-1, -1, -1), vec3(2, 0, 0), vec3(0, 0, 2), subdivisions, 3, & positions, & normals, & faces, & indices);

	// positive Z
	addNormalizedFace(vec3(-1, -1, 1), vec3(2, 0, 0), vec3(0, 2, 0), subdivisions, 4, & positions, & normals, & faces, & indices);
	// negative Z
	addNormalizedFace(vec3(1, -1, -1), vec3(-2, 0, 0), vec3(0, 2, 0), subdivisions, 5, & positions, & normals, & faces, & indices);

	auto posBufLayout = geom::BufferLayout({ geom::AttribInfo(geom::POSITION, geom::FLOAT, 3, 0, 0) });
	auto posBuf = gl::Vbo::create(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_STATIC_DRAW);
	auto normBufLayout = geom::BufferLayout({ geom::AttribInfo(geom::NORMAL, geom::FLOAT, 3, 0, 0) });
	auto normBuf = gl::Vbo::create(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW);
	auto faceBufLayout = geom::BufferLayout({ geom::AttribInfo(geom::CUSTOM_0, geom::INTEGER, 1, 0, 0) });
	auto faceBuf = gl::Vbo::create(GL_ARRAY_BUFFER, faces.size() * sizeof(int), faces.data(), GL_STATIC_DRAW);

	auto indexBuf = gl::Vbo::create(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data());

	return gl::VboMesh::create(positions.size(), GL_TRIANGLES, { { posBufLayout, posBuf }, { normBufLayout, normBuf }, { faceBufLayout, faceBuf } }, indices.size(), GL_UNSIGNED_INT, indexBuf);
}