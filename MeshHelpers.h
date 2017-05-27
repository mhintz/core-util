#pragma once

#include <vector>

#include "cinder/gl/gl.h"

#include "buildmesh.h"
#include "CoreMath.h"

GLenum bmeshPrimitive(bmesh::Primitive prim);

ci::gl::VboMeshRef bmeshToVBOMesh(bmesh::MeshRef theMesh);

ci::gl::VboMeshRef makeNormalizedCubeSphere();

ci::gl::VboMeshRef makeCubeMapToRowLayoutMesh_SPARCK(uint32_t side);

ci::gl::VboMeshRef makeRowLayoutToCubeMapMesh(uint32_t side);

ci::gl::VboMeshRef makeCubeMapFaceMesh();

// Returns an int corresponding to the vector's max dimension.
// If the vector is more in +X or -X, returns 0, returns 1 for +Y or -Y, and 2 for +Z or -Z
// The zero vector returns 2
int cubeMapDimensionIntFromVec(vec3 dir);
// Returns the GLenum corresponding to the cubemap face pointed to by the 3D vector
GLenum cubeMapFaceFromVec(vec3 dir);
// Returns the uv coords on the cubemap face corresponding to the position of the 3D vector
vec2 cubeMapFaceCoordFromVec(vec3 dir);