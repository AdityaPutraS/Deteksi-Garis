/*
 *  Author :
 *  Aditya Putra Santosa
 *  13517013
 */
#include <iostream>
#include <vector>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "goalFinder.h"
#include <curses.h>

using namespace std;
using namespace cv;

GoalPerceptor *GoalPerceptor::m_UniqueInstance = new GoalPerceptor();

////////////////////////////////////
//Bagian GoalPerceptor
//Constructor
GoalPerceptor::GoalPerceptor() {}

GoalPerceptor::~GoalPerceptor() {}

//Setter
void GoalPerceptor::setGambar(Mat x)
{
    x.copyTo(gambar);
    cvtColor(gambar, gambar, CV_BGR2YUV);
}

void GoalPerceptor::setLapangan(Mat x)
{
    x.copyTo(gambarLapangan);
    cvtColor(gambarLapangan, gambarLapangan, CV_BGR2HSV);
}

void GoalPerceptor::init()
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
void GoalPerceptor::init(minIni *ini)
{
    loadINI(ini);
    initWindow();
    initTrackbar();
}

void GoalPerceptor::loadINI(minIni *ini)
{
    int value = 0;
    if ((value = ini->geti("Goal finder", "minY", INVALID_VALUE)) != INVALID_VALUE)
        minY = value;
    if ((value = ini->geti("Goal finder", "maxY", INVALID_VALUE)) != INVALID_VALUE)
        maxY = value;
    if ((value = ini->geti("Goal finder", "minU", INVALID_VALUE)) != INVALID_VALUE)
        minU = value;
    if ((value = ini->geti("Goal finder", "maxU", INVALID_VALUE)) != INVALID_VALUE)
        maxU = value;
    if ((value = ini->geti("Goal finder", "minV", INVALID_VALUE)) != INVALID_VALUE)
        minV = value;
    if ((value = ini->geti("Goal finder", "maxV", INVALID_VALUE)) != INVALID_VALUE)
        maxV = value;
    if ((value = ini->geti("Goal finder", "f_minH", INVALID_VALUE)) != INVALID_VALUE)
        f_minH = value;
    if ((value = ini->geti("Goal finder", "f_maxH", INVALID_VALUE)) != INVALID_VALUE)
        f_maxH = value;
    if ((value = ini->geti("Goal finder", "f_minS", INVALID_VALUE)) != INVALID_VALUE)
        f_minS = value;
    if ((value = ini->geti("Goal finder", "f_maxS", INVALID_VALUE)) != INVALID_VALUE)
        f_maxS = value;
    if ((value = ini->geti("Goal finder", "f_minV", INVALID_VALUE)) != INVALID_VALUE)
        f_minV = value;
    if ((value = ini->geti("Goal finder", "f_maxV", INVALID_VALUE)) != INVALID_VALUE)
        f_maxV = value;
    if ((value = ini->geti("Goal finder", "sizeVer", INVALID_VALUE)) != INVALID_VALUE)
        sizeVer = value;
    if ((value = ini->geti("Goal finder", "erodeCount", INVALID_VALUE)) != INVALID_VALUE)
        erodeCount = value;
    if ((value = ini->geti("Goal finder", "dilateCount", INVALID_VALUE)) != INVALID_VALUE)
        dilateCount = value;
    if ((value = ini->geti("Goal finder", "thresHLT", INVALID_VALUE)) != INVALID_VALUE)
        thresHLT = value;
    if ((value = ini->geti("Goal finder", "structSize", INVALID_VALUE)) != INVALID_VALUE)
        structSize = value;
    if ((value = ini->geti("Goal finder", "epsilonHapus", INVALID_VALUE)) != INVALID_VALUE)
        epsilonHapus = value;
    if ((value = ini->geti("Goal finder", "apertureSize", INVALID_VALUE)) != INVALID_VALUE)
        apertureSize = value;
    if ((value = ini->geti("Goal finder", "thetaGabor", INVALID_VALUE)) != INVALID_VALUE)
        thetaGabor = value;
}

void GoalPerceptor::saveINI(minIni *ini)
{
    ini->put("Goal Finder", "minY", minY);
    ini->put("Goal Finder", "maxY", maxY);
    ini->put("Goal Finder", "minU", minU);
    ini->put("Goal Finder", "maxU", maxU);
    ini->put("Goal Finder", "minV", minV);
    ini->put("Goal Finder", "maxV", maxV);
    ini->put("Goal Finder", "f_minH", f_minH);
    ini->put("Goal Finder", "f_maxH", f_maxH);
    ini->put("Goal Finder", "f_minS", f_minS);
    ini->put("Goal Finder", "f_maxS", f_maxS);
    ini->put("Goal Finder", "f_minV", f_minV);
    ini->put("Goal Finder", "f_maxV", f_maxV);
    ini->put("Goal Finder", "sizeVer", sizeVer);
    ini->put("Goal Finder", "erodeCount", erodeCount);
    ini->put("Goal Finder", "dilateCount", dilateCount);
    ini->put("Goal Finder", "thresHLT", thresHLT);
    ini->put("Goal Finder", "structSize", structSize);
    ini->put("Goal Finder", "epsilonHapus", epsilonHapus);
    ini->put("Goal Finder", "apertureSize", apertureSize);
    ini->put("Goal Finder", "thetaGabor", thetaGabor);
}

