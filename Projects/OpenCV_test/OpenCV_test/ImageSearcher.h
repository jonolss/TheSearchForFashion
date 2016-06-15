#ifndef IMAGESEARCHER_H
#define IMAGESEARCHER_H

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "Config.h"
#include "ClothArticle.h"
#include "ImageUtilities.h"
#include "my_win_utills.h"

#include <aclapi.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>

//#include <string>
//#include <algorithm>
//#include <vector>
//#include <stdio.h>
//#include <strsafe.h>
#include <unordered_map>
#include <queue>

using namespace std;

DWORD WINAPI readingFromFile(LPVOID lpParam);
DWORD WINAPI readingFromFileV2(LPVOID lpParam);

vector<string> findClosestNeighbours(vector<ClothArticle*> *allArticles, ClothArticle* query, int n, vector<string> fVecType, vector<double> fVecVal, string filterType);
//vector<string> findClosestNeighbours(vector<ClothArticle*> *allArticles, ClothArticle* query, int n, vector<string> fVecType, string filterType);
vector<string> findClosestNeighbours(vector<ClothArticle*> *allArticles, ClothArticle* query, int n, string fVecType, string filterType);
vector<string> seekUsingImage(string catalogePath, string queryPath, int n);
cv::Mat createFeatureVector(ClothArticle* input);
cv::Mat createFeatureVector(ClothArticle* input, vector<double> fVecVal);
cv::Mat createFilterVector(cv::Size vecSize, vector<string> filtType, float posScale, float negScale);
cv::Mat createFilterVector(cv::Size vecSize, string filtType, float posScale, float negScale);
cv::Ptr<cv::ml::SVM> makeSVMModel(vector<ClothArticle*> *input, string testType);
cv::Ptr<cv::ml::RTrees> makeRTModel(vector<ClothArticle*> *input, string testType);
cv::Ptr<cv::ml::TrainData> createTrainingData(vector<ClothArticle*> *input, string classifierGroup);
float calcEuclDist(cv::Mat fVec1, cv::Mat fVec2, cv::Mat scale);

int webBackend(string catalogePath);
int backend(string catalogePath, bool embeded);
void frontend(string catalogePath);
int guiFrontend(string catalogePath);

inline bool validPath(const string& path);
void clusterCataloge(vector<ClothArticle*> *cataloge, string filtType);


#endif