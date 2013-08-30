//
//  ThreadedFaceTracker.h
//  FaceMorph
//
//  Created by Hesam Ohadi on 8/29/13.
//
//

#ifndef FaceMorph_ThreadedFaceTracker_h
#define FaceMorph_ThreadedFaceTracker_h



#endif

#include "ciFaceTrackerThreaded.h"
#include "cinder/Area.h"

class ThreadedFaceTracker : public ciFaceTrackerThreaded

{
public:
    Rectf       mFace;
    Rectf       mHead;
    float       mEnlargementValue;
    
    ThreadedFaceTracker() : ciFaceTrackerThreaded()
    {
        mEnlargementValue=0.7;
    }
    
    Rectf enlargeFaceBox (float incPct, const Rectf& aFace) {
        float x=aFace.getX1(); float y=aFace.getY1(); float w=aFace.getWidth(); float h= aFace.getHeight();
        float r = Vec2f(0.0,0.0).distance(Vec2f(w,h)) / 2;  //Computes radius of the center diagonal
        float theta = math<float>::atan2(h,w);  //Computes the angle of the diagonal
        float dx = r*incPct*math<float>::cos(theta); //Finds
        float dy = r*incPct*math<float>::sin(theta);
        return Rectf( x - dx, y - dy, x+ w + dx, y+ h + dy);
    }
    
    bool update(cv::Mat image)
    {
		lock();
		image.copyTo(imageMiddle);
		objectPointsFront = objectPointsMiddle;
		imagePointsFront = imagePointsMiddle;
		meanObjectPointsFront = meanObjectPointsMiddle;
		objectPointsMatFront = objectPointsMatMiddle;
		failed = failedMiddle;
		needsUpdatingFront = true;
		unlock();
		if(!failed) {
			meanObjectPointsReady = true;
		}
        mFace= getImageFeature(ciFaceTracker::FACE_OUTLINE).calcBoundingBox();
        mHead= enlargeFaceBox(mEnlargementValue, mFace);
		return getFound();
	}
    
    Rectf& getFace()
    {
        return mFace;
    }
    
    Rectf& getHead()
    {
        return mHead;
    }
    
    void drawFace(bool drawStroked=true, bool drawEllipse=false)
    {
  
            if( drawStroked) gl::drawStrokedRect(mFace);
            else gl::drawSolidRect(mFace);
            
            if (drawEllipse) gl::drawStrokedEllipse(Vec2f( (mFace.getX1()+mFace.getWidth()/2.0), (mFace.getY1()+mFace.getHeight()/2.0)), mFace.getWidth()/2.0, mFace.getHeight()/2.0);
        
        
        
    }
    
    void drawHead(bool drawStroked=true, bool drawEllipse=false)
    {
        
            if( drawStroked) gl::drawStrokedRect(mHead);
            else gl::drawSolidRect(mHead);
            
            if (drawEllipse) gl::drawStrokedEllipse(Vec2f( (mHead.getX1()+mHead.getWidth()/2.0), (mHead.getY1()+mHead.getHeight()/2.0)), mHead.getWidth()/2.0, mHead.getHeight()/2.0);
        
        
    }
    
};