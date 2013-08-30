#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Capture.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/Utilities.h"
#include "cinder/params/Params.h"

#include "ThreadedFaceDetector.h"
#include "ThreadedFaceTracker.h"

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
    ThreadedFaceTracker         mFaceTracker;
    ThreadedFaceDetector        mFaceDetector;
    
    params::InterfaceGl         mControlBoard;
    bool                        mShowFaceTracker, mShowFaceDetector;
};

void FaceMorphApp::setup()
{
    mFaceTracker.setup();
    mFaceDetector.start();
    mCapture= Capture(640,480);
    mCapture.start();
    
    mShowFaceDetector=false;
    mShowFaceTracker=false;
    mControlBoard= params::InterfaceGl ("Controls", Vec2i(200,400));
    mControlBoard.addParam("Show FaceTracker", &mShowFaceTracker);
    mControlBoard.addParam("Show FaceDetector", &mShowFaceDetector);
    mControlBoard.addParam("FaceTracker's Head Enlargement Value", &mFaceTracker.mEnlargementValue, "min=0.0 max=1.0 step=0.05");
    mControlBoard.addParam("FaceDetector's Head Enlargement Value", &mFaceDetector.mEnlargementValue, "min=0.0 max=1.0 step=0.05");
    
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
    
    if(mShowFaceDetector){
        gl::color( ColorAf(CM_HSV, .1, 1.0, 1.0, 0.2));
        mFaceDetector.drawFaces(false, true);
        gl::color( ColorAf(CM_HSV, .4, 1.0, 1.0, 0.2));
        mFaceDetector.drawHeads(false, true);
    }
    
    if(mShowFaceTracker){
        gl::color( ColorAf(CM_HSV, .1, 1.0, 1.0, 0.2));
        mFaceTracker.drawFace(true, true);
        gl::color( ColorAf(CM_HSV, .4, 1.0, 1.0, 0.2));
        mFaceTracker.drawHead(true, true);
    }

    mControlBoard.draw();
}

CINDER_APP_NATIVE( FaceMorphApp, RendererGl )
