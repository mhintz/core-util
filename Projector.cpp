#include "Projector.h"

using namespace ci;

// Default frustum near and far values
const float PROJ_NEAR_PLANE_Z = 0.3; // 30cm
const float PROJ_FAR_PLANE_Z = 10.0; // 18m

std::array<float, 2> Projector::verticalAnglesFromHeights(float zDistance, float imgHeight, float topHeight) {
	float fullAngle = atan2(topHeight, zDistance);
	float baseAngle = atan2(topHeight - imgHeight, zDistance);
	return { fullAngle - baseAngle, baseAngle };
}

float Projector::horizontalAngleFromDims(float zDistance, float imgWidth) {
	return 2.f * atan2(imgWidth / 2.f, zDistance);
}

void Projector::setColor(Color col) {
	mColor = col;
}

Projector & Projector::moveTo(vec3 projPos) {
	mPosition = projPos;
	mViewMatrixCached = false;
	return *this;
}

Projector & Projector::setUpsideDown(bool isUpsideDown) {
	mUpsideDown = isUpsideDown;
	mViewMatrixCached = false;
	return *this;
}

Projector & Projector::setYRotation(float rotation) {
	mYRotation = rotation;
	mViewMatrixCached = false;
	return *this;
}

Projector & Projector::setHorFOV(float fov) {
	mHorFOV = fov;
	setProjectionMatrixDirty();
	return *this;
}

Projector & Projector::setVertFOV(float fov) {
	mVertFOV = fov;
	setProjectionMatrixDirty();
	return *this;
}

Projector & Projector::setVertBaseAngle(float angle) {
	mVertBaseAngle = angle;
	setProjectionMatrixDirty();
	return *this;
}

void Projector::setProjectionMatrixDirty() {
	mProjectionMatrixCached = false;
	mPolylineCached = false;
}

mat4 const & Projector::getViewMatrix() {
	if (!mViewMatrixCached) {
		calcViewMatrix();
	}

	return mViewMatrix;
}

mat4 const & Projector::getProjectionMatrix() {
	if (!mProjectionMatrixCached) {
		calcProjectionMatrix();
	}

	return mProjectionMatrix;
}

vec3 Projector::getWorldPos() const {
	return vec3(cos(mPosition.z) * mPosition.x, mPosition.y, sin(mPosition.z) * mPosition.x);
}

void Projector::draw() {
	gl::ScopedModelMatrix scpMat;
	gl::multModelMatrix(glm::inverse(getViewMatrix()));

	float projLensLength = 0.06f;
	float projBodyDepth = 0.16f;

	{
		gl::ScopedGlslProg scpShader(gl::getStockShader(gl::ShaderDef().color().lambert()));
		gl::ScopedColor scpColor(mColor);

		{
			gl::ScopedModelMatrix scpMatInner;
			gl::rotate(M_PI / 2.f, vec3(1, 0, 0));
			gl::draw(geom::Cone().radius(0.06f, 0.04f).height(projLensLength));
		}

		{
			gl::ScopedModelMatrix scpMatInner;
			gl::translate(0, 0, projLensLength + projBodyDepth / 2.f);
			gl::draw(geom::Cube().size(0.2f, 0.1f, projBodyDepth));
		}
	}

	{
		if (!mPolylineCached) {
			calcPolyline();
		}

		gl::ScopedGlslProg scpShader(gl::getStockShader(gl::ShaderDef().color()));
		gl::ScopedColor scpColor(Color(1.0, 1.0, 1.0));

		gl::draw(mFrustumMesh);
	}
}

void Projector::calcViewMatrix() {
	// mPosition is considered a triple of (distance from center, y-position in space, angle about center)
	// The projector is always "looking at" the nearest y-axis point
	vec3 projWorldPosition = getWorldPos();
	mViewMatrix = glm::eulerAngleY(mYRotation) * glm::lookAt(projWorldPosition, vec3(0, mPosition.y, 0), mUpsideDown ? vec3(0, -1, 0) : vec3(0, 1, 0));
	mViewMatrixCached = true;
}

