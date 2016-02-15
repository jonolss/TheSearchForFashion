#ifndef IMAGEUTILITIES_H
#define IMAGEUTILITIES_H

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat getChannel(Mat src, int channel);
Mat normalizeHist(Mat hist);
Mat get8bitHist(Mat img1D, int numLevels = 256, int minRange = 0, int maxRange = 256);
Mat getHsvHist(Mat img1D, int type, int numLevels = 32, int minRange = 0, int maxRange = 256);
Mat preformGaussianBlur(Mat src);
Mat preformCanny(Mat src);
Mat createlocalEdgeImageHist(Mat edges, int size);

void filterAlphaArtifacts(Mat *img);

#endif