#include <opencv2/core/core.hpp>
#include "opencv2/core/utility.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <opencv/cv.hpp >

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>

#include "opencv2/features2d.hpp"

#include "ClothArticle.h"

//#include <opencv2/cudaimgproc.hpp>


using namespace cv;
using namespace cv::ml;
using namespace std;


Mat getChannel(Mat src, int channel);
Mat normalizeHist(Mat hist);
Mat get8bitHist(Mat img1D, int numLevels = 256, int minRange = 0, int maxRange = 256);
Mat getHsvHist(Mat img1D, int type, int numLevels = 32, int minRange = 0, int maxRange = 256);
Mat preformGaussianBlur(Mat src);
Mat preformCanny(Mat src);
void filterAlphaArtifacts(Mat *img);
void svm();
void rtrees();
void printHough(Mat input);
void test_ml_black();
ClothArticle* inputParser(string input);
art_color checkColor(string input);
art_clType checkClType(string input);
art_sleeveType checkSleeveType(int input);
Ptr<TrainData> createTrainingData(vector<ClothArticle*> input, string classifierGroup);
void Change2016(string filename, string testType);
void tester_SVM_vs_RF(string filename, string testType);
vector<int> createlocalEdgeImageHist(Mat edges, int size);

int main(int argc, char** argv)
{
	/*
	if (argc != 2 && argc != 3)
	{
		cout << " Usage: display_image ImageToLoadAndDisplay" << endl;
		return -1;
	}
	
	Mat image;
	image = imread(argv[1], IMREAD_UNCHANGED); // Read the file
	*/
	

	//printCanny(image);
	//printGaussianBlur(image);

	/*

	if (!image.data) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	cout << image.channels() << endl;

	filterAlphaArtifacts(&image);

	printCanny(image);

	Mat *ch = (Mat*)calloc(4, sizeof(Mat));
	split(image, ch);

	Mat dst;
	Canny(image, dst, 50, 200, 3);

	cout << dst.channels() << endl;

	namedWindow("Hepp", 1);
	imshow("Hepp", dst);
	
	Mat tmpl;
	tmpl = imread(argv[2], IMREAD_UNCHANGED);
	namedWindow("Template", 1);
	imshow("Template", tmpl);

	Mat res = Mat(image.cols - tmpl.cols + 1, image.rows - tmpl.rows + 1, CV_32FC1);

	cout << res.size << endl;

	//matchTemplate(image, tmpl, res, CV_TM_SQDIFF_NORMED);

	namedWindow("Result", 1);
	imshow("Result", res); 
	
	waitKey(0); // Wait for a keystroke in the window
	*/


	//printHough(image);
	//rtrees();
	//svm();

	//test_ml();

	//test_ml_black();

	//inputParser("PATH");

	//Change2016("rBoS.txt", "Color");

	//svm();

	tester_SVM_vs_RF("rAll.txt", "ClothingType");

	return 0;
}

