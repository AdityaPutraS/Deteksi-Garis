/*
 *  Author :
 *  Aditya Putra Santosa
 *  13517013
 */
#ifndef GOAL_FINDER
#define GOAL_FINDER

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "minIni.h"
#include <vector>

using namespace std;
using namespace cv;

#define INVALID_VALUE -1024
#define v2f vector<Vec2f>
#define v6f vector<Vec6f>
#define v3f vector<Vec3f>
#define v4i vector<Vec4i>
#define v4f vector<Vec4f>
#define v2i vector<Vec2i>

enum state
{
  NO_GAWANG,
  SATU_TIANG,
  DUA_TIANG
};
class goalFinder
{
  private:
    
    static goalFinder* m_UniqueInstance;
    Mat gambar, hasilMorph,hasilGabor, hasilCanny, hasilHLT;
    v2f garis;
    v4i horizontal,vertikal,listGaris;
    v3f gradien, titikPotong;
    v2i sudut;
    //Parameter Image Processing
    //MorphOps
    int minY, minU, minV, maxY, maxU, maxV;
    int thetaGabor,structSize,sizeVer, erodeCount, dilateCount;
    //Canny
    int apertureSize;
    //Hough Line Transform
    int thresHLT;
    //Hough Corner Detector
    int maxCorner, qLevel, minDis;
    //Deteksi garis ujung
    bool adaGaris;
    //Line Processing
    int epsilon,epsilonHapus;

  public:
    //Constructor
    goalFinder();
    //Destructor
    ~goalFinder();
    //Setter
    void setGambar(Mat x);
    void init();
    void init(minIni* ini);
    void loadINI(minIni* ini);
    void saveINI(minIni* ini);
    static goalFinder* GetInstance() { return m_UniqueInstance; }

    //Init GUI
    static void on_trackbar(int, void *);
    void initWindow();
    void initTrackbar();

    //Fungsi lainnya
    void showImage();
    //Image processing
    void process(Mat m);
    void morphOps();
    void gabor();
    void canny();
    void HLP();
    
    void garisToPoint();
    void normalisasiGaris();
    void klasifikasiGarisDanTampil();

    void adaGawang();
    //Mereturn 4-tuple koordinat kotak gawang

    void showData();

    bool adaGarisUjung();
    //ketemu garis ujung
};

#endif