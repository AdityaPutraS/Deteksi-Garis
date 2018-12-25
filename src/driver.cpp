#include "include/goalFinder.h"
#include "include/positionChecker.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <vector>
#include <curses.h>
using namespace std;
using namespace cv;

int main(int argc,  char** argv)
{
    VideoCapture cap;
    string namaVideo = "Video Test/crossbar(360).mp4";
    int nomorKamera = argv[0][0] - '0';
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
        goalFinder gF;
        gF.init();
        //positionChecker pC;
        //cap >> gambar;
        //pC.setGambar(gambar);
        //pC.init();
        for (;;)
        {
            cap >> gambar;
            
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
        endwin();
    }
}