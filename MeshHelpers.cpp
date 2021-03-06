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

void genCubeMapFace(vec2 ul, vec2 lr, vec3 tu, vec3 tv, vec3 tw, vector<vec2> * positions, vector<vec3> * cubeMapTexCoords) {
	positions->emplace_back(lr.x, ul.y); // upper right
	cubeMapTexCoords->emplace_back(normalize(tw + tu + tv));

	positions->emplace_back(ul.x, ul.y); // upper left
	cubeMapTexCoords->emplace_back(normalize(tw + tv));

	positions->emplace_back(lr.x, lr.y); // lower right
	cubeMapTexCoords->emplace_back(normalize(tw + tu));

	positions->emplace_back(ul.x, lr.y); // lower left
	cubeMapTexCoords->emplace_back(normalize(tw));

	positions->emplace_back(lr.x, lr.y); // lower right
	cubeMapTexCoords->emplace_back(normalize(tw + tu));

	positions->emplace_back(ul.x, ul.y); // upper left
	cubeMapTexCoords->emplace_back(normalize(tw + tv));
}

// Special configuration for Martin Fröhlich's SPARCK program
// (SPARCK expects a rectangle of -X, +Z, +X, -Z, +Y, -Y)
gl::VboMeshRef makeCubeMapToRowLayoutMesh_SPARCK(uint32_t side) {
	vector<vec2> positions;
	vector<vec3> cubeMapTexCoords;

	int place;
	// Generate six sets of positions and texcoords for drawing the six faces of the cube map
	// + X
	place = 2;
	genCubeMapFace(vec2(side * place, 0), vec2(side * (place + 1), side), vec3(0, 0, -2), vec3(0, 2, 0), vec3(1, -1, 1), & positions, & cubeMapTexCoords);
	// - X
	place = 0;
	genCubeMapFace(vec2(side * place, 0), vec2(side * (place + 1), side), vec3(0, 0, 2), vec3(0, 2, 0), vec3(-1, -1, -1), & positions, & cubeMapTexCoords);
	// + Y
	place = 4;
	genCubeMapFace(vec2(side * place, 0), vec2(side * (place + 1), side), vec3(2, 0, 0), vec3(0, 0, -2), vec3(-1, 1, 1), & positions, & cubeMapTexCoords);
	// - Y
	place = 5;
	genCubeMapFace(vec2(side * place, 0), vec2(side * (place + 1), side), vec3(2, 0, 0), vec3(0, 0, 2), vec3( -1, -1, -1 ), & positions, & cubeMapTexCoords);
	// Note: I think that somewhere in SPARCK the Z-axis is getting swapped.
	// But frames communicating with it can be fixed by swapping (or not)
	// the places of the +Z and -Z sides of the cubemap
	// + Z
	place = 1;
	genCubeMapFace(vec2(side * place, 0), vec2(side * (place + 1), side), vec3(2, 0, 0), vec3(0, 2, 0), vec3(-1, -1, 1), & positions, & cubeMapTexCoords);
	// - Z
	place = 3;
	genCubeMapFace(vec2(side * place, 0), vec2(side * (place + 1), side), vec3(-2, 0, 0), vec3(0, 2, 0), vec3(1, -1, -1), & positions, & cubeMapTexCoords);

	auto posBufLayout = geom::BufferLayout({ geom::AttribInfo(geom::POSITION, 2, 0, 0) });
	auto posBuf = gl::Vbo::create(GL_ARRAY_BUFFER, positions, GL_STREAM_DRAW);
	auto cubeMapTexBufLayout = geom::BufferLayout({ geom::AttribInfo(geom::TEX_COORD_0, 3, 0, 0) });
	auto cubeMapTexBuf = gl::Vbo::create(GL_ARRAY_BUFFER, cubeMapTexCoords, GL_STREAM_DRAW);

	return gl::VboMesh::create(positions.size(), GL_TRIANGLES, { { posBufLayout, posBuf }, { cubeMapTexBufLayout, cubeMapTexBuf } });
}

