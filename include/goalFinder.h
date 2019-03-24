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
#include "../src/minIni/minIni.h"
/////////////////////
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

class GoalPerceptor
{
private:
  static GoalPerceptor *m_UniqueInstance;
  Mat gambar, hasilMorph, hasilGabor, hasilCanny, hasilHLT;
  Mat gambarLapangan, hasilMorphLapangan;
  v2f garis;
  v4i horizontal, vertikal, listGaris;
  v3f gradien, titikPotong;
  v2i sudut, kandidatTitikTengahGawang;
  vector<double> cNZ;
  //Parameter Image Processing
  //MorphOps
  int minY, minU, minV, maxY, maxU, maxV;
  int f_minH, f_minS, f_minV, f_maxH, f_maxS, f_maxV;
  int thetaGabor, structSize, sizeVer, erodeCount, dilateCount;
  //Canny
  int apertureSize;
  //Hough Line Transform
  int thresHLT;
  //Hough Corner Detector
  int maxCorner, qLevel, minDis;
  //Deteksi garis ujung
  bool adaGaris;
  //Line Processing
  int epsilon, epsilonHapus;

public:
  //Constructor
  GoalPerceptor();
  //Destructor
  ~GoalPerceptor();
  //Setter
  void setGambar(Mat x);
  void setLapangan(Mat x);
  void init();
  void init(minIni *ini);
  void loadINI(minIni *ini);
  void saveINI(minIni *ini);
  static GoalPerceptor *GetInstance() { return m_UniqueInstance; }

  //Init GUI
  static void on_trackbar(int, void *);
  void initWindow();
  void initTrackbar();

  //Fungsi lainnya
  void showImage();
  //Image processing
  v2i process(Mat m);
  void morphOps(const Mat gambarAwal, const Mat gambarLapangan);
  void morphOpsLapangan(const Mat gambarAwal);
  void gabor(const Mat gambarAwal);
  void canny(const Mat gambarAwal);
  void HLP(const Mat gambarAwal);

  void garisToPoint(const v2f garisAwal);
  void normalisasiGaris();
  void klasifikasiGarisDanTampil();

  void adaGawang();
  //Mencari titik tengah gawang dari kombinasi semua garis vertikal, disimpan di KandidatTitikTengahGawang

  void showData();

};

#endif