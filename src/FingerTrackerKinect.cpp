#include "FingerTracker.h"

//--------------------------------------------------------------
void FingerTracker::unproject(unsigned short *depth, float *x, float *y, float *z) {
    int u,v;
	const float f = 500.0f;
	const float u0 = 320.0f;
	const float v0 = 240.0f;
	float zCurrent;
    
    #pragma omp parallel for
	for (int i=0; i<640*480; i++) {
		u = i % 640;
		v = i / 640;
		zCurrent = 1.0f / (-0.00307110156374373f * depth[i] + 3.33094951605675f);
		if (z != NULL) {
			z[i] = zCurrent;
		}
		if (x != NULL) {
			x[i] = (u - u0) * zCurrent / f;
		}
		if (y != NULL) {
			y[i] = (v - v0) * zCurrent / f;
		}
	}
}

//--------------------------------------------------------------
vector<cv::Point2i> FingerTracker::detectFingers(cv::Mat1f z, float zMin, float zMax) {
    vector<cv::Point2i> fingers;
    
    cv::Mat handMask = z < zMax & z > zMin;
    std::vector< std::vector<cv::Point> > contours;
    
    cv::findContours(handMask, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    if(contours.size()) {
        for (int i=0; i<contours.size(); i++) {
            vector<cv::Point> contour = contours[i];
            cv::Mat contourMat(contour);
            double area = cv::contourArea(contourMat);
            
            if(area > 3000) {
                cv::Scalar center = cv::mean(contourMat);
                cv::Point centerPoint(center.val[0], center.val[1]);
                vector<cv::Point> approxCurve;
                cv::approxPolyDP(contourMat, approxCurve, 20, true);
                vector<int> hull;
                cv::Mat temp(approxCurve);
                cv::convexHull(temp, hull);
                
                int upper=640, lower=0;
                #pragma omp parallel for
                for(int j=0;j<hull.size();j++) {
                    int idx = hull[j];
					if (approxCurve[idx].y < upper) upper = approxCurve[idx].y;
					if (approxCurve[idx].y > lower) lower = approxCurve[idx].y;
                }
                
                float cutoff = lower - (lower-upper)*0.1f;
                
                #pragma omp parallel for
                for (int j=0; j<hull.size(); j++) {
					int idx = hull[j];
					int pdx = idx == 0 ? approxCurve.size() - 1 : idx - 1;
					int sdx = idx == approxCurve.size() - 1 ? 0 : idx + 1;
                    
                    cv::Point v1 = approxCurve[sdx] - approxCurve[idx];
                    cv::Point v2 = approxCurve[pdx] - approxCurve[idx];
                    
					float angle = acos( (v1.x*v2.x + v1.y*v2.y) / (norm(v1) * norm(v2)) );
                    
					if (angle < 1 && approxCurve[idx].y < cutoff) {
						int u = approxCurve[idx].x;
						int v = approxCurve[idx].y;
                        
						fingers.push_back(cv::Point2i(u,v));
					}
				}
            }
        }
    }
    return fingers;
}

//--------------------------------------------------------------
ofColor FingerTracker::setColor(int u, int v, int alpha) {
    float h = ((float)u/kinect.width);
    float s = 1.0f;
    float l = 0.5 + 0.25*(float)v/kinect.height;
    unsigned int r, g, b;
    
	if(s == 0) {
		r = l;
		g = l;
		b = l;
	} else {
		double temp1 = 0;
		if(l < .50) {
			temp1 = l*(1 + s);
		} else {
			temp1 = l + s - (l*s);
		}
        
		double temp2 = 2*l - temp1;
        
		double temp3 = 0;
        #pragma omp parallel for
		for(int i = 0 ; i < 3 ; i++) {
			switch(i) {
                case 0: // red
				{
					temp3 = h + .33333;
					if(temp3 > 1)
						temp3 -= 1;
					HSLtoRGB(r,temp1,temp2,temp3);
					break;
				}
                case 1: // green
				{
					temp3 = h;
					HSLtoRGB(g,temp1,temp2,temp3);
					break;
				}
                case 2: // blue
				{
					temp3 = h - .33333;
					if(temp3 < 0)
						temp3 += 1;
					HSLtoRGB(b,temp1,temp2,temp3);
					break;
				}
                default:
				{
                    
				}
			}
		}
	}
	r = (uint)((((double)r)/100)*255);
	g = (uint)((((double)g)/100)*255);
	b = (uint)((((double)b)/100)*255);
    return ofColor(r, g, b, alpha);
}

//--------------------------------------------------------------
void FingerTracker::HSLtoRGB(uint& c, const double& temp1, const double& temp2, const double& temp3) {
    if((temp3 * 6) < 1)
        c = (uint)((temp2 + (temp1 - temp2)*6*temp3)*100);
    else
        if((temp3 * 2) < 1)
            c = (uint)(temp1*100);
        else
            if((temp3 * 3) < 2)
                c = (uint)((temp2 + (temp1 - temp2)*(.66666 - temp3)*6)*100);
            else
                c = (uint)(temp2*100);
}
