#include "facetrack.h"
#include <iostream>
#include <iterator>
#include <cctype>
#include <math.h>
#include <QDebug>

#include <set>

#include "opencv2/video/tracking.hpp"
#define WEBCAM_WINDOW "webcam"

using namespace std;


Facetrack::coord_t::coord_t(int pX1, int pY1, int pX2, int pY2)
{
    x1 = pX1;
    y1 = pY1;
    x2 = pX2;
    y2 = pY2;
}

Facetrack::coord_t::coord_t(Rect pRect)
{
    x1 = pRect.x;
    y1 = pRect.y;
    x2 = pRect.x + pRect.width;
    y2 = pRect.y + pRect.height;
}

Rect Facetrack::coord_t::toRect(void)
{
    Rect res;
    res.x = x1;
    res.y = y1;
    res.width = x2-x1;
    res.height = y2-y1;
    return res;
}

/*only one for all the instances
 * all ones = simple average
 */
const int Facetrack::weights_[NB_SAMPLE_FILTER]= {1,1,1,1,1,1,1,1,1,1};

Facetrack::Facetrack(string pCascadeFile)
    :capture_(0),
      cascadePath_(pCascadeFile),
      currentFace_(0,0,0,0),
      newFaceFound_(false),
      scale_(MOVE_SCALE),
      fov_(WEBCAM_FOV),
      findHead_(true),
      firstFeatures_(true)
{
    head_.x = 0;
    head_.z = 5.0;
    head_.y = 0;
}

Facetrack::~Facetrack()
{
    cvReleaseCapture(&capture_);
}

void Facetrack::init(void)
{
    capture_ = cvCaptureFromCAM(0);

    // try to open 2 different cams, else fail.
    // 0 = embedded cam like in laptops or usb cam if its the only one
    // 1 = usb cam
    if ( !capture_ )
    {
        capture_ = cvCaptureFromCAM(1);
        if ( !capture_ )
            throw string("Couldn't open webcam, device busy.\nTry closing other webcam apps or reboot");
    }

    string path = DATADIR;
    path += cascadePath_;
    if (!cascade_.load(path))
    {
        throw string("Cascade file not found: ") + string(path);
    }
}

void Facetrack::showRaw(void)
{
    imshow(WEBCAM_WINDOW, rawFrame_);
}

void Facetrack::drawFace(void)
{
    Point center;
    Scalar color =  CV_RGB(0,255,0);
    int radius;
    Rect face = currentFace_.toRect();
    double aspect_ratio = (double)face.width/face.height;
    if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )
    {
    center.x = cvRound((face.x + face.width*0.5));
    center.y = cvRound((face.y + face.height*0.5));
    radius = cvRound((face.width + face.height)*0.25);
    circle( frameCpy_, center, radius, color, 3, 8, 0 );
    }
    else
    rectangle(frameCpy_, cvPoint(cvRound(face.x), cvRound(face.y)),
              cvPoint(cvRound(face.x + face.width-1),
                      cvRound(face.y + face.height-1)),
              color, 3, 8, 0);
    drawFeatures();
}

void Facetrack::drawFeatures(void){
    for( size_t i = 0; i < corners_.size(); i++ )
    {
        cv::circle(frameCpy_, corners_[i], 2, cv::Scalar( 255. ),-1);
    }
}

QPixmap Facetrack::getPixmap(void)
{
    return QPixmap::fromImage(putImage(frameCpy_));
}

void Facetrack::showFace(void)
{
    imshow(WEBCAM_WINDOW, frameCpy_);
}

void Facetrack::getNewImg(void)
{
    IplImage* iplImg = cvQueryFrame(capture_);
    rawFrame_ = iplImg;
    if( !rawFrame_.empty() )
    {
        if( iplImg->origin == IPL_ORIGIN_TL )
            rawFrame_.copyTo( frameCpy_ );
        else
            flip( rawFrame_, frameCpy_, 0 );
    }
}

float Facetrack::distanceToCluster(cv::Point2f testPoint, std::vector< cv::Point2f>& cluster){
    int minDistance = 10000;
    for(auto& point: cluster){
        if (point == testPoint)
            continue;
        int distance = abs(testPoint.x - point.x) + abs(testPoint.y - point.y);
        if (distance < minDistance)
            minDistance = distance;
    }
    return minDistance;
}

