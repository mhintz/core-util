#pragma once

#include "cinder/CinderMath.h"
#include "cinder/Matrix.h"
#include "cinder/Vector.h"
#include "cinder/Quaternion.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"

using namespace ci;

class Node {
public:
	Node() : mPosition(vec3(0)), mOrientation(quat()), mScale(vec3(1)) {}
	Node(vec3 p, quat o, vec3 s) : mPosition(p), mOrientation(o), mScale(s) {}

	void setPosition(vec3 p) { mPosition = p; }
	void setPosition(float x, float y, float z) { mPosition = vec3(x, y, z); }
	void setOrientation(quat o) { mOrientation = o; }
	void setRotation(float angle, vec3 axis) { mOrientation = glm::angleAxis(angle, normalize(axis)); }
	void setRotation(vec3 euler) { mOrientation = toQuat(orientate3(euler)); }
	void setScale(vec3 s) { mScale = s; }
	void setScale(float x, float y, float z) { mScale = vec3(x, y, z); }
	void setAll(vec3 p, quat o, vec3 s) { mPosition = p; mOrientation = o; mScale = s; }

	void translate(vec3 t) { mPosition += t; }
	void rotate(quat r) { mOrientation = normalize(r * mOrientation); }
	void scale(vec3 s) { mScale *= s; }

	vec3 position() { return mPosition; }
	quat orientation() { return mOrientation; }
	vec3 scale() { return mScale; }
	mat4 matrix() { return glm::scale(mScale) * glm::toMat4(mOrientation) * glm::translate(mPosition); }

private:
	vec3 mPosition;
	quat mOrientation;
	vec3 mScale;
};