void tester_SVM_vs_RF(string filename, string testType)
{
	ifstream infile(filename, ios::in);

	vector<ClothArticle*> allArticles;

	string line;
	while (getline(infile, line))
	{
		ClothArticle *tmp = inputParser(line);
		allArticles.push_back(tmp);
	}
	
	int totSize = allArticles.size();
	int partSize = totSize / 10;


	double bestRTHitRatio = 0.0;
	double bestSVMHitRatio = 0.0;

	int totRTHits = 0;
	int totSVMHits = 0;
	for (int t = 0; t < 10;t++)
	{
		vector<ClothArticle*> trainingArticles;
		vector<ClothArticle*> testArticles;

		for (int r = 0; r < totSize; r++)
		{
			if (t == r / partSize)
				testArticles.push_back(allArticles.at(r));
			else
				trainingArticles.push_back(allArticles.at(r));
		}


		cout << "skapa träningsdata" << endl;
		Ptr<TrainData> tDataRT = createTrainingData(trainingArticles, testType);
		Ptr<TrainData> tDataSVM = createTrainingData(trainingArticles, testType);
		cout << "träningsdata skapad" << endl;


		cout << "Träning börjar" << endl;
		Ptr<RTrees> rt = RTrees::create();
		Ptr<SVM> svm = SVM::create();

		rt->setMaxDepth(20);
		rt->setMinSampleCount(20);
		rt->setMaxCategories(20);

		rt->setCalculateVarImportance(false);
		rt->setRegressionAccuracy(0.0f);
		rt->setPriors(Mat());

		rt->train(tDataRT, 0);

		svm->setType(SVM::C_SVC);
		//svm->setKernel(SVM::LINEAR);
		//svm->setKernel(SVM::POLY);
		svm->setKernel(SVM::CHI2);
		svm->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6));
		//svm->setDegree(2.0);
		svm->setGamma(1);
		//svm->trainAuto(tDataSVM);
		svm->train(tDataSVM, 0);

		cout << "Träning är klar" << endl;

		int svmHits = 0;
		int rtHits = 0;

		for (int i = 0; i < testArticles.size(); i++)
		{
			Mat rgbImg = testArticles.at(i)->getImage();
			Mat hsvImg;
			cvtColor(rgbImg, hsvImg, COLOR_BGR2HSV);

			Mat testFeatures;

			if (testType == "Color")
			{
				testFeatures = Mat(32 * 6, 1, CV_32F);
				for (int j = 0; j < 6; j++)
				{
					Mat ch, hs, nhs;
					if (j<3)
					{
						ch = getChannel(rgbImg, j);
						hs = get8bitHist(ch, 32);
						nhs = normalizeHist(hs);
					}
					else
					{
						ch = getChannel(hsvImg, j - 3);
						hs = getHsvHist(ch, j - 3, 32);
						nhs = normalizeHist(hs);
					}

					for (int k = 0; k < 32; k++)
					{
						testFeatures.at<float>(nhs.rows * j + k, 0) = (float)nhs.at<float>(k, 0);
					}
				}
			}
			else if (testType == "ClothingType")
			{
				testFeatures = Mat(/*rgbImg.size().area()*/ 2 * 100, 1, CV_32FC1);
				Mat imgGray;
				cvtColor(rgbImg, imgGray, COLOR_BGR2GRAY);
				Mat binary;

				threshold(imgGray, binary, 248, THRESH_BINARY_INV, THRESH_BINARY);

				binary = binary * 255;
				//namedWindow("Test binary", 1);
				//imshow("Test binary", binary);


				//Mat lapImg;
				//Laplacian(binary, lapImg, 256, 2, 1, 1, 4);

				Mat imgBlur = preformGaussianBlur(imgGray);
				Mat edges = preformCanny(imgBlur);

				//namedWindow("Test Edges", 1);
				//imshow("Test Edges", edges);


				vector<int> tmp = createlocalEdgeImageHist(edges, 30);


				//namedWindow("Test Edges", 1);
				//imshow("Test Edges", edges);


				for (int j = 0; j < tmp.size(); j++)
				{
					testFeatures.at<float>(j, 0) = (float)tmp.at(j);
				}

				imgBlur = preformGaussianBlur(binary);
				edges = preformCanny(imgBlur);

				tmp = createlocalEdgeImageHist(edges, 30);

				//namedWindow("Test Edges2", 1);
				//imshow("Test Edges2", edges);

				for (int j = 0; j < tmp.size(); j++)
				{
					testFeatures.at<float>(j + 100, 0) = (float)tmp.at(j);
				}



				/*
				for (int j = 0; j < edges.rows; j++)
				{
				for (int k = 0; k < edges.cols; k++)
				{
				testFeatures.at<float>(j * edges.cols + k, 0) = (float)edges.at<unsigned char>(j, k);
				}
				}
				*/
			}


			Mat testFeatures2;
			if(testType == "Color")
				testFeatures2 = Mat(1, 32 * 6, CV_32FC1);
			else if(testType == "ClothingType")
				testFeatures2 = Mat(1, 2 * 100, CV_32FC1);


			transpose(testFeatures, testFeatures2);

			float predictRTResponse = rt->predict(testFeatures2);
			float predictSVMResponse = svm->predict(testFeatures2);

			if (testType == "Color")
			{
				if (art_color((int)predictRTResponse) == testArticles.at(i)->getColor())
					rtHits++;

				if (art_color((int)predictSVMResponse) == testArticles.at(i)->getColor())
					svmHits++;
			}
			else if (testType == "ClothingType")
			{
				if (art_clType((int)predictRTResponse) == testArticles.at(i)->getClType())
					rtHits++;

				if (art_clType((int)predictSVMResponse) == testArticles.at(i)->getClType())
					svmHits++;

				/*
				cout << "Random Forest" << endl;
				cout << "Predicted: " << to_string(art_clType((int)predictRTResponse)) << endl;
				cout << "  Acctual: " << to_string(testArticles.at(i)->getClType()) << endl << endl;

				cout << "Support Vector Machine" << endl;
				cout << "Predicted: " << to_string(art_clType((int)predictSVMResponse)) << endl;
				cout << "  Acctual: " << to_string(testArticles.at(i)->getClType()) << endl << endl;
				*/
			}
			
		}

		cout << t << " RT Hits: " << rtHits << "\tRT Hitratio: " << (double)rtHits / (double)partSize << endl;
		cout << t << " SVM Hits: " << svmHits << "\tSVM Hitratio: " << (double)svmHits / (double)partSize << endl;

		bestRTHitRatio = max(bestRTHitRatio, (double)rtHits / (double)partSize);
		bestSVMHitRatio = max(bestSVMHitRatio, (double)svmHits / (double)partSize);

		totRTHits += rtHits;
		totSVMHits += svmHits;
	}

	for (int i = 0; i < allArticles.size(); i++)
	{
		delete(allArticles[i]);
	}
	allArticles.clear();

	double hitRatioRT  = (double)totRTHits / (double)totSize;
	double hitRatioSVM = (double)totSVMHits / (double)totSize;

	cout << "RT  Total Hitratio: " << hitRatioRT << "\tTotal hits: " << totRTHits << endl;
	cout << "SVM Total Hitratio: " << hitRatioSVM << "\tTotal hits: " << totSVMHits << endl;

	cout << "RT  Best HitRatio: " << bestRTHitRatio << endl;
	cout << "SVM Best HitRatio: " << bestSVMHitRatio << endl;

}

