#pragma once

#include "cinder/gl/gl.h"

class FboCubeMapLayered;
typedef std::shared_ptr<FboCubeMapLayered> FboCubeMapLayeredRef;

class FboCubeMapLayered {
	public:
	struct Format;

	static FboCubeMapLayeredRef create(int width, int height, const Format & format = Format());

	~FboCubeMapLayered();

	GLuint getId() { return mFboHandle; }
	int getWidth() { return mWidth; }
	int getHeight() { return mHeight; }
	ci::gl::TextureCubeMapRef getColorTex() { return mBackingColor; }
	ci::gl::TextureCubeMapRef getDepthTex() { return mBackingDepth; }

	void bind();
	void unbind();

	struct Format {
		Format();

		Format & color(bool hasColor = true) { mHasColor = hasColor; return *this; }
		Format & colorFormat(ci::gl::TextureCubeMap::Format const & fmt) { mColorFormat = fmt; return *this; }
		Format & depth(bool hasDepth = true) { mHasDepth = hasDepth; return *this; }
		Format & depthFormat(ci::gl::TextureCubeMap::Format const & fmt) { mDepthFormat = fmt; return *this; }

		protected:
		bool mHasColor, mHasDepth;
		ci::gl::TextureCubeMap::Format mColorFormat, mDepthFormat;

		friend class FboCubeMapLayered;
	};

	protected:
	FboCubeMapLayered(int width, int height, const Format & format);
	
	GLuint mFboHandle;
	int mWidth, mHeight;
	ci::gl::TextureCubeMapRef mBackingColor;
	ci::gl::TextureCubeMapRef mBackingDepth;
};