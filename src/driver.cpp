#include "include/goalFinder.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <vector>
using namespace std;
using namespace cv;

int main()
{

    VideoCapture cap;
    if (!cap.open("Video Test/crossbar(360).mp4"))
    {
        cout << "Gagal buka";
        return 0;
    }
    else
    {
        //Webcam ada
        Mat gambar;
        goalFinder gF;
        gF.init();
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
            gF.showImage();
            if (waitKey(40) == 27)
                break;
        }
    }
}