//remove bad features that are too far from the cluster
//base on the mean squared error and standard deviation
void Facetrack::remove_bad_features(float pStandardDeviationTreshold){
    Point2f avg(0,0);
    for (auto& f: corners_){
        avg += f;
    }
    avg.x /= corners_.size();
    avg.y /= corners_.size();

    float se = 0, mse = 0; //mean squared error
    for (auto& f: corners_){
       se += (f.x-avg.x)*(f.x-avg.x) + (f.y-avg.y)*(f.y-avg.y);
    }
    mse = se / corners_.size();

    for (size_t i = 0; i < corners_.size(); ++i){
        Point2d f = corners_[i];
        float std_err = ((f.x-avg.x)*(f.x-avg.x) + (f.y-avg.y)*(f.y-avg.y))/mse;
        if(std_err > pStandardDeviationTreshold){
            corners_.erase(corners_.begin()+i); //should use a list for efficient deletion
        }
    }
}

//fit the face ellipse from the feature points
Rect Facetrack::faceFromPoints(void){
    Rect face;
    //compute average of all feature points
    Point2f avg(0,0), min(-1,-1), max(-1,-1);
    for (auto& f: corners_){
        avg += f;
        if (min.x == -1 or min.x > f.x)
            min.x = f.x;
        if (max.x == -1 or max.x < f.x)
            max.x = f.x;
        if (min.y == -1 or min.y > f.y)
            min.y = f.y;
        if (max.y == -1 or max.y < f.y)
            max.y = f.y;
    }
    avg.x /= corners_.size();
    avg.y /= corners_.size();
    int dx = (int)(max.x - min.x);
    int dy = (int)(max.y - min.y);
    face.x = avg.x-dx/2;
    face.y = avg.y-dx/2;
    face.width = dx;
    face.height = dy;
    return face;
}

/*features were detected in the face region, need
 * to translate coordinates back in original image
*/
void Facetrack::rescaleFeatures(Rect face_region)
{
    for(auto& p : corners_){
        p.x += face_region.x;
        p.y += face_region.y;
    }
}

void Facetrack::addFeatures(Mat& img){
    int w = track_box_.width*expand_roi_;
    int h = track_box_.height*expand_roi_;
    cv::Rect roiBox(track_box_.x, track_box_.y, w, h);

    Mat roi = img(roiBox).clone();
    std::vector< cv::Point2f > corners;

    goodFeaturesToTrack(roi,
                        corners,
                        maxCorners_,
                        qualityLevel_,
                        minDistance_,
                        goodFeaturesMask_,
                        blockSize_,
                        useHarrisDetector_,
                        k_ );
    for (auto& corner: corners){
        int distance = distanceToCluster(corner, corners_);
        if (distance < addFeatureDistance_){
            corners_.push_back(corner);
        }
    }

    //eliminate doubles

    struct ltPoint
    {
      bool operator()(const cv::Point2f &T1, const cv::Point2f &T2) const
      {
        return(T1.x < T2.x);
      }
    };

    std::set<cv::Point2f, ltPoint> features(corners_.begin(), corners_.end());
    corners_.clear();
    for(auto point : features){
        corners_.push_back(point);
    }
}

void Facetrack::detectHead(void)
{
    Mat gray;
    cvtColor( frameCpy_, gray, CV_BGR2GRAY );
    vector<Rect> faces;
    newFaceFound_ = false;
    /*We use the haarcascade classifier
     * only take the first (biggest) face found
     */
    equalizeHist(gray,gray);
    if (findHead_){
        cascade_.detectMultiScale( gray, faces,
               1.1, 2, 0
               |CV_HAAR_FIND_BIGGEST_OBJECT
               |CV_HAAR_DO_ROUGH_SEARCH,
               Size(10, 10));
        if (faces.size() > 0){
            findHead_ = false;
            firstFeatures_ = true;
            detect_box_ = faces[0];
        }
    }

    Mat crop = gray(detect_box_).clone();
    //take coordinates of first face found
    if( firstFeatures_ || last_corners_.size() == 0){

        goodFeaturesToTrack(crop,
                            last_corners_,
                            maxCorners_,
                            qualityLevel_,
                            minDistance_,
                            goodFeaturesMask_,
                            blockSize_,
                            useHarrisDetector_,
                            k_ );
        previous_img = crop;
        firstFeatures_ = false;
    }
        next_img = crop;
        vector<uchar> status;
        vector<float> err;
        vector<cv::Point2f> all_corners;
        calcOpticalFlowPyrLK(previous_img,
                             next_img,
                             last_corners_,
                             all_corners,
                             status,
                             err);
        corners_.clear();
        float total_error = 0;
        for(size_t i = 0; i < status.size(); ++i){
            total_error += err[i];
            if (status[i]){
                corners_.push_back(all_corners[i]);
            }
        }
        total_error/=err.size();
        if (total_error > 2.0f){
            findHead_ = true;
        }

        remove_bad_features(2.5f);

        if (corners_.size() < min_features_){
            expand_roi_ = expand_roi_ini_ * expand_roi_;
            addFeatures(next_img);
        }
        last_corners_ = corners_;

        float succes = 0;
        for (auto& s : status){
            if ( s )
                succes++;
        }
        succes = succes*100/status.size();
        if (succes < 80)
            findHead_ = true;

        rescaleFeatures(detect_box_);
        currentFace_ = coord_t(faceFromPoints());
        newFaceFound_ = true;
        WTLeeTrackPosition();
 }