void genRowLayoutFace(uint32_t side, vec2 tlu, vec2 trl, vector<vec2> * positions, vector<vec2> * texCoords) {
	positions->emplace_back(vec2(side, 0)); // upper right
	texCoords->emplace_back(trl.x, tlu.y);

	positions->emplace_back(vec2(0, 0)); // upper left
	texCoords->emplace_back(tlu.x, tlu.y);

	positions->emplace_back(vec2(side, side)); // lower right
	texCoords->emplace_back(trl.x, trl.y);

	positions->emplace_back(vec2(0, side)); // lower left
	texCoords->emplace_back(tlu.x, trl.y);

	positions->emplace_back(vec2(side, side)); // lower right
	texCoords->emplace_back(trl.x, trl.y);

	positions->emplace_back(vec2(0, 0)); // upper left
	texCoords->emplace_back(tlu.x, tlu.y);
}

gl::VboMeshRef makeRowLayoutToCubeMapMesh(uint32_t side) {
	vector<vec2> positions;
	vector<vec2> texCoords;
	vector<int> faceIndex;

	float texSide = 1.0f / 6.0f;

	// + X
	genRowLayoutFace(side, vec2(texSide * 0, 1.0), vec2(texSide * 1, 0.0), & positions, & texCoords);
	faceIndex.insert(faceIndex.end(), 6, 0);
	// - X
	genRowLayoutFace(side, vec2(texSide * 1, 1.0), vec2(texSide * 2, 0.0), & positions, & texCoords);
	faceIndex.insert(faceIndex.end(), 6, 1);
	// + Y
	genRowLayoutFace(side, vec2(texSide * 2, 1.0), vec2(texSide * 3, 0.0), & positions, & texCoords);
	faceIndex.insert(faceIndex.end(), 6, 2);
	// - Y
	genRowLayoutFace(side, vec2(texSide * 3, 1.0), vec2(texSide * 4, 0.0), & positions, & texCoords);
	faceIndex.insert(faceIndex.end(), 6, 3);
	// + Z
	genRowLayoutFace(side, vec2(texSide * 4, 1.0), vec2(texSide * 5, 0.0), & positions, & texCoords);
	faceIndex.insert(faceIndex.end(), 6, 4);
	// - Z
	genRowLayoutFace(side, vec2(texSide * 5, 1.0), vec2(texSide * 6, 0.0), & positions, & texCoords);
	faceIndex.insert(faceIndex.end(), 6, 5);


	auto posBufLayout = geom::BufferLayout({ geom::AttribInfo(geom::POSITION, 2, 0, 0) });
	auto posBuf = gl::Vbo::create(GL_ARRAY_BUFFER, positions, GL_STREAM_DRAW);
	auto texBufLayout = geom::BufferLayout({ geom::AttribInfo(geom::TEX_COORD_0, 2, 0, 0) });
	auto texBuf = gl::Vbo::create(GL_ARRAY_BUFFER, texCoords, GL_STREAM_DRAW);
	auto faceIdxBufLayout = geom::BufferLayout({ geom::AttribInfo(geom::CUSTOM_0, geom::INTEGER, 1, 0, 0) });
	auto faceIdxBuf = gl::Vbo::create(GL_ARRAY_BUFFER, faceIndex, GL_STREAM_DRAW);

	return gl::VboMesh::create(positions.size(), GL_TRIANGLES, { { posBufLayout, posBuf }, { texBufLayout, texBuf }, { faceIdxBufLayout, faceIdxBuf } });
}

