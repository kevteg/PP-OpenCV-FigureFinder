#ifndef ENGINE_H
#define ENGINE_H
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

class engine
{
    public:
        engine(int argc, char **argv);
        ~engine();
        int workImage();
        void filterAplied(Mat &image, Mat &mimage, int kernel, int filtro);
        int options(char charcheckforkeypress, int &kernel, int &filtro, int &thresmin);
        void countFig(Mat grayImage);
        static void trackbarBright(int brightness, void *);
        static void trackbarContrast(int contrast, void *);
        void quality(Mat &edges);
    protected:
        VideoCapture cap;
        Mat image;
        //static Mat imag;
        Mat mimage;
        Mat edges;
        //vector<vector<Point> > contours;
        static int brightness;
        static int contrast;
        CvSeq *contours;
        CvSeq *results;
        CvMemStorage *memo;/**/
        int argc;
        char **argv;
};

#endif // ENGINE_H
