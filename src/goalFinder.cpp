#include <iostream>
#include <vector>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "include/goalFinder.h"
#include <curses.h>

using namespace std;
using namespace cv;

//Constructor
goalFinder::goalFinder() {}

goalFinder::~goalFinder() {}

//Setter
void goalFinder::setGambar(Mat x)
{
    x.copyTo(gambar);
    cvtColor(gambar, gambar, CV_BGR2YUV);
}

void goalFinder::init()
{
    minY = 102;
    maxY = 255;
    minU = 0;
    maxU = 255;
    minV = 0;
    maxV = 255;
    sizeVer = 0;
    erodeCount = 2;
    dilateCount = 3;
    thresHLT = 47;
    structSize = 2;
    epsilonHapus = 500;
    apertureSize = 0;
    thetaGabor = 60;
    initWindow();
    initTrackbar();
}
void goalFinder::initWindow()
{
    namedWindow("Asli");
    namedWindow("HLT");
    namedWindow("Properties");
    namedWindow("Canny");
    namedWindow("Morph");
}
void goalFinder::initTrackbar()
{
    //Morph
    createTrackbar("Min Y", "Properties", &minY, 255, on_trackbar);
    createTrackbar("Min U", "Properties", &minU, 255, on_trackbar);
    createTrackbar("Min V", "Properties", &minV, 255, on_trackbar);
    createTrackbar("Max Y", "Properties", &maxY, 255, on_trackbar);
    createTrackbar("Max U", "Properties", &maxU, 255, on_trackbar);
    createTrackbar("Max V", "Properties", &maxV, 255, on_trackbar);
    //createTrackbar("Structure Size", "Properties", &structSize, 7, on_trackbar);
    createTrackbar("Structure Size Vertikal", "Properties", &sizeVer, 10, on_trackbar);
    createTrackbar("Banyak Erosi", "Properties", &erodeCount, 7, on_trackbar);
    createTrackbar("Banyak Dilasi", "Properties", &dilateCount, 7, on_trackbar);
    createTrackbar("Theta", "Properties", &thetaGabor, 180, on_trackbar);
    //Hough Line Transform
    createTrackbar("Thresold HLT", "Properties", &thresHLT, 200, on_trackbar);
    createTrackbar("Epsilon Hapus", "Properties", &epsilonHapus, 500, on_trackbar);
}
void goalFinder::on_trackbar(int, void *) {}

void goalFinder::showImage()
{
    imshow("Asli", gambar);
    imshow("HLT", hasilHLT);
    imshow("Canny", hasilCanny);
    imshow("Morph", hasilMorph);
}
//Image Processing
void goalFinder::morphOps()
{
    gambar.copyTo(hasilMorph);
    //Ambil warna putih saja
    inRange(hasilMorph, Scalar(minY, minU, minV), Scalar(maxY, maxU, maxV), hasilMorph);

    //Erode & Dilatasi gambar
    //Mat strElem = getStructuringElement(CV_SHAPE_RECT, Size(structSize, structSize), Point(0, 0));
    Mat strElemVertikal = getStructuringElement(CV_SHAPE_RECT, Size(1, sizeVer + 1));
    for (int i = 0; i < erodeCount; i++)
    {
        erode(hasilMorph, hasilMorph, strElemVertikal);
    }
    for (int i = 0; i < dilateCount; i++)
    {
        dilate(hasilMorph, hasilMorph, strElemVertikal);
    }
    /*
    for (int i = 0; i < erodeCount; i++)
    {
        erode(hasilMorph, hasilMorph, strElem);
    }
    for (int i = 0; i < dilateCount; i++)
    {
        dilate(hasilMorph, hasilMorph, strElem);
    }
    */
}

void goalFinder::gabor()
{
    Mat hasilGabor1, hasilGabor2,hasilGabor3;
    Mat gaborKernel1 = getGaborKernel(Size(3, 3), 3, (double)thetaGabor / 360 * CV_2PI, 12, 1);
    filter2D(hasilMorph, hasilGabor1, hasilMorph.depth(), gaborKernel1);
    Mat gaborKernel2 = getGaborKernel(Size(3, 3), 3, (double)(thetaGabor + 180) / 360 * CV_2PI, 12, 1);
    filter2D(hasilMorph, hasilGabor2, hasilMorph.depth(), gaborKernel2);
    Mat gaborKernel3 = getGaborKernel(Size(3, 3), 3, (double)(thetaGabor+90) / 360 * CV_2PI, 12, 1);
    filter2D(hasilMorph, hasilGabor3, hasilMorph.depth(), gaborKernel3);
    addWeighted(hasilGabor1, 1, hasilGabor2, 1, 0, hasilGabor);
    //addWeighted(hasilGabor,1,hasilGabor3,1,0,hasilGabor);
    inRange(hasilGabor, Scalar(minY, minU, minV), Scalar(maxY, maxU, maxV), hasilMorph);
}

void goalFinder::canny()
{
    hasilMorph.copyTo(hasilCanny);
    int apSize = 2 * (apertureSize + 1) + 1;
    Canny(hasilCanny, hasilCanny, 0, 0, apSize);
}
void goalFinder::HLP()
{
    gambar.copyTo(hasilHLT);
    //HoughLinesP(hasilCanny, garis, 1, CV_PI / 180, thresHLP, minLen, maxGap); //Probabilistic
    HoughLines(hasilCanny, garis, 1, CV_PI / 180, thresHLT);
    garisToPoint();
    normalisasiGaris();
    klasifikasiGarisDanTampil();
}