gl::VboMeshRef makeCubeMapFaceMesh() {
	vector<vec2> positions;
	vector<vec3> texCoords;
	vector<int> faceIndex;

	// + X
	genCubeMapFace(vec2(-1, 1), vec2(1, -1), vec3(0, 0, -2), vec3(0, 2, 0), vec3(1, -1, 1), & positions, & texCoords);
	faceIndex.insert(faceIndex.end(), 6, 0);
	// - X
	genCubeMapFace(vec2(-1, 1), vec2(1, -1), vec3(0, 0, 2), vec3(0, 2, 0), vec3(-1, -1, -1), & positions, & texCoords);
	faceIndex.insert(faceIndex.end(), 6, 1);
	// + Y
	// Both y directions have their cubemap coordinate y-axis flipped from the way the
	// other cube map coords are set up in makeCubeMapToRowLayoutMesh.
	// Tbh I don't know exactly why this is necessary, but it is
	genCubeMapFace(vec2(-1, 1), vec2(1, -1), vec3(2, 0, 0), vec3(0, 0, 2), vec3(-1, -1, -1), & positions, & texCoords);
	faceIndex.insert(faceIndex.end(), 6, 2);
	// - Y
	genCubeMapFace(vec2(-1, 1), vec2(1, -1), vec3(2, 0, 0), vec3(0, 0, -2), vec3( -1, 1, 1 ), & positions, & texCoords);
	faceIndex.insert(faceIndex.end(), 6, 3);
	// + Z
	genCubeMapFace(vec2(-1, 1), vec2(1, -1), vec3(2, 0, 0), vec3(0, 2, 0), vec3(-1, -1, 1), & positions, & texCoords);
	faceIndex.insert(faceIndex.end(), 6, 4);
	// - Z
	genCubeMapFace(vec2(-1, 1), vec2(1, -1), vec3(-2, 0, 0), vec3(0, 2, 0), vec3(1, -1, -1), & positions, & texCoords);
	faceIndex.insert(faceIndex.end(), 6, 5);

	auto posBufLayout = geom::BufferLayout({ geom::AttribInfo(geom::POSITION, 2, 0, 0) });
	auto posBuf = gl::Vbo::create(GL_ARRAY_BUFFER, positions, GL_STREAM_DRAW);
	auto texBufLayout = geom::BufferLayout({ geom::AttribInfo(geom::TEX_COORD_0, 3, 0, 0) });
	auto texBuf = gl::Vbo::create(GL_ARRAY_BUFFER, texCoords, GL_STREAM_DRAW);
	auto faceIdxBufLayout = geom::BufferLayout({ geom::AttribInfo(geom::CUSTOM_0, geom::INTEGER, 1, 0, 0) });
	auto faceIdxBuf = gl::Vbo::create(GL_ARRAY_BUFFER, faceIndex, GL_STREAM_DRAW);

	return gl::VboMesh::create(positions.size(), GL_TRIANGLES, { { posBufLayout, posBuf }, { texBufLayout, texBuf }, { faceIdxBufLayout, faceIdxBuf } });
}

// Returns an int corresponding to the vector's max dimension.
// If the vector is more in +X or -X, returns 0, returns 1 for +Y or -Y, and 2 for +Z or -Z
// The zero vector returns 2
int cubeMapDimensionIntFromVec(vec3 dir) {
	vec3 absV = abs(normalize(dir));
	return absV.x > absV.y && absV.x > absV.z ? 0 : (absV.y > absV.x && absV.y > absV.z ? 1 : 2);
}

// Returns the GLenum corresponding to the cubemap face pointed to by the 3D vector
GLenum cubeMapFaceFromVec(vec3 dir) {
	int cmDim = cubeMapDimensionIntFromVec(dir);
	assert(cmDim == 0 || cmDim == 1 || cmDim == 2);

	// If the max value is 0, then the vector is the zero vector, which is an error
	// but in this case the function will return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	// because cmDim will be 2 and sign(0) = 0
	switch (cmDim) {
		case 0:
			return glm::sign(dir.x) == 1.0 ? GL_TEXTURE_CUBE_MAP_POSITIVE_X : GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		case 1:
			return glm::sign(dir.y) == 1.0 ? GL_TEXTURE_CUBE_MAP_POSITIVE_Y : GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		case 2:
			return glm::sign(dir.z) == 1.0 ? GL_TEXTURE_CUBE_MAP_POSITIVE_Z : GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		default:
			assert(0);
	}

	// Execution should never reach this point
	assert(0);
}

// Returns the uv coords on the cubemap face corresponding to the position of the 3D vector
vec2 cubeMapFaceCoordFromVec(vec3 dir) {
	GLenum cmFace = cubeMapFaceFromVec(dir);

	vec3 divd;
	switch (cmFace) {
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
			divd = dir / abs(dir.x);
			return vec2(inv(map01(divd.z)), map01(divd.y));
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
			divd = dir / abs(dir.x);
			return vec2(map01(divd.z), map01(divd.y));

		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
			divd = dir / abs(dir.y);
			return vec2(map01(divd.x), map01(divd.z));
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
			divd = dir / abs(dir.y);
			return vec2(map01(divd.x), inv(map01(divd.z)));

		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
			divd = dir / abs(dir.z);
			return vec2(map01(divd.x), inv(map01(divd.y)));
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			divd = dir / abs(dir.z);
			return vec2(inv(map01(divd.x)), inv(map01(divd.y)));

		default:
			// This code should be unreachable
			assert(0);
	}
}