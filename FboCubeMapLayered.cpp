#include "FboCubeMapLayered.h"

using namespace ci;

FboCubeMapLayered::Format::Format() {
	mHasColor = true;
	mColorFormat = gl::TextureCubeMap::Format().internalFormat(GL_RGBA8);
	mHasDepth = true;
	mDepthFormat = gl::TextureCubeMap::Format().internalFormat(GL_DEPTH_COMPONENT);
}

FboCubeMapLayeredRef FboCubeMapLayered::create(int width, int height, const Format & format) {
	return FboCubeMapLayeredRef(new FboCubeMapLayered(width, height, format));
}

FboCubeMapLayered::FboCubeMapLayered(int width, int height, const Format & format)
: mWidth(width), mHeight(height) {
	glGenFramebuffers(1, & mFboHandle);
	gl::ScopedFramebuffer scpFbo(GL_FRAMEBUFFER, mFboHandle);

	if (format.mHasColor) {
		mBackingColor = gl::TextureCubeMap::create(width, height, format.mColorFormat);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mBackingColor->getId(), 0);
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);
	}

	if (format.mHasDepth) {
		mBackingDepth = gl::TextureCubeMap::create(width, height, format.mDepthFormat);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mBackingDepth->getId(), 0);
	}
}

FboCubeMapLayered::~FboCubeMapLayered() {
	glDeleteFramebuffers(1, & mFboHandle);
}

void FboCubeMapLayered::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, mFboHandle);
}

void FboCubeMapLayered::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}