void Change2016(string filename, string testType)
{
	ifstream infile(filename, ios::in);
	
	vector<ClothArticle*> trainingArticles;
	vector<ClothArticle*> testArticles;

	

	string line;
	while (getline(infile, line))
	{
		ClothArticle *tmp = inputParser(line);
		if (tmp->getInputType() == 'T')
			trainingArticles.push_back(tmp);
		else
			testArticles.push_back(tmp);
	}

	cout << "skapa träningsdata" << endl;
	Ptr<TrainData> tData = createTrainingData(trainingArticles,testType);
	cout << "träningsdata skapad" << endl;


	Ptr<RTrees> rt = RTrees::create();
	Ptr<SVM> svm = SVM::create();

	if(false)
	{
		

		rt->setMaxDepth(5);
		rt->setMinSampleCount(5);
		rt->setMaxCategories(10);

		rt->setCalculateVarImportance(false);
		rt->setRegressionAccuracy(0.0f);
		rt->setPriors(Mat());

		cout << "påbörja träning" << endl;

		rt->train(tData, 0);
	}
	else
	{
		
		svm->setType(SVM::C_SVC);
		svm->setKernel(SVM::LINEAR);
		svm->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6));

		svm->train(tData, 0);
	}

	

	

	
	for (int i = 0; i < trainingArticles.size(); i++)
	{
		delete(trainingArticles[i]);
	}
	trainingArticles.clear();

	cout << "träning är klar" << endl;

	for (int i = 0; i < testArticles.size(); i++)
	{
		Mat rgbImg = testArticles.at(i)->getImage();
		Mat hsvImg;
		cvtColor(rgbImg, hsvImg, COLOR_BGR2HSV);

		Mat testFeatures;

		if(testType == "Color")
		{
			testFeatures = Mat(32 * 6, 1, CV_32FC1);
			for (int j = 0; j < 6; j++)
			{
				Mat ch, hs, nhs;
				if (j<3)
				{
					ch = getChannel(rgbImg, j);
					hs = get8bitHist(ch, 32);
					nhs = normalizeHist(hs);
				}
				else
				{
					ch = getChannel(hsvImg, j - 3);
					hs = getHsvHist(ch, j - 3, 32);
					nhs = normalizeHist(hs);
				}

				for (int k = 0; k < 32; k++)
				{
					testFeatures.at<float>(nhs.rows * j + k, 0) = (float)nhs.at<float>(k, 0);
				}
			}
		}
		else if (testType == "ClothingType")
		{
			testFeatures = Mat(/*rgbImg.size().area()*/ 2*100, 1, CV_32FC1);
			Mat imgGray;
			cvtColor(rgbImg, imgGray, COLOR_BGR2GRAY);
			Mat binary;

			threshold(imgGray, binary, 248, THRESH_BINARY_INV, THRESH_BINARY);

			binary = binary * 255;
			namedWindow("Test binary", 1);
			imshow("Test binary", binary);


			//Mat lapImg;
			//Laplacian(binary, lapImg, 256, 2, 1, 1, 4);

			Mat imgBlur = preformGaussianBlur(imgGray);
			Mat edges = preformCanny(imgBlur);

			namedWindow("Test Edges", 1);
			imshow("Test Edges", edges);

			
			vector<int> tmp = createlocalEdgeImageHist(edges, 30);
			

			namedWindow("Test Edges", 1);
			imshow("Test Edges", edges);


			for (int j = 0; j < tmp.size(); j++)
			{
				testFeatures.at<float>(j, 0) = (float)tmp.at(j);
			}

			imgBlur = preformGaussianBlur(binary);
			edges = preformCanny(imgBlur);

			tmp = createlocalEdgeImageHist(edges, 30);

			namedWindow("Test Edges2", 1);
			imshow("Test Edges2", edges);

			for (int j = 0; j < tmp.size(); j++)
			{
				testFeatures.at<float>(j+100, 0) = (float)tmp.at(j);
			}



			/*
			for (int j = 0; j < edges.rows; j++)
			{
				for (int k = 0; k < edges.cols; k++)
				{
					testFeatures.at<float>(j * edges.cols + k, 0) = (float)edges.at<unsigned char>(j, k);
				}
			}
			*/
		}
		

		cout << testFeatures << endl;
		cout << testFeatures.type() << endl;
		cout << testFeatures.cols << endl;
		
		Mat testFeatures2 = Mat(1, 32 * 6, CV_32FC1);

		transpose(testFeatures, testFeatures2);

		float predictResponse;
		if(false)
			predictResponse = rt->predict(testFeatures2);
		else
			predictResponse = svm->predict(testFeatures2);

		if (testType == "Color")
		{
			cout << "Predicted: " << to_string(art_color((int)predictResponse)) << endl;
			cout << "  Acctual: " << to_string(testArticles.at(i)->getColor()) << endl << endl;
		}
		else if (testType == "ClothingType") 
		{
			cout << "Predicted: " << to_string(art_clType((int)predictResponse)) << endl;
			cout << "  Acctual: " << to_string(testArticles.at(i)->getClType()) << endl << endl;
		}
		
		namedWindow("Test Image", 1);
		imshow("Test Image", rgbImg);

		waitKey(0);

	}

	cout << "SLUT" << endl;
}

