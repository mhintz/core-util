#pragma once

#include "Math.h"

class Node {
public:
	Node() : mPosition(vec3(0)), mOrientation(quat()), mScale(vec3(1)) {}
	Node(vec3 p, quat o, vec3 s) : mPosition(p), mOrientation(o), mScale(s) {}

	// setters
	void setPosition(vec3 p) { mPosition = p; }
	void setPosition(float x, float y, float z) { mPosition = vec3(x, y, z); }

	void setOrientation(quat o) { mOrientation = o; }
	void setRotation(float angle, vec3 axis) { mOrientation = glm::angleAxis(angle, normalize(axis)); }
	void setRotation(vec3 euler) { mOrientation = toQuat(orientate3(euler)); }

	void setScale(vec3 s) { mScale = s; }
	void setScale(float x, float y, float z) { mScale = vec3(x, y, z); }

	void setAll(vec3 p, quat o, vec3 s) { mPosition = p; mOrientation = o; mScale = s; }

	// modifiers
	void translate(vec3 t) { mPosition += t; }
	// glm::quaternion rotation composition: 2nd * 1st
	void rotate(quat r) { mOrientation = normalize(r * mOrientation); }
	void scale(float factor) { mScale *= factor; }
	void scale(vec3 s) { mScale *= s; }

	// getters
	vec3 position() const { return mPosition; }
	quat orientation() const { return mOrientation; }
	vec3 scale() const { return mScale; }
	mat4 matrix() const { return glm::scale(mScale) * glm::toMat4(mOrientation) * glm::translate(mPosition); }

private:
	vec3 mPosition;
	quat mOrientation;
	vec3 mScale;
};