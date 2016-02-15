#include <iostream>
#include <fstream>

//#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "ClothArticle.h"
#include "ImageUtilities.h"
#include "ImageSearcher.h"


using namespace cv;
using namespace cv::ml;
using namespace std;


void svm_test();
void rtrees_test();
void svmANDrfTest(string filename, string testType);
float calcEuclDist(Mat fVec1, Mat fVec2);
void testModelWithImage(string trainingFilename, string testFilename, string testType, bool loadModel = false);


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
	
	//svm_test();

	//rtrees_test();

	//svmANDrfTest("SallReady.txt", "Color");

	//testModelWithImage("readyFile.xx", "hmtest3.jpg", "None", false);

	vector<string> closest = seekUsingImage("readyFile.xx", "hmtest3.jpg", 11);

	for (int i = 0; i < closest.size();i++)
	{
		cout << closest[i] << endl;
	}

	return 0;
}

void testModelWithImage(string trainingFilename, string testFilename, string testType, bool loadModel)
{
	vector<ClothArticle*> allArticles = readCatalogeFromFile(trainingFilename);
	
	ClothArticle* testItem = new ClothArticle("Test0", testFilename, "Gra", "Top", -1);

	if(testType != "None")
	{
		Mat testFeatVec = createFeatureVector(testItem, testType);

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
	else
	{
		vector<string> nn = findClosestNeighbours(allArticles, testItem, 11, testType);

		namedWindow("Query", 1);
		imshow("Query", testItem->getImage());

		for (int i = 0; i < nn.size(); i++)
		{
			for (int j = 0; j < allArticles.size(); j++)
			{
				if (nn[i] == allArticles[j]->getId())
				{
					namedWindow("Result # " + to_string(i + 1), 1);
					imshow("Result # " + to_string(i + 1), allArticles[j]->getImage());
				}
			}
		}


		waitKey(0);

		if (true) //Shows some diffrent features from input image.
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
	

}

void svmANDrfTest(string filename, string testType)
{
	vector<ClothArticle*> allArticles = readCatalogeFromFile(filename);

	int totSize = (int)allArticles.size();
	int partSize = totSize / 10;


	double bestRTHitRatio = 0.0;
	double bestSVMHitRatio = 0.0;

	int totRTHits = 0;
	int totSVMHits = 0;
	for (int t = 0; t < 10; t++)
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

		Ptr<SVM> modelSVM;
		Ptr<RTrees> modelRF;
		modelSVM = makeSVMModel(trainingArticles, testType);
		modelRF = makeRTModel(trainingArticles, testType);

		int svmHits = 0;
		int rtHits = 0;

		for (int i = 0; i < testArticles.size(); i++)
		{
			Mat testFeatVec = createFeatureVector(testArticles[i], testType);

			float predictSVMResponse = modelSVM->predict(testFeatVec);
			float predictRTResponse = modelRF->predict(testFeatVec);

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

	double hitRatioRT = (double)totRTHits / (double)totSize;
	double hitRatioSVM = (double)totSVMHits / (double)totSize;

	cout << "RT  Total Hitratio: " << hitRatioRT << "\tTotal hits: " << totRTHits << endl;
	cout << "SVM Total Hitratio: " << hitRatioSVM << "\tTotal hits: " << totSVMHits << endl;

	cout << "RT  Best HitRatio: " << bestRTHitRatio << endl;
	cout << "SVM Best HitRatio: " << bestSVMHitRatio << endl;

}

void surf_test(Mat image)
{
	;

}

void rtrees_test()
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

void svm_test()
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

