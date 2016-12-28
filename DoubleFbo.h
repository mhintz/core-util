#include "cinder/gl/gl.h"
#include "cinder/Noncopyable.h"

class DoubleFbo;
typedef std::shared_ptr<DoubleFbo> DoubleFboRef;

class DoubleFbo {
public:
	static DoubleFboRef create(int width, int height, ci::gl::Fbo::Format const & format = ci::gl::Fbo::Format()) {
		return std::shared_ptr<DoubleFbo>(new DoubleFbo(width, height, format, format));
	}
	static DoubleFboRef create(int width, int height, ci::gl::Fbo::Format const & sourceFmt, ci::gl::Fbo::Format const & destFmt) {
		return std::shared_ptr<DoubleFbo>(new DoubleFbo(width, height, sourceFmt, destFmt));
	}

	ci::gl::FboRef getSource() { return mSource; }
	ci::gl::FboRef getDest() { return mDest; }
	void bindSource(uint8_t textureUnit = 0);
	void bindDest();
	void unbindSource(uint8_t textureUnit = 0);
	void unbindDest();
	void update(uint8_t textureUnit = 0);
	void swap();

protected:
	DoubleFbo(int width, int height, ci::gl::Fbo::Format const & sourceFmt, ci::gl::Fbo::Format const & destFmt)
	: mWidth(width), mHeight(height), mSource(ci::gl::Fbo::create(width, height, sourceFmt)), mDest(ci::gl::Fbo::create(width, height, destFmt)) {}

private:
	int mWidth, mHeight;
	ci::gl::FboRef mSource, mDest;
};

class ScopedDoubleFbo : private ci::Noncopyable {
public:
	ScopedDoubleFbo(DoubleFboRef const & doubleFbo, uint8_t textureUnit = 0);
	~ScopedDoubleFbo();

private:
	ci::gl::Context * mCtx;
	uint8_t mTextureUnit;
};