void GoalPerceptor::initWindow()
{
    namedWindow("Asli");
    namedWindow("HLT");
    namedWindow("Properties");
    namedWindow("Lapangan_Properties");
    namedWindow("Canny");
    namedWindow("Morph");
    namedWindow("Lapangan");
}
void GoalPerceptor::initTrackbar()
{
    //Morph
    createTrackbar("Min Y", "Properties", &minY, 255, on_trackbar);
    createTrackbar("Min U", "Properties", &minU, 255, on_trackbar);
    createTrackbar("Min V", "Properties", &minV, 255, on_trackbar);
    createTrackbar("Max Y", "Properties", &maxY, 255, on_trackbar);
    createTrackbar("Max U", "Properties", &maxU, 255, on_trackbar);
    createTrackbar("Max V", "Properties", &maxV, 255, on_trackbar);
    createTrackbar("f_Min H", "Lapangan_Properties", &f_minH, 255, on_trackbar);
    createTrackbar("f_Min S", "Lapangan_Properties", &f_minS, 255, on_trackbar);
    createTrackbar("f_Min V", "Lapangan_Properties", &f_minV, 255, on_trackbar);
    createTrackbar("f_Max H", "Lapangan_Properties", &f_maxH, 255, on_trackbar);
    createTrackbar("f_Max S", "Lapangan_Properties", &f_maxS, 255, on_trackbar);
    createTrackbar("f_Max V", "Lapangan_Properties", &f_maxV, 255, on_trackbar);
    //createTrackbar("Structure Size", "Properties", &structSize, 7, on_trackbar);
    // createTrackbar("Structure Size Vertikal", "Properties", &sizeVer, 10, on_trackbar);
    // createTrackbar("Banyak Erosi", "Properties", &erodeCount, 7, on_trackbar);
    // createTrackbar("Banyak Dilasi", "Properties", &dilateCount, 7, on_trackbar);
    createTrackbar("Theta", "Properties", &thetaGabor, 180, on_trackbar);
    //Hough Line Transform
    createTrackbar("Thresold HLT", "Properties", &thresHLT, 200, on_trackbar);
    createTrackbar("Epsilon Hapus", "Properties", &epsilonHapus, 500, on_trackbar);
}
void GoalPerceptor::on_trackbar(int, void *) {}

void GoalPerceptor::showImage()
{
    imshow("Asli", gambar);
    imshow("HLT", hasilHLT);
    imshow("Canny", hasilCanny);
    imshow("Morph", hasilMorph);
    imshow("Gabor", hasilGabor);
    imshow("Lapangan", hasilMorphLapangan);
}

v2i GoalPerceptor::process(Mat m)
{
    GetInstance()->setLapangan(m);
    GetInstance()->morphOpsLapangan(gambarLapangan);
    GetInstance()->setGambar(m);
    GetInstance()->morphOps(gambar, hasilMorphLapangan);
    GetInstance()->gabor(hasilMorph);
    GetInstance()->canny(hasilGabor);
    GetInstance()->HLP(hasilCanny);
    GetInstance()->adaGawang();
    GetInstance()->showData();
    GetInstance()->showImage();
    return kandidatTitikTengahGawang;
}

//Image Processing
void GoalPerceptor::morphOps(const Mat gambarAwal, const Mat lapangan)
{
    gambarAwal.copyTo(hasilMorph);
    //Ambil warna putih saja
    GaussianBlur(hasilMorph, hasilMorph, Size(3, 3), 2, 2);
    inRange(hasilMorph, Scalar(minY, minU, minV), Scalar(maxY, maxU, maxV), hasilMorph);
    // imshow("Morph before",hasilMorph);
    // bitwise_xor(hasilMorph,lapangan,hasilMorph);
    // Mat gambarMask;
    // gambar.copyTo(gambarMask,hasilMorph);
    // cvtColor(gambarMask,gambarMask,CV_YUV2BGR);
    // imshow("Gambar Mask",gambarMask);
    //Erode & Dilatasi gambar1
    //Mat strElem = getStructuringElement(CV_SHAPE_RECT, Size(structSize, structSize), Point(0, 0));
    // Mat strElemVertikal = getStructuringElement(CV_SHAPE_RECT, Size(1, sizeVer + 1));
    // for (int i = 0; i < erodeCount; i++)
    // {
    //     erode(hasilMorph, hasilMorph, strElemVertikal);
    // }
    // for (int i = 0; i < dilateCount; i++)
    // {
    //     dilate(hasilMorph, hasilMorph, strElemVertikal);
    // }
}