std::array<float, 6> Projector::getFrustumDims() {
	float xval = tan(mHorFOV / 2.0) * PROJ_NEAR_PLANE_Z;
	float bottom = tan(mVertBaseAngle) * PROJ_NEAR_PLANE_Z;
	float top = tan(mVertBaseAngle + mVertFOV) * PROJ_NEAR_PLANE_Z;
	return { -xval, xval, bottom, top, PROJ_NEAR_PLANE_Z, PROJ_FAR_PLANE_Z };
}

void Projector::calcProjectionMatrix() {
	auto dims = getFrustumDims();
	mProjectionMatrix = glm::frustum(dims[0], dims[1], dims[2], dims[3], dims[4], dims[5]);
	mProjectionMatrixCached = true;
}

void Projector::calcPolyline() {
	auto dims = getFrustumDims();

	float l = dims[0], r = dims[1], b = dims[2], t = dims[3], n = dims[4], f = dims[5];
	vec3 xdir(1, 0, 0);
	vec3 ydir(0, 1, 0);
	vec3 zdir(0, 0, -1); // The projector "looks" down the negative z axis from its own origin

	vec3 ntl = l * xdir + t * ydir + n * zdir;
	vec3 ntr = r * xdir + t * ydir + n * zdir;
	vec3 nbl = l * xdir + b * ydir + n * zdir;
	vec3 nbr = r * xdir + b * ydir + n * zdir;

	// Using the similar triangles theorem, can calculate the location of the four boundary points of the far clipping rectangle
	float sideRatio = f / n;

	vec3 ftl = sideRatio * ntl;
	vec3 ftr = sideRatio * ntr;
	vec3 fbl = sideRatio * nbl;
	vec3 fbr = sideRatio * nbr;

	// Draw the positions into the frustum mesh
	mFrustumMesh = PolyLine3();

	// Near rectangle
	mFrustumMesh.push_back(ntl);
	mFrustumMesh.push_back(ntr);
	mFrustumMesh.push_back(nbr);
	mFrustumMesh.push_back(nbl);
	mFrustumMesh.push_back(ntl);

	// Far rectangle with connections
	mFrustumMesh.push_back(ftl);

	mFrustumMesh.push_back(ftr);
	mFrustumMesh.push_back(ntr);
	mFrustumMesh.push_back(ftr);

	mFrustumMesh.push_back(fbr);
	mFrustumMesh.push_back(nbr);
	mFrustumMesh.push_back(fbr);

	mFrustumMesh.push_back(fbl);
	mFrustumMesh.push_back(nbl);
	mFrustumMesh.push_back(fbl);

	mFrustumMesh.push_back(ftl);

	mPolylineCached = true;
}

Projector getAcerP5515MinZoom() {
	auto hfov = Projector::horizontalAngleFromDims(2.0, 1.38);
	auto vertAngles = Projector::verticalAnglesFromHeights(2.0, 0.78, 0.90);
	return Projector(hfov, vertAngles[0], vertAngles[1]);
}

Projector getAcerP5515MaxZoom() {
	auto hfov = Projector::horizontalAngleFromDims(2.0, 1.87);
	auto vertAngles = Projector::verticalAnglesFromHeights(2.0, 1.05, 1.22);
	return Projector(hfov, vertAngles[0], vertAngles[1]);
}

Projector getQumi() {
	auto hfov = Projector::horizontalAngleFromDims(2.0, 1.292);
	auto vertAngles = Projector::verticalAnglesFromHeights(2.0, .808, 0.808 + 0.20);
	return Projector(hfov, vertAngles[0], vertAngles[1]);
}

Projector getAcerP3251MinZoom() {
	auto hfov = Projector::horizontalAngleFromDims(2.0, 0.093);
	auto vertAngles = Projector::verticalAnglesFromHeights(2.0, .7, 0.8);
	return Projector(hfov, vertAngles[0], vertAngles[1]);	
}