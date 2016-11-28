#pragma once

#include "cinder/gl/gl.h"

#include "buildmesh.h"

GLenum bmeshPrimitive(bmesh::Primitive prim);
ci::gl::VboMeshRef bmeshToVBOMesh(bmesh::MeshRef theMesh);