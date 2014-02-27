#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ChormusikApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void ChormusikApp::setup()
{
}

void ChormusikApp::mouseDown( MouseEvent event )
{
}

void ChormusikApp::update()
{
}

void ChormusikApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( ChormusikApp, RendererGl )
