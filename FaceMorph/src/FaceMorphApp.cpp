#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "ciFaceTrackerThreaded.h"
#include "cinder/Capture.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/Utilities.h"
#include "ThreadedFaceDetection.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FaceMorphApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    Capture                     mCapture;
    Surface                     mCamSurface;
    ciFaceTrackerThreaded       mFaceTracker;
    ThreadedFaceDetection       mFaceDetector;
};

void FaceMorphApp::setup()
{
    mFaceTracker.setup();
    mFaceDetector.start();
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
        mFaceDetector.update(mCamSurface);
    }
    
}

void FaceMorphApp::draw()
{
	// clear out the window with black
    console()<<toString(getAverageFps())<<endl;
	gl::clear( Color( 0, 0, 0 ) );
    gl::color (Colorf(1.0,1.0,1.0));
    gl::enableAlphaBlending();
    if(mCamSurface){
       
        //mFaceTracker.draw();
        gl::Texture::Format format;
        format.setTargetRect();
        gl::Texture tex= gl::Texture(mCamSurface, format);
         gl::draw(tex,getWindowBounds());
        
        //gl::draw(mFaceTracker.getImageMesh());
        
        //gl::draw(mFaceTracker.getObjectMesh());
        gl::setMatricesWindowPersp(getWindowWidth(), getWindowHeight());
        //gl::translate(100,100);
        //gl::scale(5,5,5);
        tex.enableAndBind();
        //gl::draw(mFaceTracker.getImageMesh());
        tex.unbind();
        //gl::draw(mFaceTracker.getImageFeature(ciFaceTracker::ALL_FEATURES));
    }
    gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
    
    gl::color( ColorAf(CM_HSV, .1, 1.0, 1.0, 0.2));
    mFaceDetector.drawFaces(false, true);
    gl::color( ColorAf(CM_HSV, .4, 1.0, 1.0, 0.2));
    mFaceDetector.drawHeads(false, true);

    
}

CINDER_APP_NATIVE( FaceMorphApp, RendererGl )