vector<int> createlocalEdgeImageHist(Mat edges, int size)
{
	int nVerBoxs = edges.size().height / size;
	int nHorBoxs = edges.size().width / size;
	int leiHistSize = nVerBoxs * nHorBoxs;
	vector<int> leiHist(leiHistSize);

	for (int s = 0; s < nHorBoxs;s++)
	{
		for (int t = 0; t < nVerBoxs; t++)
		{
			int tot = 0;
			for (int i = 0; i < size; i++)
			{
				for (int j = 0; j < size; j++)
				{
					//cout << "x: " << s * nHorBoxs + i << "\ty: " << t * nVerBoxs + j << endl;
					tot += (int)edges.at<unsigned char>(s * size + i, t * size + j);
				}
			}
			leiHist[s * nVerBoxs + t] = (int)tot;
		}
	}
	return leiHist;
}

Ptr<TrainData> createTrainingData(vector<ClothArticle*> input, string classifierGroup)
{
	vector<int> labels; //color{ + clothType + sleeveType
	vector<Mat> trainingImages;
	
	for (int i = 0; i < input.size(); i++)
	{
		trainingImages.push_back(input.at(i)->getImage());
		
		if (classifierGroup == "Color")
			labels.push_back(input.at(i)->getColor());
		else if (classifierGroup == "ClothingType")
			labels.push_back(input.at(i)->getClType());
		else if (classifierGroup == "SleeveType")
			labels.push_back(input.at(i)->getSleeveType());
	}

	Mat labelsMat(labels.size(), 1, CV_32SC1);
	for (int i = 0; i < labels.size(); i++)
	{
		labelsMat.at<int>(i, 0) = labels[i];
	}
	
	int dataMatFeature = 0;
	if (classifierGroup == "Color")
	{
		dataMatFeature = 6 * 32;
	}
	else if (classifierGroup == "ClothingType")
	{
		dataMatFeature = 100 * 2;//trainingImages[0].size().area();
	}

	cout << trainingImages[0].size() << endl;
	cout << dataMatFeature << endl;

	Mat trainingDataMat(trainingImages.size(), dataMatFeature, CV_32FC1);
	for (int i = 0; i < labels.size(); i++)
	{
		if(classifierGroup == "Color")
		{
			Mat rgbImg = trainingImages[i];
			Mat hsvImg;
			cvtColor(rgbImg, hsvImg, COLOR_BGR2HSV);

			for (int j = 0; j < 6; j++)
			{
				Mat ch, hs, nhs;
				if (j<3)
				{
					ch = getChannel(rgbImg, j);
					hs = get8bitHist(ch, 32);
					nhs = normalizeHist(hs);
				}
				else
				{
					ch = getChannel(hsvImg, j - 3);
					hs = getHsvHist(ch, j - 3, 32);
					nhs = normalizeHist(hs);
				}

				for (int k = 0; k < 32; k++)
				{
					trainingDataMat.at<float>(i, j * 32 + k) = (float)nhs.at<float>(k, 0);
				}
			}
		}
		else if (classifierGroup == "ClothingType")
		{
			Mat imgGray;
			cvtColor(trainingImages[i], imgGray, COLOR_BGR2GRAY);
			
			Mat binary;

			threshold(imgGray, binary, 248, THRESH_BINARY_INV, THRESH_BINARY);

			binary = binary * 255;
			
			Mat imgBlur = preformGaussianBlur(imgGray);
			Mat edges = preformCanny(imgBlur);
			
			vector<int> tmp = createlocalEdgeImageHist(edges, 30);
			

			for (int j = 0; j < tmp.size(); j++)
			{
				trainingDataMat.at<float>(i,j) = (float)tmp.at(j);
			}

			imgBlur = preformGaussianBlur(binary);
			edges = preformCanny(imgBlur);

			tmp = createlocalEdgeImageHist(edges, 30);


			for (int j = 0; j < tmp.size(); j++)
			{
				trainingDataMat.at<float>(i, j+100) = (float)tmp.at(j);
			}



			/*
			for (int j = 0; j < edges.rows; j++)
			{
				for (int k = 0; k < edges.cols; k++)
				{
					trainingDataMat.at<float>(i, j * edges.cols + k) = (float)edges.at<unsigned char>(j, k);
				}
			}
			*/
		}
	}

	Ptr<TrainData> tData = TrainData::create(trainingDataMat, SampleTypes::ROW_SAMPLE, labelsMat);

	return tData;
}


