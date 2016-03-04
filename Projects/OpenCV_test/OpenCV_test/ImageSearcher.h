#ifndef IMAGESEARCHER_H
#define IMAGESEARCHER_H

#include "ClothArticle.h"
#include "ImageUtilities.h"
#include "Config.h"


#include <iostream>
#include <queue>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>

#include <string>
#include <algorithm>
#include <vector>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <unordered_map>

//using namespace cv;
//using namespace cv::ml;
using namespace std;

vector<string> findClosestNeighbours(vector<ClothArticle*> *allArticles, ClothArticle* query, int n, string fVecType, string filterType);
vector<string> seekUsingImage(string catalogePath, string queryPath, int n);
cv::Mat createFeatureVector(ClothArticle* input);
cv::Mat createFilterVector(cv::Size vecSize, string filtType, float posScale, float negScale);
cv::Ptr<cv::ml::SVM> makeSVMModel(vector<ClothArticle*> *input, string testType);
cv::Ptr<cv::ml::RTrees> makeRTModel(vector<ClothArticle*> *input, string testType);
cv::Ptr<cv::ml::TrainData> createTrainingData(vector<ClothArticle*> *input, string classifierGroup);
float calcEuclDist(cv::Mat fVec1, cv::Mat fVec2, cv::Mat scale);

int backend(string catalogePath, bool embeded, bool loadModel);
void frontend(string catalogePath);

bool t();
cv::Mat oldcreateFeatureVector(ClothArticle* input, string fVecType);
vector<string> oldfindClosestNeighbours(vector<ClothArticle*> *allArticles, ClothArticle* query, int n, string testType);
cv::Ptr<cv::ml::TrainData> oldcreateTrainingData(vector<ClothArticle*> *input, string classifierGroup);
vector<string> oldseekUsingImage(string catalogePath, string queryPath, int n);

#endif