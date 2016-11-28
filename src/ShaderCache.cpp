#include "ShaderCache.hpp"

static gl::GlslProgRef sFacetShader;

gl::GlslProgRef getFacetShader() {
	if (sFacetShader == nullptr) {
		sFacetShader = gl::GlslProg::create(app::loadResource(VS_FACET_SHADER), app::loadResource(FS_FACET_SHADER));
	}
	return sFacetShader;
}
