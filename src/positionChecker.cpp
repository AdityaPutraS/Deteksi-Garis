#include <iostream>
#include <vector>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "include/positionChecker.h"
#include <curses.h>

using namespace std;
using namespace cv;

//Constructor
positionChecker::positionChecker() {}

positionChecker::~positionChecker() {}

//Setter
void positionChecker::setGambar(Mat x)
{
    x.copyTo(gambar);
    cvtColor(gambar, gambar, CV_BGR2YUV);
}

void positionChecker::init()
{
    initVariable();
    initWindow();
    initTrackbar();
}

void positionChecker::on_trackbar(int, void *) {}

void positionChecker::initVariable()
{
    meanColor = 0;
    minY = 88;
    maxY = 255;
    minU = 0;
    maxU = 255;
    minV = 0;
    maxV = 255;
    thetaGabor = 0;
    minXLap = 0;
    minYLap = 0;
    maxXLap = gambar.cols;
    maxYLap = gambar.rows;
}

void positionChecker::initWindow()
{
    namedWindow("Asli");
    namedWindow("Filter");
    namedWindow("Properties");
}
void positionChecker::initTrackbar()
{
    createTrackbar("Min Y", "Properties", &minY, 255, on_trackbar);
    createTrackbar("Min U", "Properties", &minU, 255, on_trackbar);
    createTrackbar("Min V", "Properties", &minV, 255, on_trackbar);
    createTrackbar("Max Y", "Properties", &maxY, 255, on_trackbar);
    createTrackbar("Max U", "Properties", &maxU, 255, on_trackbar);
    createTrackbar("Max V", "Properties", &maxV, 255, on_trackbar);
}

void positionChecker::showImage()
{
    imshow("Asli", gambar);
    imshow("Filter", hasilFilter);
}

void positionChecker::showDataInConsole()
{
    clear();
    mvprintw(1, 0, "===================================================");
    mvprintw(2, 15, "Data Position Checker");
    mvprintw(3, 0, "===================================================");
    mvprintw(5, 1, "Y : %d - %d", minY, maxY);
    mvprintw(6, 1, "U : %d - %d", minU, maxU);
    mvprintw(7, 1, "V : %d - %d", minV, maxV);
    mvprintw(9, 1, "Maks Average Baris : %.2f", tempAVGBaris);
    mvprintw(10, 1, "MinY-MaxY : %d  -  %d", minYLap, maxYLap);
    mvprintw(12, 1, "Maks Average Kolom : %.2f", tempAVGKolom);
    mvprintw(13, 1, "MinX-MaxX : %d  -  %d", minXLap, maxXLap);
    refresh();
}

void positionChecker::filterGambar()
{
    gambar.copyTo(hasilFilter);
    Mat strElemVertikal = getStructuringElement(CV_SHAPE_RECT, Size(1, 5 + 1));
    for (int i = 0; i < 3; i++)
    {
        erode(hasilFilter, hasilFilter, strElemVertikal);
    }
    for (int i = 0; i < 2; i++)
    {
        dilate(hasilFilter, hasilFilter, strElemVertikal);
    }
    inRange(hasilFilter, Scalar(minY, minU, minV), Scalar(maxY, maxU, maxV), hasilFilter);
}

void getMaxAvg(vector<long int> arr,int* idxAwal, int* idxAkhir,long double* maks,int k)
{
    int n = arr.size();
    int *tempSum = new int[n]; 
    tempSum[0] = arr[0]; 
    for (int i=1; i<n; i++) 
       tempSum[i] = tempSum[i-1] + arr[i]; 

    int max_sum = tempSum[k-1], max_end = k-1; 
    for (int i=k; i<n; i++) 
    { 
        int curr_sum = tempSum[i] - tempSum[i-k]; 
        if (curr_sum > max_sum) 
        { 
            max_sum = curr_sum; 
            max_end = i; 
        } 
    } 
    delete [] tempSum;
    *maks = max_sum/k;
    *idxAwal = max_end - k + 1;
    *idxAkhir = max_end;

}

void positionChecker::getMinMaxLap()
{
    vector<long int> sumBaris, sumKolom;
    int baris = hasilFilter.rows;
    int kolom = hasilFilter.cols;
    for (int i = 0; i < baris; i++)
    {
        long int sum = 0;
        for (int j = 0; j < kolom; j++)
        {
            Vec3b pixel = hasilFilter.at<uchar>(Point(j,i));
            if (pixel.val[0] > (255 / 2))
            {
                sum -= 255;
            }
            else
            {
                sum += 255;
            }
        }
        sumBaris.push_back(sum);
    }
    long double tempMax = 0;
    for(int i = baris/8;i <= baris; i++)
    {
        long double temp;
        int tMin,tMax;
        getMaxAvg(sumBaris,&tMin,&tMax,&temp,i);
        if(temp > tempMax)
        {
            minYLap = tMin;
            maxYLap = tMax;
            tempMax = temp;
        }
    }
    tempAVGBaris = tempMax;
    line(gambar,Point(0,minYLap),Point(500,minYLap),Scalar(0,0,255),2);
    line(gambar,Point(0,maxYLap),Point(500,maxYLap),Scalar(0,0,255),2);
    for (int j = 0; j < kolom; j++)
    {
        long int sum = 0;
        for (int i = 0; i < baris; i++)
        {
            Vec3b pixel = hasilFilter.at<uchar>(Point(j,i));
            if (pixel.val[0] > (255 / 2))
            {
                sum -= 255;
            }
            else
            {
                sum += 255;
            }
        }
        sumKolom.push_back(sum);
    }
    tempMax = 0;
    for(int i = kolom/8;i <= kolom; i++)
    {
        long double temp;
        int tMin,tMax;
        getMaxAvg(sumKolom,&tMin,&tMax,&temp,i);
        if(temp > tempMax)
        {
            minXLap = tMin;
            maxXLap = tMax;
            tempMax = temp;
        }
    }
    tempAVGKolom = tempMax;
    line(gambar,Point(minXLap,0),Point(minXLap,700),Scalar(0,255,0),2);
    line(gambar,Point(maxXLap,0),Point(maxXLap,700),Scalar(0,255,0),2);
}