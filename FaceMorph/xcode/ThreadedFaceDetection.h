//
//  ThreadedFaceDetection.h
//  FaceMorph
//
//  Created by Hesam Ohadi on 8/24/13.
//
//


#include "CinderOpenCV.h"
#include "cinder/Thread.h"
#include "cinder/app/AppNative.h"
#include "cinder/Surface.h"
#include "cinder/Area.h"
#include "cinder/Rect.h"
#include "cinder/CinderMath.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class ThreadedFaceDetection
{
  
public:
    ThreadedFaceDetection()
    {
        mRunning = false;
       
    }
    ~ThreadedFaceDetection()
    {
        stop();
    }
    
    void start()
    {
        stop();
        mFaceCascade.load(getAssetPath("haarcascade_frontalface_alt.xml").string());
        mRawImage=Surface(getWindowWidth(), getWindowHeight(), false);
        mRunning = true;
        mThread = ThreadRef( new std::thread( &ThreadedFaceDetection::run, this ) );
    }
    void stop()
    {
        mRunning = false;
        if ( mThread ) {
            mThread->join();
            mThread.reset();
        }
    }
    
    void update( Surface aRawImage)
    {
        mRawImage = aRawImage;
    }
    
    vector<Rectf>& getFaces()
    {
        return mFaces;
    }
    
    vector<Rectf>& getHeads()
    {
        return mHeads;
    }
    
    Rectf enlargeFaceBox (float incPct, const Rectf& aFace) {
        float x=aFace.getX1(); float y=aFace.getY1(); float w=aFace.getWidth(); float h= aFace.getHeight();
        float r = Vec2f(0.0,0.0).distance(Vec2f(w,h)) / 2;  //Computes radius of the center diagonal
        float theta = math<float>::atan2(h,w);  //Computes the angle of the diagonal
        float dx = r*incPct*math<float>::cos(theta); //Finds
        float dy = r*incPct*math<float>::sin(theta);
        return Rectf( x - dx, y - dy, x+ w + dx, y+ h + dy);
    }
    
    void drawFaces(bool drawStroked=true, bool drawEllipse=false)
    {
        for ( vector<Rectf>::iterator iter= mFaces.begin(); iter!=mFaces.end(); ++iter)
        {
            if( drawStroked) gl::drawStrokedRect(*iter);
            else gl::drawSolidRect(*iter);
            
            if (drawEllipse) gl::drawStrokedEllipse(Vec2f( (iter->getX1()+iter->getWidth()/2.0), (iter->getY1()+iter->getHeight()/2.0)), iter->getWidth()/2.0, iter->getHeight()/2.0);
        }
        
       
    }
    
    void drawHeads(bool drawStroked=true, bool drawEllipse=false)
    {
        for ( vector<Rectf>::iterator iter= mHeads.begin(); iter!=mHeads.end(); ++iter)
        {
            if( drawStroked) gl::drawStrokedRect(*iter);
            else gl::drawSolidRect(*iter);
            
            if (drawEllipse) gl::drawStrokedEllipse(Vec2f( (iter->getX1()+iter->getWidth()/2.0), (iter->getY1()+iter->getHeight()/2.0)), iter->getWidth()/2.0, iter->getHeight()/2.0);
        }
        
    }
    
    
private:
    typedef std::shared_ptr<std::thread> ThreadRef;
    
    ThreadRef                   mThread;
    volatile bool               mRunning;
    
    cv::CascadeClassifier       mFaceCascade;
    Surface                     mRawImage;
    vector<Rectf>               mFaces;
    vector<Rectf>               mHeads;
    
    void run()
    {
        while ( mRunning ) {
            // create a grayscale copy of the input image
            float calcScale=2.0;
            cv::Mat grayCameraImage( toOcv( mRawImage, CV_8UC1 ) );
            
            // scale it to half size, as dictated by the calcScale constant
            int scaledWidth = mRawImage.getWidth() / calcScale;
            int scaledHeight = mRawImage.getHeight() / calcScale;
            cv::Mat smallImg( scaledHeight, scaledWidth, CV_8UC1 );
            cv::resize( grayCameraImage, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR );
            
            // equalize the histogram
            cv::equalizeHist( smallImg, smallImg );
            
            // clear out the previously deteced faces & eyes
            
            
            // detect the faces and iterate them, appending them to mFaces
            vector<cv::Rect> faces;
            mFaceCascade.detectMultiScale( smallImg, faces );
            mFaces.clear();
            mHeads.clear();
            for( vector<cv::Rect>::const_iterator faceIter = faces.begin(); faceIter != faces.end(); ++faceIter ) {
                Rectf faceRect( fromOcv( *faceIter ) );
                faceRect *= calcScale;
                mFaces.push_back( faceRect );
                mHeads.push_back( enlargeFaceBox(0.4, faceRect));
                
                // detect eyes within this face and iterate them, appending them to mEyes
            }
            
            
        }
    }
    
  
    
    
};



