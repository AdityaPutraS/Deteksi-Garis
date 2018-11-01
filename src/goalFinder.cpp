#include <iostream>
#include <vector>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "include/goalFinder.h"

using namespace std;
using namespace cv;

//Constructor
goalFinder::goalFinder() {}

goalFinder::~goalFinder() {}

//Setter
void goalFinder::setGambar(Mat x)
{
    x.copyTo(gambar);
    //cvtColor(gambar, gambar, CV_BGR2HSV);
}

void goalFinder::init()
{
    minY = 215;
    maxY = 255;
    minU = 0;
    maxU = 255;
    minV = 0;
    maxV = 255;
    sizeVer = 3;
    erodeCount = 2;
    dilateCount = 3;
    thresHLT = 56;
    structSize = 2;
    epsilonHapus = 50;
    apertureSize = 0;
    thetaGabor = 0;
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
    //createTrackbar("Min U", "Properties", &minU, 255, on_trackbar);
    //createTrackbar("Min V", "Properties", &minV, 255, on_trackbar);
    createTrackbar("Max Y", "Properties", &maxY, 255, on_trackbar);
    //createTrackbar("Max U", "Properties", &maxU, 255, on_trackbar);
    //createTrackbar("Max V", "Properties", &maxV, 255, on_trackbar);
    //createTrackbar("Structure Size", "Properties", &structSize, 7, on_trackbar);
    createTrackbar("Structure Size Vertikal", "Properties", &sizeVer, 10, on_trackbar);
    createTrackbar("Banyak Erosi", "Properties", &erodeCount, 7, on_trackbar);
    createTrackbar("Banyak Dilasi", "Properties", &dilateCount, 7, on_trackbar);
    createTrackbar("Theta", "Properties", &thetaGabor, 360, on_trackbar);
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
    inRange(hasilMorph, Scalar(minY, 0, 0), Scalar(maxY, 255, 255), hasilMorph);

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
    Mat gaborKernel = getGaborKernel(Size(3, 3), 3, (double)thetaGabor / 360 * CV_2PI, 12, 1);
    filter2D(hasilMorph, hasilGabor, CV_8U, gaborKernel);
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
            Point2f P1 = Point2f(g1[0], g1[1]), P2 = Point2f(g1[2], g1[3]);
            for (int j = i + 1; j < ukuran; j++)
            {
                if (!akanDihapus[j])
                {
                    Vec4i g2 = listGaris[j];
                    Point2f P3 = Point2f(g2[0], g2[1]), P4 = Point2f(g2[2], g2[3]);
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
            line(hasilHLT,P1,P2,Scalar(255,0,0),4);
        }
        else
        {
            if (abs(P1.y - P2.y) < 100)
            {
                //Horizontal
                horizontal.push_back(g1);
                line(hasilHLT,P1,P2,Scalar(0,255,0),4);
            }else{
                line(hasilHLT,P1,P2,Scalar(255,255,255),4);
            }
        }
    }
    cout << "Vertikal : "<< vertikal.size() << endl;
    cout << "Horizontal : "<< horizontal.size() << endl;
    cout << "Total Garis : "<< listGaris.size() << endl;
}


v4i goalFinder::adaGawang()
{
}