#pragma once

#include <vector>

#include "cinder/gl/gl.h"

#include "buildmesh.h"
#include "CoreMath.h"

GLenum bmeshPrimitive(bmesh::Primitive prim);

ci::gl::VboMeshRef bmeshToVBOMesh(bmesh::MeshRef theMesh);

ci::gl::VboMeshRef makeNormalizedCubeSphere();

ci::gl::VboMeshRef makeCubeMapRowLayout(uint32_t side);