void surf_test(Mat image)
{
	;

}


void test_ml_black()
{

	vector<int> labels;
	vector<Mat> trainingImages;
	for (int i = 0; i < 5; i++)
	{
		labels.push_back(1);
		Mat tmpImg = imread("./Training/black" + to_string(i) + ".jpg", IMREAD_UNCHANGED);
		trainingImages.push_back(tmpImg);
	}
	for (int i = 0; i < 3; i++)
	{
		labels.push_back(-1);
		Mat tmpImg = imread("./Training/red" + to_string(i) + ".jpg", IMREAD_UNCHANGED);
		trainingImages.push_back(tmpImg);
	}

	/*
	vector<vector<Mat>> trainingChannels;
	for (int i = 0; i < trainingImages.size(); i++)
	{
		vector<Mat> channels;

		Mat ch0 = getChannel(trainingImages[i], 0);
		Mat ch1 = getChannel(trainingImages[i], 1);
		Mat ch2 = getChannel(trainingImages[i], 2);

		

		channels.push_back(getHist(ch0, 32));
		channels.push_back(getHist(ch1, 32));
		channels.push_back(getHist(ch2, 32));

		trainingChannels.push_back(channels);
	}
	*/

	Mat labelsMat(labels.size(), 1, CV_32SC1);
	for (int i = 0; i < labels.size(); i++)
	{
		labelsMat.at<int>(i,0) = labels[i];
	}
	Mat trainingDataMat(trainingImages.size(), 6 * 32, CV_32FC1);
	for (int i = 0; i < labels.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Mat ch = getChannel(trainingImages[i], j);
			Mat hs = get8bitHist(ch, 32);

			for (int k = 0; k < 32; k++)
			{
				trainingDataMat.at<float>(i, j * 32 + k) = (float)hs.at<float>(k, 0);
			}
		}
	}

	Ptr<TrainData> tData = TrainData::create(trainingDataMat, SampleTypes::ROW_SAMPLE, labelsMat);

	Ptr<RTrees> rt = RTrees::create();

	rt->setMaxDepth(5);
	rt->setMinSampleCount(5);
	rt->setMaxCategories(10);

	rt->setCalculateVarImportance(false);
	rt->setRegressionAccuracy(0.0f);
	rt->setPriors(Mat());

	rt->train(tData, 0);

	Mat test = imread("./Training/blue0.jpg", IMREAD_UNCHANGED);

	Mat ch0 = getChannel(test, 0);
	Mat ch1 = getChannel(test, 1);
	Mat ch2 = getChannel(test, 2);


	Mat hn[3];
	hn[0] = get8bitHist(ch0, 32);
	hn[1] = get8bitHist(ch1, 32);
	hn[2] = get8bitHist(ch2, 32);

	cout << hn[0] << endl << endl;
	cout << hn[1] << endl << endl;
	cout << hn[2] << endl << endl;

	Mat h123(hn[0].rows * 3, 1, CV_32FC1);

	
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < hn[0].rows; j++)
		{
			h123.at<float>(hn[0].rows * i + j, 0) = hn[i].at<float>(j, 0);
		}
	}

	float predictResponse;
	predictResponse = rt->predict(h123);


	cout << "Predict 1: " << predictResponse << endl;

	cout << "Tjodilo" << endl;
}

