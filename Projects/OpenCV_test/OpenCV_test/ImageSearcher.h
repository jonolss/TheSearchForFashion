#ifndef IMAGESEARCHER_H
#define IMAGESEARCHER_H

#include "ClothArticle.h"
#include "ImageUtilities.h"

#include <iostream>
#include <queue>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>

using namespace cv;
using namespace cv::ml;
using namespace std;

vector<string> findClosestNeighbours(vector<ClothArticle*> allArticles, ClothArticle* query, int n, string testType);
vector<string> seekUsingImage(string catalogePath, string queryPath, int n);
Mat createFeatureVector(ClothArticle* input, string testType);
Ptr<SVM> makeSVMModel(vector<ClothArticle*> input, string testType);
Ptr<RTrees> makeRTModel(vector<ClothArticle*> input, string testType);
Ptr<TrainData> createTrainingData(vector<ClothArticle*> input, string classifierGroup);
float calcEuclDist(Mat fVec1, Mat fVec2);


#endif