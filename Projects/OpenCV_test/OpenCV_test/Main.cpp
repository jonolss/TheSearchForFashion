#include <opencv2/core/core.hpp>
#include "opencv2/core/utility.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <tuple> 

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
vector<float> createlocalEdgeImageHist(Mat edges, int size);
float calcEuclDist(Mat fVec1, Mat fVec2);
void testModelWithImage(string trainingFilename, string testFilename, string testType, bool loadModel = false);
Mat createFeatureVector(ClothArticle* input, string testType);
Ptr<SVM> makeSVMModel(vector<ClothArticle*> input, string testType);
Ptr<RTrees> makeRTModel(vector<ClothArticle*> input, string testType);
vector<string> findClosestNeighbours(vector<ClothArticle*> allArticles, ClothArticle* query, int n, string testType);


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
	//rtrees();

	//test_ml();

	//test_ml_black();

	//inputParser("PATH");

	//Change2016("rBoS.txt", "Color");

	//svm();

	//tester_SVM_vs_RF("SallReady.txt", "ClothingType");

	testModelWithImage("SallReady.txt", "hmtest2.jpg", "None", false);

	return 0;
}



void testModelWithImage(string trainingFilename, string testFilename, string testType, bool loadModel)
{
	ifstream trainingFile(trainingFilename, ios::in);

	vector<ClothArticle*> allArticles;

	string line;
	while (getline(trainingFile, line))
	{
		allArticles.push_back(inputParser(line));
	}
	trainingFile.close();

	ClothArticle* testItem = new ClothArticle('T', "Test0", testFilename, "Gra", "Top", -1);

	Mat testFeatVec = createFeatureVector(testItem, testType);

	if(testType != "None")
	{
		Ptr<SVM> model;
		if (!loadModel)
		{
			model = makeSVMModel(allArticles, testType);
			if (testType == "Color")
				model->save("ColorModel.xml");
			else if (testType == "ClothingType")
				model->save("ClTypeModel.xml");
		}
		else
		{
			if (testType == "Color")
				model = Algorithm::load<SVM>("ColorModel.xml");
			else if (testType == "ClothingType")
				model = Algorithm::load<SVM>("ClTypeModel.xml");
		}

		float predictResponse = model->predict(testFeatVec);

		cout << "Support Vector Machine" << endl;
		if (testType == "Color")
		{
			cout << "Predicted: " << to_string(art_color((int)predictResponse)) << endl;
			testItem->setColor(art_color((int)predictResponse));
		}
		else if (testType == "ClothingType")
		{
			cout << "Predicted: " << to_string(art_clType((int)predictResponse)) << endl;
			testItem->setClType(art_clType((int)predictResponse));
		}
	}

	

	vector<string> nn = findClosestNeighbours(allArticles, testItem, 11, testType);

	namedWindow("Query", 1);
	imshow("Query", testItem->getImage());

	for (int i = 0; i < nn.size(); i++)
	{
		for (int j = 0; j < allArticles.size(); j++)
		{
			if(nn[i] == allArticles[j]->getId())
			{
				namedWindow("Result # " + to_string(i+1), 1);
				imshow("Result # " + to_string(i+1), allArticles[j]->getImage());
			}
		}
	}


	waitKey(0);

	if(true) //Shows some diffrent features from input image.
	{
		Mat imgGray;
		cvtColor(testItem->getImage(), imgGray, COLOR_BGR2GRAY);
		Mat binary;

		threshold(imgGray, binary, 248, THRESH_BINARY_INV, THRESH_BINARY);

		binary = binary * 255;

		Mat noBluredges = preformCanny(binary);

		Mat imgBlur = preformGaussianBlur(imgGray);
		Mat edges = preformCanny(imgBlur);


		namedWindow("Query T", 1);
		imshow("Query T", testItem->getImage());

		namedWindow("Query Gray", 1);
		imshow("Query Gray", imgGray);

		namedWindow("Query Binary", 1);
		imshow("Query Binary", binary);

		namedWindow("Query noBluredges", 1);
		imshow("Query noBluredges", noBluredges);

		namedWindow("Query imgBlur", 1);
		imshow("Query imgBlur", imgBlur);

		namedWindow("Query edges", 1);
		imshow("Query edges", edges);


		waitKey(0);
	}
	
}

struct GreatTuple
{
	bool operator()(const tuple<float, string>& lhs, const tuple<float, string>& rhs) const
	{
		return get<0>(lhs) > get<0>(rhs);
	}
};

