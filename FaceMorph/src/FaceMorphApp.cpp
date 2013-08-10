#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "ciFaceTrackerThreaded.h"
#include "cinder/Capture.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FaceMorphApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    Capture             mCapture;
    Surface             mCamSurface;
    ciFaceTrackerThreaded       mFaceTracker;
};

void FaceMorphApp::setup()
{
    mFaceTracker.setup();
    mCapture= Capture(640,480);
    mCapture.start();
    
}

void FaceMorphApp::mouseDown( MouseEvent event )
{
}

void FaceMorphApp::update()
{
    if(mCapture.isCapturing() && mCapture.checkNewFrame()) {
        mCamSurface=mCapture.getSurface();
        mFaceTracker.update(toOcv(mCamSurface));
    }
    
}

void FaceMorphApp::draw()
{
	// clear out the window with black
    console()<<toString(getAverageFps())<<endl;
	gl::clear( Color( 0, 0, 0 ) );
    if(mCamSurface){
        gl::draw(gl::Texture(mCamSurface),getWindowBounds());
        mFaceTracker.draw();
    }
    
}

CINDER_APP_NATIVE( FaceMorphApp, RendererGl )