/* Convert the rectangle found in 2D to 3D pos in unit box
 */
// Track head position with Johnny Chung Lee's trig stuff
// XXX: Note that positions should be float values from 0-1024
//      and 0-720 (width, height, respectively).
void Facetrack::WTLeeTrackPosition (void)
{
    /*Find nb of rad/pixel from webcam resolution
     * and webcam field of view (supposed 45° by default)
    */
    int fovWidth = frameCpy_.cols;
    float camW2 = (float)frameCpy_.cols/2;
    float camH2 = (float)frameCpy_.rows/2;
    float radPerPix = (fov_/fovWidth);

    //get the size of the head in degrees (relative to the field of view)
    float dx = (float)(currentFace_.x1 - currentFace_.x2);
    float dy = (float)(currentFace_.y1 - currentFace_.y2);
    float pointDist = (float)sqrt(dx * dx + dy * dy);
    float angle = radPerPix * pointDist / 2.0;

    /* Set the head distance in units of screen size
     * creates more or less zoom
     */
    head_.z = (float)(DEPTH_ADJUST + scale_*((AVG_HEAD_MM / 2) / std::tan(angle)) / (float)SCREENHEIGHT);

    //average distance = center of the head
    float aX = (currentFace_.x1 + currentFace_.x2) / 2.0f;
    float aY = (currentFace_.y1 + currentFace_.y2) / 2.0f;

    // Set the head position horizontally
    head_.x = scale_*((float)sin(radPerPix * (aX - camW2)) * head_.z);
    float relAng = (aY - camH2) * radPerPix;

    // Set the head height
    head_.y = scale_*(-0.5f + (float)sin((float)VERTICAL_ANGLE/ 100.0 + relAng) * head_.z);

    // we suppose in general webcam is above the screen like in most laptops
    if (CAMERA_ABOVE)
        head_.y = head_.y + 0.5f + (float)sin(relAng)*head_.z;

    emit signalNewHeadPos(head_);
}

QImage Facetrack::putImage(const Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else
    {
        throw string("ERROR: Mat could not be converted to QImage.");
    }
}

/*
 * Smooth the movement
 * TODO
 */
void Facetrack::stabilize(Rect pNewFace)
{
    coord_t newFace(pNewFace);
    prevFaces_.push_back(newFace);
    if (prevFaces_.size() > NB_SAMPLE_FILTER)
        prevFaces_.pop_front();

    coord_t result;
    int cumul = 0;

    //compute the weighted average
    for(int i=0; i < prevFaces_.size(); ++i)
    {
        cumul += weights_[i];
        result.x1 += prevFaces_.at(i).x1*weights_[i];
        result.y1 += prevFaces_.at(i).y1*weights_[i];
        result.x2 += prevFaces_.at(i).x2*weights_[i];
        result.y2 += prevFaces_.at(i).y2*weights_[i];
    }

    //don't forget to normalize
    result.x1 /=  cumul;
    result.y1 /= cumul;
    result.x2 /= cumul;
    result.y2 /= cumul;

    currentFace_ = result;
    newFaceFound_ = true;
    WTLeeTrackPosition();
}

bool Facetrack::isNewFace(void)
{
    return newFaceFound_;
}