vector<string> findClosestNeighbours(vector<ClothArticle*> allArticles, ClothArticle* query, int n, string testType)
{
	priority_queue< tuple<float, string>, vector<tuple<float, string>>, GreatTuple > priQueue;

	Mat queryFeat = createFeatureVector(query, "Color+ClothingType");

	for (int i = 0; i < allArticles.size(); i++)
	{
		ClothArticle* curr = allArticles[i];
		if(testType == "ClothingType" && curr->getClType() == query->getClType() || testType == "Color" && curr->getColor() == query->getColor())
		{
			Mat currFeat = createFeatureVector(curr, "Color+ClothingType");
			float dist = calcEuclDist(queryFeat, currFeat);

			tuple<float, string> currPriEntry;
			currPriEntry = make_tuple(dist, curr->getId());

			priQueue.push(currPriEntry);
		}
		else if (testType == "None")
		{
			Mat currFeat = createFeatureVector(curr, "Color+ClothingType");
			float dist = calcEuclDist(queryFeat, currFeat);

			tuple<float, string> currPriEntry;
			currPriEntry = make_tuple(dist, curr->getId());

			priQueue.push(currPriEntry);
		}
	}

	vector<string> topResults;

	for (int i = 0; i < n; i++)
	{
		topResults.push_back(get<1>(priQueue.top()));
		cout << get<0>(priQueue.top()) << endl;
		priQueue.pop();
	}

	return topResults;
}



Mat createFeatureVector(ClothArticle* input, string testType)
{
	Mat fVec;

	if (testType == "Color")
	{
		Mat hsvImg;
		cvtColor(input->getImage(), hsvImg, COLOR_BGR2HSV);

		fVec = Mat(1, 32 * 6, CV_32F);
		for (int j = 0; j < 6; j++)
		{
			Mat ch, hs, nhs;
			if (j<3)
			{
				ch = getChannel(input->getImage(), j);
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
				fVec.at<float>(0, nhs.rows * j + k) = (float)nhs.at<float>(k, 0);
			}
		}
	}
	else if (testType == "ClothingType")
	{
		fVec = Mat(1, 2 * 100, CV_32FC1);
		Mat imgGray;
		cvtColor(input->getImage(), imgGray, COLOR_BGR2GRAY);
		Mat binary;

		threshold(imgGray, binary, 248, THRESH_BINARY_INV, THRESH_BINARY);

		binary = binary * 255;

		Mat imgBlur = preformGaussianBlur(imgGray);
		Mat edges = preformCanny(imgBlur);

		vector<float> tmp = createlocalEdgeImageHist(edges, 30);

		for (int j = 0; j < tmp.size(); j++)
		{
			fVec.at<float>(0, j) = (float)tmp.at(j);
		}

		imgBlur = preformGaussianBlur(binary);
		edges = preformCanny(imgBlur);

		tmp = createlocalEdgeImageHist(/*binary*/edges, 30);

		for (int j = 0; j < tmp.size(); j++)
		{
			fVec.at<float>(0, j + 100) = (float)tmp.at(j);
		}

	}
	else if (testType == "Color+ClothingType" || testType == "ClothingType+Color")
	{
		fVec = Mat(1, 2 * 100 + 32 * 6, CV_32FC1);
		
		Mat imgGray;
		cvtColor(input->getImage(), imgGray, COLOR_BGR2GRAY);
		Mat binary;

		threshold(imgGray, binary, 248, THRESH_BINARY_INV, THRESH_BINARY);

		binary = binary * 255;

		Mat imgBlur = preformGaussianBlur(imgGray);
		Mat edges = preformCanny(imgBlur);

		vector<float> tmp = createlocalEdgeImageHist(edges, 30);

		for (int j = 0; j < tmp.size(); j++)
		{
			fVec.at<float>(0, j) = (float)tmp.at(j);
		}

		imgBlur = preformGaussianBlur(binary);
		edges = preformCanny(imgBlur);

		tmp = createlocalEdgeImageHist(/*binary*/edges, 30);

		for (int j = 0; j < tmp.size(); j++)
		{
			fVec.at<float>(0, j + 100) = (float)tmp.at(j);
		}

		Mat hsvImg;
		cvtColor(input->getImage(), hsvImg, COLOR_BGR2HSV);

		for (int j = 0; j < 6; j++)
		{
			Mat ch, hs, nhs;
			if (j<3)
			{
				ch = getChannel(input->getImage(), j);
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
				fVec.at<float>(0, nhs.rows * j + k + 200) = (float)nhs.at<float>(k, 0);
			}
		}

	}
	return fVec;
}

Ptr<SVM> makeSVMModel(vector<ClothArticle*> input, string testType)
{
	Ptr<TrainData> tData = createTrainingData(input, testType);

	Ptr<SVM> svm = SVM::create();
	
	svm->setType(SVM::C_SVC);
	//svm->setKernel(SVM::LINEAR);
	//svm->setKernel(SVM::POLY); svm->setDegree(2.0);
	svm->setKernel(SVM::CHI2);
	svm->setGamma(1);
	svm->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6));

	svm->train(tData, 0);

	return svm;
}

