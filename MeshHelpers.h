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