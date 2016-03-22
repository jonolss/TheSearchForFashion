#ifndef IMAGEUTILITIES_H
#define IMAGEUTILITIES_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <stack>
#include <vector>

#include "Config.h"


using namespace std;

cv::Mat resizeImg(cv::Mat input, int sizeX = 300, int sizeY = 300);
cv::Mat getChannel(cv::Mat src, int channel);
cv::Mat normalizeHist(cv::Mat hist);
cv::Mat get8bitHist(cv::Mat img1D, int numLevels = 32);
cv::Mat getHsvHist(cv::Mat img1D, int type, int numLevels = 32);
cv::Mat preformGaussianBlur(cv::Mat src);
cv::Mat preformCanny(cv::Mat src, double lowThresh, double highThresh);
cv::Mat createlocalEdgeImageHist(cv::Mat edges, int size);
cv::Mat skeletonizeMorph(cv::Mat *binaryImg);
cv::Mat skeletonizeZhangSuen(cv::Mat binaryImg);

void createGradiantHistogram(cv::Mat &src, cv::Mat &dst, int num);
void rotateAndGradiant(cv::Mat &src, cv::Mat &dstX, cv::Mat &dstY, double ang);
void thinning(const cv::Mat& src, cv::Mat& dst);
void onlyBackground(cv::Mat& src, cv::Mat& dst);
int maxHorizontalEdges(cv::Mat edgeImg, int minDist, int minHeight);

void filterAlphaArtifacts(cv::Mat *img);
void saveMat(cv::Mat input, ofstream *outFile);
cv::Mat loadMat(ifstream *inFile);

#endif