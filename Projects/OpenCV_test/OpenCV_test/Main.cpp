
#include <iostream>
#include <fstream>

//#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>

#include "ClothArticle.h"
#include "ImageUtilities.h"
#include "ImageSearcher.h"

#include "FrontEnd.h"

#include "ezsift.h"
#include "image.h"
#include "img_io.h"



//using namespace cv;
//using namespace cv::ml;
using namespace std;

void sift_test();
void fast_test();
void svm_test();
void rtrees_test();
void svmANDrfTest(string filename, string testType);
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

	//svmANDrfTest("readyFile.xx", "ClothingType");

	testModelWithImage("readyFile.xx", "hmtest2.jpg", "All", false);  //<-- Bör funka på dirren

	//fast_test();

	//sift_test();
	
	/*
	if (argc == 2)
	{
		frontLoop();
	}
	else
	{
		t();
	}
	*/

	/*
	vector<string> closest = seekUsingImage("readyFile.xx", "hmtest3.jpg", 11);

	
	for (int i = 0; i < closest.size();i++)
	{
		cout << closest[i] << endl;
	}
	*/
	return 0;
}

/**Tests a simple search in a cataloge, with the use of SVM or RF to filter out classes.
* 
* \params trainingFilename Path to the trainging cataloge.
* \params testFilename Path image query.
* \params testType The type of classifier that is going to be used as filter.
* \params loadModel Boolean indicating if the model should be loaded from file or not.
*/
void testModelWithImage(string trainingFilename, string testFilename, string testType, bool loadModel)
{
	vector<ClothArticle*> allArticles = readCatalogeFromFile(trainingFilename);
	
	ClothArticle* testItem = new ClothArticle("Test0", testFilename, "Gra", "Top", -1);

	if(testType != "None")
	{
		cv::Ptr<cv::ml::SVM> model;
		if (testType == "Color" || testType == "All")
		{
			cv::Mat testFeatVec = createFeatureVector(testItem, "Color");

			if (!loadModel)
			{
				model = makeSVMModel(allArticles, "Color");
				model->save("ColorModel.xml");
			}
			else
			{
				model = cv::Algorithm::load<cv::ml::SVM>("ColorModel.xml");
			}

			float predictResponse = model->predict(testFeatVec);

			cout << "Predicted: " << to_string(art_color((int)predictResponse)) << endl;
			testItem->setColor(art_color((int)predictResponse));
		}
		if (testType == "ClothingType" || testType == "All")
		{
			cv::Mat testFeatVec = createFeatureVector(testItem, "ClothingType");

			if (!loadModel)
			{
				model = makeSVMModel(allArticles, "ClothingType");
				model->save("ClTypeModel.xml");
			}
			else
			{
				model = cv::Algorithm::load<cv::ml::SVM>("ClTypeModel.xml");
			}

			float predictResponse = model->predict(testFeatVec);

			cout << "Predicted: " << to_string(art_clType((int)predictResponse)) << endl;
			testItem->setClType(art_clType((int)predictResponse));
		}
		
	}

	vector<string> nn = findClosestNeighbours(allArticles, testItem, 11, testType);

	cv::namedWindow("Query", 1);
	cv::imshow("Query", testItem->getImage());

	for (int i = 0; i < nn.size(); i++)
	{
		for (int j = 0; j < allArticles.size(); j++)
		{
			if (nn[i] == allArticles[j]->getId())
			{
				cv::namedWindow("Result # " + to_string(i + 1), 1);
				cv::imshow("Result # " + to_string(i + 1), allArticles[j]->getImage());
			}
		}
	}


	cv::waitKey(0);

	if (false) //Shows some diffrent features from input image.
	{
		cv::Mat imgGray;
		cv::cvtColor(testItem->getImage(), imgGray, cv::COLOR_BGR2GRAY);
		cv::Mat binary;

		cv::threshold(imgGray, binary, 248, cv::THRESH_BINARY_INV, cv::THRESH_BINARY);

		binary = binary * 255;

		cv::Mat noBluredges = preformCanny(binary);

		cv::Mat imgBlur = preformGaussianBlur(imgGray);
		cv::Mat edges = preformCanny(imgBlur);

		cv::Mat coolEdge;
		cv::Mat image = testItem->getImage();
		bool png = testItem->getPath().find(".png") != string::npos;
		if(png)
		{
			cv::Mat *ch = (cv::Mat*)calloc(4, sizeof(cv::Mat));

			cv::split(image, ch);
			cv::Mat alpha = ch[3];
			cv::threshold(alpha, coolEdge, 255, cv::THRESH_BINARY_INV, cv::THRESH_BINARY);

			coolEdge = coolEdge * 255;

			cv::namedWindow("Edge0", 1);
			cv::imshow("Edge0", alpha);

			cv::namedWindow("Edge00", 1);
			cv::imshow("Edge00", coolEdge);

			for (int i = 0; i < 4; i++)
			{

				ch[i].release();
			}
			free(ch);
		}

		cv::namedWindow("Edge1", 1);
		cv::imshow("Edge1", coolEdge);

		cv::namedWindow("Edge2", 1);
		cv::imshow("Edge2", edges);

		cv::namedWindow("Edge3", 1);
		cv::imshow("Edge3", noBluredges);
			


		cv::waitKey(0);


		cv::namedWindow("Query T", 1);
		cv::imshow("Query T", testItem->getImage());

		cv::namedWindow("Query Gray", 1);
		cv::imshow("Query Gray", imgGray);

		cv::namedWindow("Query Binary", 1);
		cv::imshow("Query Binary", binary);

		cv::namedWindow("Query noBluredges", 1);
		cv::imshow("Query noBluredges", noBluredges);

		cv::namedWindow("Query imgBlur", 1);
		cv::imshow("Query imgBlur", imgBlur);

		cv::namedWindow("Query edges", 1);
		cv::imshow("Query edges", edges);


		cv::waitKey(0);
	
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

		cv::Ptr<cv::ml::SVM> modelSVM;
		cv::Ptr<cv::ml::RTrees> modelRF;
		modelSVM = makeSVMModel(trainingArticles, testType);
		modelRF = makeRTModel(trainingArticles, testType);

		int svmHits = 0;
		int rtHits = 0;

		for (int i = 0; i < testArticles.size(); i++)
		{
			cv::Mat testFeatVec = createFeatureVector(testArticles[i], testType);

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

	std::cout << "RT  Total Hitratio: " << hitRatioRT << "\tTotal hits: " << totRTHits << endl;
	std::cout << "SVM Total Hitratio: " << hitRatioSVM << "\tTotal hits: " << totSVMHits << endl;

	std::cout << "RT  Best HitRatio: " << bestRTHitRatio << endl;
	std::cout << "SVM Best HitRatio: " << bestSVMHitRatio << endl;

}

void surf_test(cv::Mat image)
{
	;

}

void rtrees_test()
{
	// Data for visual representation
	int width = 512, height = 512;
	cv::Mat image = cv::Mat::zeros(height, width, CV_8UC3);

	// Set up training data
	int labels[4] = { 1, -1, 1, 1 };
	cv::Mat labelsMat(4, 1, CV_32SC1, labels);

	float trainingData[4][9] = { { 0,0,0,0,0,0,0,0,0 },{ 100,100,100,100,100,100,100,100,100 },{ 0,0,0,0,0,0,0,0,0 },{ 0,0,0,0,0,0,0,0,0 } };
	//float trainingData[4][9] = { { 100,100,100,100,0,100,100,100,100 },{ 100,100,100,100,100,100,100,100,100 },{ 100,100,100, 0,0,0,100,100,100 },{ 0,100,0, 100,0,100 , 0,100,0 } };
	cv::Mat trainingDataMat(4, 9, CV_32FC1, trainingData);

	int labels2[7] = { 0,1,-1,1,1,-1,0 };
	cv::Mat labelsMat2(7, 1, CV_32SC1, labels2);
	float trainingData2[7][2] = { {100,100},{300,80},{80,60},{30,20},{10,30}, {150,200},{130,250}};
	cv::Mat trainingDataMat2(7, 2, CV_32FC1, trainingData2);
	cv::Ptr<cv::ml::TrainData> tData2 = cv::ml::TrainData::create(trainingDataMat2, cv::ml::SampleTypes::ROW_SAMPLE, labelsMat2);

	

	cv::Mat responses;

	cv::Ptr<cv::ml::TrainData> tData = cv::ml::TrainData::create(trainingDataMat, cv::ml::SampleTypes::ROW_SAMPLE, labelsMat);
	cv::Ptr<cv::ml::TrainData> trData = cv::ml::TrainData::create(trainingDataMat, cv::ml::SampleTypes::ROW_SAMPLE, responses, cv::noArray(), cv::noArray(), cv::noArray());



	cv::Ptr<cv::ml::RTrees> rt = cv::ml::RTrees::create();

	rt->setMaxDepth(5);
	rt->setMinSampleCount(5);
	rt->setMaxCategories(10);

	rt->setCalculateVarImportance(false);
	rt->setRegressionAccuracy(0.0f);
	rt->setPriors(cv::Mat());

	rt->train(tData2, 0);

	cv::Mat sampleMat2 = (cv::Mat_<float>(1, 2) << 100, 100);
	cv::Mat sampleMat = (cv::Mat_<float>(1, 9) << 100, 100, 100, 100, 100, 100, 100, 100, 100);
	float predictResponse;
	predictResponse = rt->predict(sampleMat2);


	cout << "Predict 1: " << predictResponse << endl;

	sampleMat2 = (cv::Mat_<float>(1, 2) << 300, 80);
	predictResponse = rt->predict(sampleMat2);
	cout << "Predict 2: " << predictResponse << endl;

	cout << "HEJ" << endl;

	cv::Vec3b green(0, 255, 0), blue(255, 0, 0), red(0, 0, 255);
	// Show the decision regions given by the SVM
	for (int i = 0; i < image.rows; ++i)
		for (int j = 0; j < image.cols; ++j)
		{
			cv::Mat sampleMat = (cv::Mat_<float>(1, 2) << i, j);
			cv::Mat predictResponse;
			float predicted = rt->predict(sampleMat);


			if (predicted == 1)
				image.at<cv::Vec3b>(i, j) = green;
			else if (predicted == -1)
				image.at<cv::Vec3b>(i, j) = blue;
			else
				image.at<cv::Vec3b>(i, j) = red;

		}

	imshow("SVM Simple Example", image); // show it to the user

	cv::waitKey(0);


}

void svm_test()
{
	

	
	// Data for visual representation
    int width = 512, height = 512;
	cv::Mat image = cv::Mat::zeros(height, width, CV_8UC3);

    // Set up training data
    int labels[4] = {1, -1, 1, 1};
	cv::Mat labelsMat(4, 1, CV_32SC1, labels);

	float trainingData[4][9] = { {100,100,100,100,0,100,100,100,100},{ 100,100,100,100,100,100,100,100,100 },{ 100,100,100, 0,0,0,100,100,100 }, { 0,100,0, 100,0,100 , 0,100,0 }};
	cv::Mat trainingDataMat(4, 9, CV_32FC1, trainingData);

	int labels2[4] = { -1,1,0,2 };
	cv::Mat labelsMat2(4, 1, CV_32SC1, labels2);
	float trainingData2[4][2] = { { 20,40 },{ 60,80 },{ 80,60 },{ 30,20 } };
	cv::Mat trainingDataMat2(4, 2, CV_32FC1, trainingData2);
	cv::Ptr<cv::ml::TrainData> tData2 = cv::ml::TrainData::create(trainingDataMat2, cv::ml::SampleTypes::ROW_SAMPLE, labelsMat2);


	cv::Mat responses;

	cv::Ptr<cv::ml::TrainData> tData = cv::ml::TrainData::create(trainingDataMat, cv::ml::SampleTypes::ROW_SAMPLE, labelsMat);
	//Ptr<TrainData> trData = TrainData::create(trainingDataMat, SampleTypes::ROW_SAMPLE, responses,noArray(),noArray(),noArray());
	
	cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
	svm->setType(cv::ml::SVM::C_SVC);
	//svm->setKernel(SVM::LINEAR);
	svm->setGamma(1); svm->setKernel(cv::ml::SVM::CHI2);
	svm->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6));
	
	svm->train(tData2, 0);

	cv::Mat sampleMat2 = (cv::Mat_<float>(1, 2) << 100, 100);

	cv::Mat sampleMat = (cv::Mat_<float>(1, 9) << 100, 100, 100, 100, 50, 100, 100, 100, 100);
	cv::Mat predictResponse;
	svm->predict(sampleMat2, predictResponse, 0);

	cout << sampleMat << endl;

	cout << "Predict 1: " << predictResponse.at<float>(0, 0) << endl;
	
	sampleMat2 = (cv::Mat_<float>(1, 2) << 10, 10);
	sampleMat = (cv::Mat_<float>(1, 9) << 0, 0, 0, 0, 100, 0, 0, 0, 0);
	svm->predict(sampleMat2, predictResponse, 0);
	cout << "Predict 2: " << predictResponse.at<float>(0, 0) << endl;



	
	cv::Vec3b green(0,255,0), blue (255,0,0);
    // Show the decision regions given by the SVM
    for (int i = 0; i < image.rows; ++i)
        for (int j = 0; j < image.cols; ++j)
        {
			cv::Mat sampleMat = (cv::Mat_<float>(1,2) << i,j );
			cv::Mat predictResponse;
			svm->predict(sampleMat, predictResponse, 0);
			
			
            if (predictResponse.at<float>(0, 0) == 1)
                image.at<cv::Vec3b>(i,j)  = green;
            else if (predictResponse.at<float>(0, 0) == -1)
                 image.at<cv::Vec3b>(i,j)  = blue;
				 
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
	cv::Mat support = svm->getUncompressedSupportVectors();
	int c = support.rows;

	

    for (int i = 0; i < c; ++i)
    {
		float v[2];
        v[0] = support.at<float>(i,0);
		v[1] = support.at<float>(i, 1);
        circle( image, cv::Point( (int) v[0], (int) v[1]),   6, cv::Scalar(128, 128, 128), thickness, lineType);
    }

    //imwrite("result.png", image);        // save the image

	cv::imshow("SVM Simple Example", image); // show it to the user
	
	
	cv::waitKey(0);
	
}

void fast_test()
{
	cv::Mat inp = cv::imread("hmtest3.jpg", CV_LOAD_IMAGE_UNCHANGED);
	cv::Mat inpRez = resizeImg(inp,70,70);
	cv::Mat img;

	cv::namedWindow("0", 0);
	cv::imshow("0", inp);

	cv::cvtColor(inpRez, img, cv::COLOR_BGR2GRAY);

	//cout << inp.type() << endl;

	cv::namedWindow("-1", 0);
	cv::imshow("-1", inpRez);

	cv::waitKey(0);

	// Initiate FAST object with default values
	vector<cv::KeyPoint> keypoints;
	cout << keypoints.size() << endl;

	cv::FAST(img, keypoints, 10);// , false, FastFeatureDetector::TYPE_5_8);


	cout << keypoints.size() << endl;

	// find and draw the keypoints
	cv::Mat img2;
	cv::drawKeypoints(inpRez, keypoints, img2, cv::Scalar(150, 0, 0));

	cv::namedWindow("1", 1);
	cv::imshow("1", img2);

	vector<cv::KeyPoint> keypoints2;
	cv::FAST(img, keypoints, 60, false);

	cv::Mat img3;
	cv::drawKeypoints(inpRez, keypoints2, img3, cv::Scalar(255, 0, 0, 255));

	cv::namedWindow("2", 1);
	cv::imshow("2", img3);

}

void sift_test()
{
	/*
	ImageObj<uchar> image;
	image.read_pgm("img1.pgm");

	bool bExtractDescriptor = true;
	list<SiftKeypoint> kpt_list;
	// Perform SIFT computation on CPU.
	sift_cpu(image, kpt_list, bExtractDescriptor);
	// Generate output image
	//draw_keypoints_to_ppm_file("output.ppm", image, kpt_list);
	// Generate keypoints list
	//export_kpt_list_to_file("output.key", kpt_list, bExtractDescriptor);

	*/
}