#include "DoubleFbo.h"

using namespace ci;

void DoubleFbo::bindSource(uint8_t textureUnit) {
	mSource->bindTexture(textureUnit);
}

void DoubleFbo::bindDest() {
	mDest->bindFramebuffer();
}

void DoubleFbo::unbindSource(uint8_t textureUnit) {
	mSource->unbindTexture(textureUnit);
}

void DoubleFbo::unbindDest() {
	mDest->unbindFramebuffer();
}

void DoubleFbo::update(uint8_t textureUnit) {
	gl::ScopedViewport scpView(0, 0, mWidth, mHeight);
	gl::ScopedMatrices scpMat;
	gl::setMatricesWindow(mWidth, mHeight);
	gl::ScopedTextureBind scpTex(mSource->getColorTexture(), textureUnit);
	gl::ScopedFramebuffer scpFbo(mDest);
	gl::drawSolidRect(Rectf(0, 0, mWidth, mHeight));
}

void DoubleFbo::swap() {
	std::swap(mSource, mDest);
}

ScopedDoubleFbo::ScopedDoubleFbo(DoubleFboRef const & doubleFbo, uint8_t textureUnit)
: mCtx(gl::context()), mTextureUnit(textureUnit) {
	gl::Texture2dRef texRef = doubleFbo->getSource()->getColorTexture();
	mCtx->pushTextureBinding(GL_TEXTURE_2D, texRef->getId(), mTextureUnit);
	mCtx->pushFramebuffer(doubleFbo->getDest(), GL_FRAMEBUFFER);
}

ScopedDoubleFbo::~ScopedDoubleFbo() {
	mCtx->popTextureBinding(GL_TEXTURE_2D, mTextureUnit);
	mCtx->popFramebuffer(GL_FRAMEBUFFER);
}