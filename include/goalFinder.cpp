#include <iostream>
#include <vector>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "goalFinder.h"

using namespace std;
using namespace cv;

//Constructor
goalFinder::goalFinder(){}

goalFinder::~goalFinder(){}

//Setter
void goalFinder::setGambar(Mat x)
{
    x.copyTo(this->gambar);
    cvtColor(this->gambar,this->gambar, CV_BGR2YUV);
}

void goalFinder::init()
{
    this->thresVal = 180;
    this->maxThres = 255;
    this->erodeCount = 2;
    this->dilateCount = 3;
    this->thresHLT = 150;
    this->structSize = 2;
    this->initWindow();
    this->initTrackbar();
}
void goalFinder::initWindow()
{
    namedWindow("HLT");
    namedWindow("Properties");
    namedWindow("Canny");
    namedWindow("Morph");
}
void goalFinder::initTrackbar()
{
    //Morph
    createTrackbar("Thresold Val", "Properties", &this->thresVal, 255, on_trackbar);
    createTrackbar("Thresold Max", "Properties", &this->maxThres, 255, on_trackbar);
    createTrackbar("Structure Size", "Properties", &this->structSize, 7, on_trackbar);
    createTrackbar("Banyak Erosi", "Properties", &this->erodeCount, 7, on_trackbar);
    createTrackbar("Banyak Dilasi", "Properties", &this->dilateCount, 7, on_trackbar);
    //Canny
    createTrackbar("Aperture Canny", "Properties", &this->apertureSize, 2, on_trackbar);
    //Hough Line Transform
    createTrackbar("Thresold HLT", "Properties", &this->thresHLT, 200, on_trackbar);
}
void goalFinder::on_trackbar(int, void *){}
//Image Processing
void goalFinder::morphOps()
{
    this->gambar.copyTo(this->hasilMorph);
    threshold(this->hasilMorph, this->hasilMorph,this->thresVal,this->maxThres, 0);
    Mat strElem = getStructuringElement(CV_SHAPE_RECT, Size(this->structSize,this->structSize), Point(0, 0));

    for (int i = 0; i < this->erodeCount; i++) //2 bagus
    {
        erode(this->hasilMorph, this->hasilMorph, strElem);
    }
    for (int i = 0; i < this->dilateCount; i++) //4 bagus
    {
        dilate(this->hasilMorph, this->hasilMorph, strElem);
    }
    imshow("Morph", this->hasilMorph);
}

void goalFinder::canny()
{
    //this->hasilMorph.copyTo(this->hasilCanny);
    int apSize = 2 * (this->apertureSize + 1) + 1;
    Canny(this->hasilMorph,this->hasilCanny,0,0, apSize);
    imshow("Canny", hasilCanny);
}
void goalFinder::HLP()
{
    this->gambar.copyTo(this->hasilHLT);
    //HoughLinesP(hasilCanny, garis, 1, CV_PI / 180, thresHLP, minLen, maxGap); //Probabilistic
    HoughLines(this->hasilCanny,this->garis,1,CV_PI/180,this->thresHLT);
    for (auto g1 : this->garis)
    {
        //line(hasilHLP, Point(titik[0], titik[1]), Point(titik[2], titik[3]), Scalar(0, 0, 255), 2);
        float rho = g1[0], theta = g1[1];
        float deltaT = theta;
        if(deltaT>-epsilon && deltaT<epsilon){
            //printf("theta = %.5f",theta);
            Point p1,p2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho,y0 = b*rho;
            p1.x = cvRound(x0+1000*(-b));
            p1.y = cvRound(y0+1000*(a));
            p2.x = cvRound(x0-1000*(-b));
            p2.y = cvRound(y0-1000*(a));
            line(this->hasilHLT,p1,p2,Scalar(0,0,255),1);
        }

    }
    imshow("HLT", this->hasilHLT);
}