void rtrees()
{
	// Data for visual representation
	int width = 512, height = 512;
	Mat image = Mat::zeros(height, width, CV_8UC3);

	// Set up training data
	int labels[4] = { 1, -1, 1, 1 };
	Mat labelsMat(4, 1, CV_32SC1, labels);

	float trainingData[4][9] = { { 0,0,0,0,0,0,0,0,0 },{ 100,100,100,100,100,100,100,100,100 },{ 0,0,0,0,0,0,0,0,0 },{ 0,0,0,0,0,0,0,0,0 } };
	//float trainingData[4][9] = { { 100,100,100,100,0,100,100,100,100 },{ 100,100,100,100,100,100,100,100,100 },{ 100,100,100, 0,0,0,100,100,100 },{ 0,100,0, 100,0,100 , 0,100,0 } };
	Mat trainingDataMat(4, 9, CV_32FC1, trainingData);

	int labels2[7] = { 0,1,-1,1,1,-1,0 };
	Mat labelsMat2(7, 1, CV_32SC1, labels2);
	float trainingData2[7][2] = { {100,100},{300,80},{80,60},{30,20},{10,30}, {150,200},{130,250}};
	Mat trainingDataMat2(7, 2, CV_32FC1, trainingData2);
	Ptr<TrainData> tData2 = TrainData::create(trainingDataMat2, SampleTypes::ROW_SAMPLE, labelsMat2);

	

	Mat responses;

	Ptr<TrainData> tData = TrainData::create(trainingDataMat, SampleTypes::ROW_SAMPLE, labelsMat);
	Ptr<TrainData> trData = TrainData::create(trainingDataMat, SampleTypes::ROW_SAMPLE, responses, noArray(), noArray(), noArray());



	Ptr<RTrees> rt = RTrees::create();

	rt->setMaxDepth(5);
	rt->setMinSampleCount(5);
	rt->setMaxCategories(10);

	rt->setCalculateVarImportance(false);
	rt->setRegressionAccuracy(0.0f);
	rt->setPriors(Mat());

	rt->train(tData2, 0);

	Mat sampleMat2 = (Mat_<float>(1, 2) << 100, 100);
	Mat sampleMat = (Mat_<float>(1, 9) << 100, 100, 100, 100, 100, 100, 100, 100, 100);
	float predictResponse;
	predictResponse = rt->predict(sampleMat2);


	cout << "Predict 1: " << predictResponse << endl;

	sampleMat2 = (Mat_<float>(1, 2) << 300, 80);
	predictResponse = rt->predict(sampleMat2);
	cout << "Predict 2: " << predictResponse << endl;

	cout << "HEJ" << endl;

	Vec3b green(0, 255, 0), blue(255, 0, 0), red(0, 0, 255);
	// Show the decision regions given by the SVM
	for (int i = 0; i < image.rows; ++i)
		for (int j = 0; j < image.cols; ++j)
		{
			Mat sampleMat = (Mat_<float>(1, 2) << i, j);
			Mat predictResponse;
			float predicted = rt->predict(sampleMat);


			if (predicted == 1)
				image.at<Vec3b>(i, j) = green;
			else if (predicted == -1)
				image.at<Vec3b>(i, j) = blue;
			else
				image.at<Vec3b>(i, j) = red;

		}

	imshow("SVM Simple Example", image); // show it to the user

	waitKey(0);


}