void GoalPerceptor::morphOpsLapangan(const Mat gambarAwal)
{
    gambarAwal.copyTo(hasilMorphLapangan);
    GaussianBlur(hasilMorphLapangan, hasilMorphLapangan, Size(3, 3), 2, 2);
    inRange(hasilMorphLapangan, Scalar(f_minH, f_minS, f_minV), Scalar(f_maxH, f_maxS, f_maxV), hasilMorphLapangan);
}

void GoalPerceptor::gabor(const Mat gambarAwal)
{
    Mat hasilGabor1, hasilGabor2;
    Size2i sizeKernel(3, 3);
    Mat gaborKernel1 = getGaborKernel(sizeKernel, 3, (double)thetaGabor / 360 * CV_2PI, 12, 1);
    filter2D(gambarAwal, hasilGabor1, gambarAwal.depth(), gaborKernel1);
    Mat gaborKernel2 = getGaborKernel(sizeKernel, 3, (double)(thetaGabor + 180) / 360 * CV_2PI, 12, 1);
    filter2D(gambarAwal, hasilGabor2, gambarAwal.depth(), gaborKernel2);
    //Mat gaborKernel3 = getGaborKernel(sizeKernel, 3, (double)(thetaGabor+90) / 360 * CV_2PI, 12, 1);
    //filter2D(gambarAwal, hasilGabor, gambarAwal.depth(), gaborKernel3);
    //Mat gaborKernelTotal;
    //bitwise_or(gaborKernel1, gaborKernel2,gaborKernelTotal);
    //bitwise_or(gaborKernel3, gaborKernelTotal, gaborKernelTotal);
    //filter2D(hasilMorph, hasilGabor, hasilMorph.depth(), gaborKernelTotal);
    //addWeighted(hasilGabor1, 1, hasilGabor2, 1, 0, hasilGabor2);
    //addWeighted(hasilGabor,1,hasilGabor2,1,0,hasilGabor);
    bitwise_or(hasilGabor1, hasilGabor2, hasilGabor);
    //bitwise_or(hasilGabor2, hasilGabor, hasilGabor);
    //inRange(hasilGabor, Scalar(minY, minU, minV), Scalar(maxY, maxU, maxV), hasilGabor);
}

void GoalPerceptor::canny(const Mat gambarAwal)
{
    gambarAwal.copyTo(hasilCanny);
    int apSize = 2 * (apertureSize + 1) + 1;
    Canny(hasilCanny, hasilCanny, 0, 0, apSize);
}
void GoalPerceptor::HLP(const Mat gambarAwal)
{
    //HoughLinesP(hasilCanny, garis, 1, CV_PI / 180, thresHLP, minLen, maxGap); //Probabilistic
    this->gambar.copyTo(hasilHLT);
    HoughLines(gambarAwal, garis, 1, CV_PI / 180, thresHLT);
    garisToPoint(garis);
    normalisasiGaris();
    klasifikasiGarisDanTampil();
}

