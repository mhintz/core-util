#pragma once

#include <array>

#include "cinder/gl/gl.h"

#include "CoreMath.h"

class Projector {
public:
	// For helping in calculating configuration parameters. Returns the vertical field of view and the vertical "offset angle"
	// zDistance: distance of projector from wall
	// imgHeight: height of the projected image
	// topHeight: height of the top of the image on the wall
	// Returns [ vertical fov, "base angle" or "offset angle" ]
	static std::array<float, 2> verticalAnglesFromHeights(float zDistance, float imgHeight, float topHeight);
	// For helping in calculating configuration parameters. Returns the horizontal field of view (assumes that the horizontal offset angle is zero)
	static float horizontalAngleFromDims(float zDistance, float imgWidth);
	// Calculates aspect ratio from a width and a height

	Projector()
	: mHorFOV(M_PI / 4), mVertFOV(M_PI / 4), mVertBaseAngle(M_PI / 8) {}

	Projector(float horFOV, float vertFOV, float baseAngle)
	: mHorFOV(horFOV), mVertFOV(vertFOV), mVertBaseAngle(baseAngle) {}

	Projector & setColor(ci::Color col) { mColor = col; return *this; }

	// View-changing functions
	Projector & moveTo(vec3 pos);
	Projector & setUpsideDown(bool isUpsideDown = true);
	Projector & setYRotation(float rotation);

	// Projection-changing functions
	// For changing the projector's zoom and cast angle, which affect fov and base angle values
	Projector & setHorFOV(float fov);
	Projector & setVertFOV(float fov);
	Projector & setVertBaseAngle(float angle);

	// Accessors for the matrices
	mat4 const & getViewMatrix();
	mat4 const & getProjectionMatrix();

	// Accessors for properties
	float getHorFOV() const { return mHorFOV; }
	float getVertFOV() const { return mVertFOV; }
	float getVertBaseAngle() const { return mVertBaseAngle; }
	vec3 getPos() const { return mPosition; }
	vec3 getWorldPos() const;
	vec3 getTarget() const { return vec3(0, mPosition.y, 0); }
	bool getUpsideDown() const { return mUpsideDown; }
	float getYRotation() const { return mYRotation; }
	ci::Color getColor() const { return mColor; }

	void draw();

private:
	// Cached matrix calculation
	void calcViewMatrix();
	void calcProjectionMatrix();
	void calcPolyline();
	void setProjectionMatrixDirty();

	std::array<float, 6> getFrustumDims();

	// Matrix caching system
	mat4 mViewMatrix;
	bool mViewMatrixCached = false;
	mat4 mProjectionMatrix;
	bool mProjectionMatrixCached = false;
	ci::PolyLine3 mFrustumMesh;
	bool mPolylineCached = false;

	// 3D position and orientation properties of the Projector's lens, here abstracted as a point in space
	// Position is cylindrical coordinates - x is distance from center, y is y-position, z is rotation angle around center
	vec3 mPosition = vec3(1, 0, 0);
	bool mUpsideDown = false;
	float mYRotation = 0; // rotation around the Projector's y-axis, to account for offsets from center

	// Properties of the projector's beam, used to determine the shape of the projector's projection frustum
	// Horizontal FOV
	float mHorFOV;
	// Vertical FOV
	float mVertFOV;
	// The angular vertical shift of the projector.
	// Measured as the angle between the projector's base level and the bottom of the projected image.
	// I believe that for all projectors and projector setings, this is at least 0.
	float mVertBaseAngle;

	ci::Color mColor = ci::Color::white();
};

typedef std::shared_ptr<Projector> ProjectorRef;

// Returns configuration for an Acer P5515 projector at minimum "zoom" setting
Projector getAcerP5515MinZoom();

// Returns configuration for an Acer P5515 projector at maximum "zoom" setting
Projector getAcerP5515MaxZoom();

// Returns configuration for a Qumi pocket projector
Projector getQumi();

// Returns configuration for an Acer P3251 projector at minimum "zoom" setting
Projector getAcerP3251MinZoom();