void svm()
{
	

	
	// Data for visual representation
    int width = 512, height = 512;
    Mat image = Mat::zeros(height, width, CV_8UC3);

    // Set up training data
    int labels[4] = {1, -1, 1, 1};
    Mat labelsMat(4, 1, CV_32SC1, labels);

	float trainingData[4][9] = { {100,100,100,100,0,100,100,100,100},{ 100,100,100,100,100,100,100,100,100 },{ 100,100,100, 0,0,0,100,100,100 }, { 0,100,0, 100,0,100 , 0,100,0 }};
    Mat trainingDataMat(4, 9, CV_32FC1, trainingData);

	int labels2[4] = { -1,1,0,2 };
	Mat labelsMat2(4, 1, CV_32SC1, labels2);
	float trainingData2[4][2] = { { 20,40 },{ 60,80 },{ 80,60 },{ 30,20 } };
	Mat trainingDataMat2(4, 2, CV_32FC1, trainingData2);
	Ptr<TrainData> tData2 = TrainData::create(trainingDataMat2, SampleTypes::ROW_SAMPLE, labelsMat2);


	Mat responses;

	Ptr<TrainData> tData = TrainData::create(trainingDataMat, SampleTypes::ROW_SAMPLE, labelsMat);
	//Ptr<TrainData> trData = TrainData::create(trainingDataMat, SampleTypes::ROW_SAMPLE, responses,noArray(),noArray(),noArray());
	
	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);
	svm->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6));
	
	svm->train(tData2, 0);

	Mat sampleMat2 = (Mat_<float>(1, 2) << 100, 100);

	Mat sampleMat = (Mat_<float>(1, 9) << 100, 100, 100, 100, 50, 100, 100, 100, 100);
	Mat predictResponse;
	svm->predict(sampleMat2, predictResponse, 0);

	cout << sampleMat << endl;

	cout << "Predict 1: " << predictResponse.at<float>(0, 0) << endl;
	
	sampleMat2 = (Mat_<float>(1, 2) << 10, 10);
	sampleMat = (Mat_<float>(1, 9) << 0, 0, 0, 0, 100, 0, 0, 0, 0);
	svm->predict(sampleMat2, predictResponse, 0);
	cout << "Predict 2: " << predictResponse.at<float>(0, 0) << endl;



	
    Vec3b green(0,255,0), blue (255,0,0);
    // Show the decision regions given by the SVM
    for (int i = 0; i < image.rows; ++i)
        for (int j = 0; j < image.cols; ++j)
        {
            Mat sampleMat = (Mat_<float>(1,2) << i,j );
			Mat predictResponse;
			svm->predict(sampleMat, predictResponse, 0);
			
			
            if (predictResponse.at<float>(0, 0) == 1)
                image.at<Vec3b>(i,j)  = green;
            else if (predictResponse.at<float>(0, 0) == -1)
                 image.at<Vec3b>(i,j)  = blue;
				 
        }

	
	// Show the training data
    int thickness = -1;
    int lineType = 8;
    //circle( image, Point(501,  10), 5, Scalar(  0,   0,   0), thickness, lineType);
    //circle( image, Point(255,  10), 5, Scalar(255, 255, 255), thickness, lineType);
    //circle( image, Point(501, 255), 5, Scalar(255, 255, 255), thickness, lineType);
    //circle( image, Point( 10, 501), 5, Scalar(255, 255, 255), thickness, lineType);

    // Show support vectors
    thickness = 2;
    lineType  = 8;
	Mat support = svm->getUncompressedSupportVectors();
	int c = support.rows;

	

    for (int i = 0; i < c; ++i)
    {
		float v[2];
        v[0] = support.at<float>(i,0);
		v[1] = support.at<float>(i, 1);
        circle( image,  Point( (int) v[0], (int) v[1]),   6,  Scalar(128, 128, 128), thickness, lineType);
    }

    //imwrite("result.png", image);        // save the image

    imshow("SVM Simple Example", image); // show it to the user
	
	
    waitKey(0);
	
}

