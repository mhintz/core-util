#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class _TBOX_PREFIX_App : public App {
public:
	static void prepSettings(Settings * settings);

	void setup() override;
	void keyDown(KeyEvent evt) override;
	void update() override;
	void draw() override;
};

void _TBOX_PREFIX_App::prepSettings(Settings * settings) {
	settings->setTitle("_TBOX_PREFIX_");
	settings->setHighDensityDisplayEnabled();
	settings->setFullScreen();
}

void _TBOX_PREFIX_App::setup() {
}

void _TBOX_PREFIX_App::keyDown(KeyEvent evt) {
	if (evt.getCode() == KeyEvent::KEY_ESCAPE) {
		quit();
	}
}

void _TBOX_PREFIX_App::update() {
}

void _TBOX_PREFIX_App::draw()
{
	gl::clear(Color(0, 0, 0)); 
}

CINDER_APP( _TBOX_PREFIX_App, RendererGl, &_TBOX_PREFIX_App::prepSettings )