Ptr<RTrees> makeRTModel(vector<ClothArticle*> input, string testType)
{
	Ptr<TrainData> tData = createTrainingData(input, testType);

	Ptr<RTrees> rt = RTrees::create();

	rt->setMaxDepth(20);
	rt->setMinSampleCount(20);
	rt->setMaxCategories(20);

	rt->setCalculateVarImportance(false);
	rt->setRegressionAccuracy(0.0f);
	rt->setPriors(Mat());

	rt->train(tData, 0);

	return rt;
}

void tester_SVM_vs_RF(string filename, string testType)
{
	ifstream infile(filename, ios::in);

	vector<ClothArticle*> allArticles;

	string line;
	while (getline(infile, line))
	{
		allArticles.push_back(inputParser(line));
	}

	infile.close();
	
	int totSize = (int)allArticles.size();
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
		
		svm->train(tDataSVM, 0);

		cout << "Träning är klar" << endl;

		Ptr<TrainData> vDataRT = createTrainingData(testArticles, testType);
		Ptr<TrainData> vDataSVM = createTrainingData(testArticles, testType);


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
				testFeatures = Mat(2 * 100, 1, CV_32FC1);
				Mat imgGray;
				cvtColor(rgbImg, imgGray, COLOR_BGR2GRAY);
				Mat binary;

				threshold(imgGray, binary, 248, THRESH_BINARY_INV, THRESH_BINARY);

				binary = binary * 255;

				Mat imgBlur = preformGaussianBlur(imgGray);
				Mat edges = preformCanny(imgBlur);

				vector<float> tmp = createlocalEdgeImageHist(edges, 30);

				for (int j = 0; j < tmp.size(); j++)
				{
					testFeatures.at<float>(j, 0) = (float)tmp.at(j);
				}

				imgBlur = preformGaussianBlur(binary);
				edges = preformCanny(imgBlur);

				tmp = createlocalEdgeImageHist(/*binary*/edges, 30);

				for (int j = 0; j < tmp.size(); j++)
				{
					testFeatures.at<float>(j + 100, 0) = (float)tmp.at(j);
				}

			}


			Mat testFeatures2;
			if(testType == "Color")
				testFeatures2 = Mat(1, 32 * 6, CV_32FC1);
			else if(testType == "ClothingType")
				testFeatures2 = Mat(1, 2 * 100, CV_32FC1);

			//cout << testFeatures2 << endl;


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
				

				namedWindow("Test Image", 1);
				imshow("Test Image", rgbImg);

				waitKey(0);
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
			testFeatures = Mat( 2*100, 1, CV_32FC1);
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

			
			vector<float> tmp = createlocalEdgeImageHist(edges, 30);
			

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
		}
		
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

float calcEuclDist(Mat fVec1, Mat fVec2)
{
	if (fVec1.size() != fVec2.size())
		return -1.0f;

	float dist = 0;
	for (int i = 0; i < fVec1.rows; i++)
	{
		for (int j = 0; j < fVec1.cols; j++)
		{
			dist += powf((fVec1.at<float>(i, j) - fVec2.at<float>(i, j)),2);
		}
	}
	return sqrt(dist);
}

vector<float> createlocalEdgeImageHist(Mat edges, int size)
{
	int nVerBoxs = edges.size().height / size;
	int nHorBoxs = edges.size().width / size;
	int leiHistSize = nVerBoxs * nHorBoxs;
	vector<float> leiHist(leiHistSize);

	for (int s = 0; s < nHorBoxs;s++)
	{
		for (int t = 0; t < nVerBoxs; t++)
		{
			int tot = 0;
			for (int i = 0; i < size; i++)
			{
				for (int j = 0; j < size; j++)
				{
					if((int)edges.at<unsigned char>(s * size + i, t * size + j) != 0)
						tot ++;
				}
			}
			leiHist[s * nVerBoxs + t] = (float)tot / (float)(size*size);
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
		dataMatFeature = 100 * 2;
	}

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
			
			vector<float> tmp = createlocalEdgeImageHist(edges, 30);
			

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

		}
	}

	Ptr<TrainData> tData = TrainData::create(trainingDataMat, SampleTypes::ROW_SAMPLE, labelsMat);

	return tData;
}


void surf_test(Mat image)
{
	;

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
	//svm->setKernel(SVM::LINEAR);
	svm->setGamma(1); svm->setKernel(SVM::CHI2);
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

	return hist2;
}


/**Makes a histogram of chosen channel of a HSV image.
*
* Input1: The image whose channel is going to be extracted.
* Input2: Number of the channel that is going to be extracted.
* InputX: ...
* Output: The requested channel of the input image.
*/
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

/**Recovers the requsted channel from an image.
 *
 * Input1: The image whose channel is going to be extracted.
 * Input2: Number of the channel that is going to be extracted.
 * Output: The requested channel of the input image.
 */
Mat getChannel(Mat src, int channel)
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