void filterAlphaArtifacts(Mat *img)
{
	Mat *ch = (Mat*)calloc(4, sizeof(Mat));

	split(*img, ch);
	Mat ch1 = ch[3];

	for (int i = 0; i < ch1.rows; i++)
	{
		for (int j = 0; j < ch1.cols; j++)
		{
			if (ch1.at<unsigned char>(i, j) == 0)
			{
				img->at<Vec4b>(i, j) = Vec4b(255, 255, 255, 0);
			}
		}
	}
	free(ch);
}

void printHough(Mat input) //Funkar bara i Release mode
{
	Mat dst, color_dst;

	Canny(input, dst, 50, 200, 3);
	cvtColor(dst, color_dst, COLOR_GRAY2RGB);

	vector<Vec4i> lines;
	HoughLinesP(dst, lines, 1, CV_PI / 180, 20, 80, 10);

	for (size_t i = 0; i < lines.size(); i++)
	{
		line(color_dst, Point(lines[i][0], lines[i][1]),
			Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
	}

	namedWindow("Source", 1);
	imshow("Source", input);

	namedWindow("Detected Lines", 1);
	imshow("Detected Lines", color_dst);

	waitKey(0);

}

void printGPUCanny(Mat src)
{
	InputArray g = src;
	

	Mat& mGr = src;

	Mat mask;
	
	
	Mat src1(640, 480, CV_8UC4, Scalar::all(0));
	//GpuMat *d_src1 = GpuMat::create;

	//GpuMat b(mask);

	//GpuMat::Allocator all;
	//GpuMat *prGpu(g,);

}

Mat preformCanny(Mat src)
{
	Mat out;

	double threshold1 = 60.0;
	double threshold2 = 120.0;

	int apertureSize = 3;
	bool L2gradient = false; 

	Canny(src, out, threshold1, threshold2, apertureSize, L2gradient);

	return out;
}

Mat preformGaussianBlur(Mat src)
{
	Mat out;

	Size ksize = Size(3, 3);
	double sigmaX = 4.00;
	double sigmaY = 4.00;

	GaussianBlur(src, out, ksize, sigmaX, sigmaY);
	
	return out;
}

Mat normalizeHist(Mat hist)
{
	float totEnt = 0;
	for (int k = 0; k < 32; k++)
	{
		totEnt += hist.at<float>(k, 0);
	}
	for (int k = 0; k < 32; k++)
	{
		hist.at<float>(k, 0) /= totEnt;
	}

	return hist;
}

Mat get8bitHist(Mat img1D, int numLevels, int minRange, int maxRange)
{
	MatND hist, hist2;
	int histSize[] = { numLevels };
	float hranges[] = { 0, 256 };
	float hranges2[] = { 0, 255 }; //Takes away the background which always are completly white in test images. May also take away other bright pixels that aren't entierly white, but this shouldn't be many pixels.
	const float* ranges[] = { hranges };
	const float* ranges2[] = { hranges2 };
	int channels[] = { 0 };

	calcHist(&img1D, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);
	calcHist(&img1D, 1, channels, Mat(), hist2, 1, histSize, ranges2, true, false);

	//cout << hist << endl;
	//cout << hist2 << endl;

	return hist2;
}

Mat getHsvHist(Mat img1D, int type, int numLevels, int minRange, int maxRange)
{
	MatND hist;
	int histSize[] = { numLevels };
	int channels[] = { 0 };
	float hranges[] = { 0, 180 };
	float sranges[] = { 0, 256 };
	float vranges[] = { 0, 360 };
	const float* ranges[] = { hranges };
	if (type == 0)
	{
		const float* ranges[] = { hranges };
		calcHist(&img1D, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);
	}

	else if (type == 1)
	{
		const float* ranges[] = { sranges };
		calcHist(&img1D, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);
	}

	else
	{
		const float* ranges[] = { vranges };
		calcHist(&img1D, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);
	}
	return hist;
}

Mat getChannel(Mat src, int channel)  // HÄR <<-<-<-<-<-<-<-<-<-<-<-
{
	Mat *ch = (Mat*)calloc(4, sizeof(Mat));

	Mat tmpCh;
	split(src, ch);
	ch[channel].copyTo(tmpCh);
	
	for (int i = 0; i < 4; i++)
	{

		ch[i].release();
	}
	
	free(ch);
	
	return tmpCh;
}