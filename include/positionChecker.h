#ifndef POSITION_CHECKER
#define POSITION_CHECKER

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <vector>

using namespace std;
using namespace cv;

#define v2f vector<Vec2f>
#define v6f vector<Vec6f>
#define v3f vector<Vec3f>
#define v4i vector<Vec4i>
#define v4f vector<Vec4f>
#define v2i vector<Vec2i>

class positionChecker
{
  private:
    Mat gambar,hasilFilter;
    double meanColor;
    int minY, minU, minV, maxY, maxU, maxV;
    int thetaGabor;
    int minXLap,minYLap,maxXLap,maxYLap;
    long double tempAVGBaris,tempAVGKolom;
    Vec4i titikGawang;
  public:
    //Constructor
    positionChecker();
    //Destructor
    ~positionChecker();
    //Setter
    void setGambar(Mat x);
    void init();

    //Init GUI
    static void on_trackbar(int, void *);
    void initVariable();
    void initWindow();
    void initTrackbar();
    
    void showImage();
    void showDataInConsole();

    void filterGambar();
    void getMinMaxLap();
};

void getMaxAvg(vector<long int> arr,int* idxAwal, int* idxAkhir,long int* maks,int k);
#endif