#ifndef IMAGEUTILITIES_H
#define IMAGEUTILITIES_H

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

//using namespace cv;
using namespace std;

cv::Mat resizeImg(cv::Mat input, int sizeX = 300, int sizeY = 300);
cv::Mat getChannel(cv::Mat src, int channel);
cv::Mat normalizeHist(cv::Mat hist);
cv::Mat get8bitHist(cv::Mat img1D, int numLevels = 256, int minRange = 0, int maxRange = 256);
cv::Mat getHsvHist(cv::Mat img1D, int type, int numLevels = 32, int minRange = 0, int maxRange = 256);
cv::Mat preformGaussianBlur(cv::Mat src);
cv::Mat preformCanny(cv::Mat src);
cv::Mat createlocalEdgeImageHist(cv::Mat edges, int size);

void filterAlphaArtifacts(cv::Mat *img);


#endif