#include "goalFinder.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <vector>
#include <curses.h>
#include <string>
using namespace std;
using namespace cv;

#define INI_FILE_PATH "data/config.ini"

int main(int argc, char **argv)
{
    VideoCapture cap;
    string namaVideo = "Video Test/crossbar(360).mp4";
    int nomorKamera = argv[1][0] - '0';
    cout << nomorKamera << endl;
    if (!cap.open(nomorKamera))
    {
        cout << "Gagal buka";
        return 0;
    }
    else
    {
        initscr();
        clear();
        refresh();
        Mat gambar;
        minIni *ini = new minIni(INI_FILE_PATH);
        GoalPerceptor::GetInstance()->init(ini);
        //positionChecker pC;
        //cap >> gambar;
        //pC.setGambar(gambar);
        //pC.init();
        for (;;)
        {
            cap >> gambar;
            GoalPerceptor::GetInstance()->process(gambar);
            /*
            gF.setGambar(gambar);
            //Pre-Processing
            gF.morphOps();
            gF.gabor();
            gF.canny();
            //Cari garis
            gF.HLP();
            gF.adaGawang();
            gF.showData();  
            gF.showImage();
            */
            /*
            pC.setGambar(gambar);
            pC.filterGambar();
            pC.getMinMaxLap();
            pC.showImage();
            pC.showDataInConsole();
            */
            
            if (waitKey(40) == 27)
                break;
            refresh();
        }
        GoalPerceptor::GetInstance()->saveINI(ini);
        endwin();
    }
}