void goalFinder::garisToPoint()
{
    listGaris.clear();
    int tx = gambar.size[1];
    int ty = gambar.size[0];
    for (auto g1 : garis)
    {
        float rho = g1[0], theta = g1[1];
        float deltaT = theta;
        Point p1, p2;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        p1.x = cvRound(x0 + 1000 * (-b));
        p1.y = cvRound(y0 + 1000 * (a));
        p2.x = cvRound(x0 - 1000 * (-b));
        p2.y = cvRound(y0 - 1000 * (a));
        listGaris.push_back({p1.x, p1.y, p2.x, p2.y});
    }
}

void goalFinder::normalisasiGaris()
{
    //Normalisasi garis vertikal hasil hough line transform
    //Mengganti semua garis yang menjadi 1
    //Definisi dekat adalah deltaX titik atas garis dan titik bawah garis <= epsilon
    //TODO : Ganti StrukDat listGaris dari Vector jadi Double Linked-List
    //Karena Double Linked List lebih efisien masalah penghapusan
    long int ukuran = listGaris.size();
    bool akanDihapus[ukuran];
    memset(akanDihapus, false, ukuran);
    for (int i = 0; i < ukuran; i++)
    {
        if (!akanDihapus[i])
        {
            //Taruh koordinat di titik sementara
            Vec4i g1 = listGaris[i];
            Point2i P1,P2;
            if(g1[1] < g1[3]){
                P1 = Point2f(g1[0], g1[1]), P2 = Point2f(g1[2], g1[3]);
            }else{
                P1 = Point2f(g1[2], g1[3]), P2 = Point2f(g1[0], g1[1]);
            }
            for (int j = i + 1; j < ukuran; j++)
            {
                if (!akanDihapus[j])
                {
                    Vec4i g2 = listGaris[j];
                    Point2f P3,P4;
                    if(g2[1] < g2[3]){
                        P3 = Point2f(g2[0], g2[1]), P4 = Point2f(g2[2], g2[3]);
                    }else{
                        P3 = Point2f(g2[2],g2[3]), P4 = Point2f(g2[0],g2[1]);
                    }
                    //Cari delta X dari kedua garis vertikal
                    double deltaGarisAtas = abs(P1.x - P3.x), deltaGarisBawah = abs(P2.x - P4.x);
                    if (deltaGarisAtas <= epsilonHapus && deltaGarisBawah <= epsilonHapus)
                    {
                        //Hapus dari vector
                        akanDihapus[j] = true;
                    }
                }
            }
        }
    }

    //Hapus semua garis yang akanDihapus
    for (int i = 0; i < ukuran; i++)
    {
        if (akanDihapus[i])
        {
            listGaris.erase(listGaris.begin() + i);
        }
    }
}

void goalFinder::klasifikasiGarisDanTampil()
{
    vertikal.clear();
    horizontal.clear();
    long int ukuran = listGaris.size();
    for (int i = 0; i < ukuran; i++)
    {
        //Taruh koordinat di titik sementara
        Vec4i g1 = listGaris[i];
        Point2f P1 = Point2f(g1[0], g1[1]), P2 = Point2f(g1[2], g1[3]);
        if (abs(P1.x - P2.x) < 100)
        {
            //Vertikal
            vertikal.push_back(g1);
            line(hasilHLT, P1, P2, Scalar(255, 0, 0), 4);
        }
        else
        {
            if (abs(P1.y - P2.y) < 100)
            {
                //Horizontal
                horizontal.push_back(g1);
                line(hasilHLT, P1, P2, Scalar(0, 255, 0), 4);
            }
            else
            {
                line(hasilHLT, P1, P2, Scalar(255, 255, 255), 4);
            }
        }
    }
}

void goalFinder::showData()
{
    clear();
    mvprintw(1, 0, "===================================================");
    mvprintw(2, 15, "Data Goal Finder");
    mvprintw(3, 0, "===================================================");
    mvprintw(5, 1, "Y : %d - %d", minY, maxY);
    mvprintw(6, 1, "U : %d - %d", minU, maxU);
    mvprintw(7, 1, "V : %d - %d", minV, maxV);
    mvprintw(9, 1, "Vertikal : %d", vertikal.size());
    mvprintw(10, 1, "Horizontal : %d", horizontal.size());
    mvprintw(11, 1, "Total Garis : %d", listGaris.size());
    mvprintw(5, 15, "Data Garis Vertikal");
    for (int i = 0; i < vertikal.size(); i++)
    {
        Vec4i garis = vertikal[i];
        mvprintw(6 + i, 15, "%d %d %d %d", garis[0], garis[1], garis[2], garis[3]);
    }
    mvprintw(5, 40, "Data Garis Horizontal");
    for (int i = 0; i < horizontal.size(); i++)
    {
        Vec4i garis = horizontal[i];
        mvprintw(6 + i, 40, "%d %d %d %d", garis[0], garis[1], garis[2], garis[3]);
    }
    refresh();
    //circle(hasilHLT,Point2i(300,200),7,Scalar(0,0,255),2);
}

void goalFinder::adaGawang()
{
    for(int i = 0;i < vertikal.size();i++)
    {
        Vec4i g1 = vertikal[i];
        int midX = (g1[2]+g1[0])/2;
        for(int j = 0;j < horizontal.size();j++)
        {
            Vec4i g2 = horizontal[i];
            int midY = (g2[3]+g2[1])/2;
            Point2i tipot = Point2i(midX,midY);
            circle(hasilHLT,tipot,5,Scalar(0,0,255),2);
        }
    }
}