void GoalPerceptor::garisToPoint(const v2f garisAwal)
{
    listGaris.clear();
    for (auto g1 : garisAwal)
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

void GoalPerceptor::normalisasiGaris()
{
    //Normalisasi garis vertikal hasil hough line transform
    //Mengganti semua garis yang dekat menjadi 1
    //Definisi dekat adalah deltaX titik atas garis dan titik bawah garis <= epsilon
    //Atau bisa juga deltaY titik kiri garis dan titik kanan garis <= epsilon
    long int ukuran = listGaris.size();
    bool akanDihapus[ukuran];
    memset(akanDihapus, false, ukuran);
    for (int i = 0; i < ukuran; i++)
    {
        if (!akanDihapus[i])
        {
            //Taruh koordinat di titik sementara
            Vec4i g1 = listGaris[i];
            Point2i P1Ver, P2Ver;
            //Untuk cek vertikal
            if (g1[1] < g1[3])
            {
                P1Ver = Point2f(g1[0], g1[1]), P2Ver = Point2f(g1[2], g1[3]);
            }
            else
            {
                P1Ver = Point2f(g1[2], g1[3]), P2Ver = Point2f(g1[0], g1[1]);
            }
            //Untuk cek horizontal
            Point2i P1Hor, P2Hor;
            if (g1[0] < g1[2])
            {
                P1Hor = Point2f(g1[0], g1[1]), P2Hor = Point2f(g1[2], g1[3]);
            }
            else
            {
                P1Hor = Point2f(g1[2], g1[3]), P2Hor = Point2f(g1[0], g1[1]);
            }
            for (int j = i + 1; j < ukuran; j++)
            {
                if (!akanDihapus[j])
                {
                    Vec4i g2 = listGaris[j];
                    //Untuk cek vertikal
                    Point2f P3Ver, P4Ver;
                    if (g2[1] < g2[3])
                    {
                        P3Ver = Point2f(g2[0], g2[1]), P4Ver = Point2f(g2[2], g2[3]);
                    }
                    else
                    {
                        P3Ver = Point2f(g2[2], g2[3]), P4Ver = Point2f(g2[0], g2[1]);
                    }
                    //Untuk cek horizontal
                    Point2f P3Hor, P4Hor;
                    if (g2[1] < g2[3])
                    {
                        P3Hor = Point2f(g2[0], g2[1]), P4Hor = Point2f(g2[2], g2[3]);
                    }
                    else
                    {
                        P3Hor = Point2f(g2[2], g2[3]), P4Hor = Point2f(g2[0], g2[1]);
                    }
                    //Cari delta X dari kedua garis vertikal
                    double deltaGarisAtas = abs(P1Ver.x - P3Ver.x), deltaGarisBawah = abs(P2Ver.x - P4Ver.x);
                    double deltaGarisKiri = abs(P1Hor.y - P3Hor.y), deltaGarisKanan = abs(P2Hor.y - P4Hor.y);
                    if (
                        (deltaGarisAtas <= epsilonHapus && deltaGarisBawah <= epsilonHapus) ||
                        (deltaGarisKiri <= epsilonHapus && deltaGarisKanan <= epsilonHapus))
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

void GoalPerceptor::klasifikasiGarisDanTampil()
{
    vertikal.clear();
    horizontal.clear();
    cNZ.clear();
    long int ukuran = listGaris.size();
    Mat cekSentuhLapangan;
    bitwise_xor(hasilMorph, hasilMorphLapangan, cekSentuhLapangan);
    for (int i = 0; i < ukuran; i++)
    {
        //Taruh koordinat di titik sementara
        Vec4i g1 = listGaris[i];
        Point2f P1 = Point2f(g1[0], g1[1]), P2 = Point2f(g1[2], g1[3]);
        if (abs(P1.x - P2.x) < 100)
        {
            //Vertikal
            //Cek brp persen putihnya
            int lebar = abs(P1.x - P2.x) + 50;
            int tinggi = cekSentuhLapangan.rows;
            int awalX = (P1.x + P2.x) / 2;
            if (awalX + lebar <= cekSentuhLapangan.cols && awalX >= 50)
            {
                Rect boundingBox(awalX - 50, 0, lebar, tinggi);
                Mat maskGaris = cekSentuhLapangan(boundingBox);
                int countNZero = countNonZero(maskGaris);
                int countMax = lebar * tinggi;
                double persentase = (double)countNZero / countMax;
                if (persentase > 0.75)
                {
                    cNZ.push_back(persentase);
                    vertikal.push_back(g1);
                    line(hasilHLT, P1, P2, Scalar(255, 0, 0), 4);
                }
                else
                {
                    line(hasilHLT, P1, P2, Scalar(0, 0, 255), 4);
                }
            }
            else
            {
                line(hasilHLT, P1, P2, Scalar(255, 0, 255), 4);
            }
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

void GoalPerceptor::showData()
{
    //clear();
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
        mvprintw(6 + i, 15, "%d %d %d %d, %.3f", garis[0], garis[1], garis[2], garis[3], cNZ[i]);
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

void GoalPerceptor::adaGawang()
{
    int sumX = 0;
    for (int i = 0; i < vertikal.size(); i++)
    {
        int xG1 = (vertikal[i][0] + vertikal[i][2]) / 2;
        int yG1 = 240;
        for (int j = i + 1; j < vertikal.size(); j++)
        {
            int xG2 = (vertikal[j][0] + vertikal[j][2]) / 2;
            kandidatTitikTengahGawang.push_back(Vec2i((xG1 + xG2) / 2, yG1));
            sumX += (xG1 + xG2) / 2;
            circle(hasilHLT, Point2i((xG1 + xG2) / 2, yG1), 7, Scalar(255, 0, 0), 2);
        }
    }
    stringstream ss;
    ss << sumX;
    clear();
    mvprintw(30,5,ss.str().c_str());
    if (kandidatTitikTengahGawang.size() > 0)
    {
        circle(hasilHLT, Point2i(sumX / kandidatTitikTengahGawang.size(), 240), 10,
               Scalar(100, 100, 100), 3);
    }
}