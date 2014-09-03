#ifndef FACETRACK_H
#define FACETRACK_H

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <QObject>
#include <QPixmap>
#include <QQueue>
#include "tracking_defines.h"

#define CASCADE "haarcascade_frontalface_alt2.xml"
#define DATADIR "../code/ressources/"

using namespace std;
using namespace cv;

class Facetrack : public QObject
{
    Q_OBJECT
public:
    struct coord_t
    {
        int x1;
        int y1;
        int x2;
        int y2;
        coord_t(int pX1 = 0, int pY1 = 0, int pX2 = 0, int pY2 = 0);
        coord_t(Rect pRect);
        Rect toRect(void);
    };

private:
    // Link to capture device ie Webcam.
    CvCapture* capture_;

    /* coordinates in pixels in the image of the bottom left and top right
     * rectangle around the face.
     */


    // handlers for image data
    Mat rawFrame_;
    Mat frameCpy_;

    string cascadePath_;
    CascadeClassifier cascade_;

    //store last positions to filter them
    QQueue<coord_t> prevFaces_;
    cv::RotatedRect currentFace_;

    //filters for faces coordinates
    static const int weights_[NB_SAMPLE_FILTER];
    bool newFaceFound_;
    double scale_;
    double fov_;
    head_t head_;

    //GoodfeaturesToTack parameters
    std::vector< cv::Point2f > corners_, last_corners_;
    const uint maxCorners_ = 200;
    const double qualityLevel_ = 0.01;
    const double minDistance_ = 10.0;
    const uint addFeatureDistance_ = 10;
     // mask – The optional region of interest. If the image is not empty (then it
     // needs to have the type CV_8UC1 and the same size as image ), it will specify
     // the region in which the corners are detected
     cv::Mat goodFeaturesMask_;

     // blockSize – Size of the averaging block for computing derivative covariation
     // matrix over each pixel neighborhood, see cornerEigenValsAndVecs()
     const int blockSize_ = 3;

     // useHarrisDetector – Indicates, whether to use operator or cornerMinEigenVal()
     const bool useHarrisDetector_ = false;

     // k – Free parameter of Harris detector
     const double k_ = 0.04;

     const float outlierTreshold_ = 5.0f;

     bool findHead_;
     bool firstFeatures_;
     cv::Mat previous_img, next_img;
     Rect detect_box_, track_box_;
    uint min_features_ = 50;
    uint absoluteMinFeatures_ = 25;

    float expand_roi_ = 1.02;
    const float expand_roi_ini_ = 1.02;

public:
    Facetrack(string pCascadeFile = CASCADE);
    ~Facetrack();

    void init(void);
    void getNewImg(void);
    void showRaw(void);
    void drawFace(void);
    void drawFeatures(void);
    void rescaleFeatures(Rect face_region);
    void showFace(void);
    void detectHead(void);
    QPixmap getPixmap(void);
    void getCoordinates(void);
    void WTLeeTrackPosition ();
    QImage putImage(const Mat& mat);
    bool isNewFace(void);
    cv::Rect getFace(const Mat& img);
    cv::RotatedRect faceFromPoints(void);
    int remove_bad_features(float pStandardDeviationTreshold = 10000);
    void addFeatures(Mat& img);
    bool isRectNonZero(Rect r);
private:
    float distanceToCluster(cv::Point2f testPoint, std::vector< cv::Point2f>& cluster);

signals:
    void signalNewHeadPos(head_t pNewPos);

};

#endif // FACETRACK_H
