



#include <iostream>
#include <fstream>

//#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core.hpp>

#include "Config.h"
#include "ClothArticle.h"
#include "ImageUtilities.h"
#include "ImageSearcher.h"



#include "ezsift.h"
#include "image.h"
#include "img_io.h"

#define CONFIG_PATH "TSFS.conf"


//using namespace cv;
//using namespace cv::ml;
using namespace std;

void sift_test();
void fast_test();
void svm_test();
void rtrees_test();
void svmANDrfTest(string filename, string testType);
void testModelWithImage(string trainingFilename, string testFilename, string testType, bool loadModel = false);




#define MAIN
#ifdef MAIN
int main(int argc, char* argv[])
{
	Config::get().readConfigFile(CONFIG_PATH);
	Config::get().printConfig();

	if (argc == 3)
	{
		cout << argv[2] << endl;
	}

	if (argc == 1)
	{
		cout << "Too few arguments, use -h for help.";
	}
	else if (string(argv[1]) == "-f")
	{
		frontend("readyFile2.xx");
	}
	else if (string(argv[1]) == "-b")
	{
		if (argc == 2)
		{
			backend("readyFile2.xx", false, false);
		}
		else
		{
			backend("readyFile2.xx", true, false);
		}
	}
	else if (string(argv[1]) == "-h")
	{
		cout << "Use -f to start frontend process." << endl
			<< "Use -b to start backend process." << endl;
	}
	else if (string(argv[1]) == "--test")
	{
		testModelWithImage("readyFile2.xx", "indiskatest0.jpg", "ClothingType", false);  //<-- Bör funka på dirren
	}
	else if (string(argv[1]) == "--pirate")
	{
		//Mat output(frame1.rows * 2, frame1.cols * 2, frame1.type());
		//output.setTo(0);

		//frame1.copyTo(output(Rect(0, 0, frame1.cols, frame1.rows)));

		//cv::Mat input = cv::imread("indiskatest0.jpg", -1);

		//cv::Mat img = resizeImg(input, 300, 300);

		cv::Mat back(cv::Size(20, 25), CV_8U, cv::Scalar(0));
		cv::Mat front(cv::Size(10, 5), CV_8U, cv::Scalar(255));
		front.copyTo(back(cv::Rect(0, 0, front.cols, front.rows)));

		cv::Mat blopp(back);
		blopp /= 255;
		cv::Mat zSkeleton = skeletonizeZhangSuen(blopp);
		//cv::Mat skeleton = skeletonizeMorph(&back);

		cout << blopp << endl;

		cout << zSkeleton << endl;


		//ClothArticle *art = new ClothArticle("kov", "./testfiles/shirt3.png", "Rod", "Top", 0);

		/*
		cv::Mat img = cv::imread("./testfiles/dress0.jpg", cv::IMREAD_UNCHANGED);
		cv::Mat tmp2 = resizeImg(img, 300, 300);

		cv::Mat gray;
		cv::cvtColor(tmp2, gray, CV_BGR2GRAY);
		cv::Mat der = apply2derFilt(gray, true);

		cv::Mat thr;
		cv::threshold(der, thr, 120, cv::THRESH_BINARY_INV, cv::THRESH_BINARY);

		thr *= 255;
		*/

		cv::Mat img = cv::imread("./testfiles/skirt0.jpg", cv::IMREAD_UNCHANGED);
		cv::Mat img2 = resizeImg(img, 300, 300);

		cv::Mat imgGray;
		cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

		cv::Mat binary;
		cv::threshold(imgGray, binary, 248, cv::THRESH_BINARY_INV, cv::THRESH_BINARY_INV);
		binary = binary * 255;

		//cout << binary << endl;

		cout << "torvald " << endl;

		cv::namedWindow("binary", 0);
		cv::imshow("binary", binary);

		cv::waitKey(0);

		thinning(binary, binary);

		cv::namedWindow("binary", 0);
		cv::imshow("binary", binary);

		cv::waitKey(0);

		//cv::Mat binary2 = resizeImg(binary, 75, 75);



		//cv::Mat binary3(binary.size(), CV_8U);
		//binary.copyTo(binary3);
		//binary3 /= 255;
		//cv::Mat skeleton5 = skeletonizeZhangSuen(binary3);
		//skeleton5 *= 255;

		//cv::namedWindow("skeleton5", 0);
		//cv::imshow("skeleton5", skeleton5);

		cv::waitKey(0);
		cv::Mat skeleton2 = skeletonizeMorph(&binary);
		//cv::Mat skeleton4 = skeletonizeMorph(&binary2);

		cv::namedWindow("img", 0);
		cv::imshow("img", img);

		cv::namedWindow("skeleton2", 0);
		cv::imshow("skeleton2", skeleton2);

		//cv::namedWindow("skeleton4", 0);
		//cv::imshow("skeleton4", skeleton4);

		/*
		cv::namedWindow("THG", 0);
		cv::imshow("THG", thr);

		cv::namedWindow("BA", 0);
		cv::imshow("BA", gray);

		cv::namedWindow("DER", 0);
		cv::imshow("DER", der);
		*/
		cv::waitKey(0);

		/*
		ofstream outFile("b.bop", ios::out | ios::binary | ios::ate);
		if(outFile.is_open())
		{
			saveImgFeats(art->getImgFeats(), &outFile);
		}
		outFile.close();
		cout << "Igenom första." << endl;

		ifstream inFile("b.bop", ios::in | ios::binary);
		if (inFile.is_open())
		{
			ImageFeatures *bop = loadImgFeats(&inFile); // <--- Ger ut ImageFeatures med tomma vectorer

			delete bop;
		}
		inFile.close();
		*/

		/*
		vector<ClothArticle*> kov;
		kov.push_back(art);
		kov.push_back(art);
		kov.push_back(art);

		saveCataloge(&kov, "attans.bop");
		vector<ClothArticle*> *vok;
		vok = loadCataloge("attans.bop");
		cv::imshow("muffs", vok->at(0)->getImage());
		cv::waitKey(0);
		*/

		/*
		vector<ClothArticle*> *mov = readCatalogeFromFile("readyFile3.xx", false);
		saveCataloge(mov, "kov.try");
		vector<ClothArticle*> *moe = loadCataloge("kov.try");

		cout << mov->at(0)->getImgFeats()->getEdgeHist(0).at<float>(0, 0) << endl;
		cout << moe->at(0)->getImgFeats()->getEdgeHist(0).at<float>(0, 0) << endl;
		*/





		ofstream boutFile("b.bop", ios::out | ios::binary | ios::ate);
		if (boutFile.is_open())
		{
			saveMat(back, &boutFile);
			boutFile.close();
			cout << "before: " << (int)back.at<uchar>(0, 0) << endl;
		}


		ifstream binFile("b.bop", ios::in | ios::binary);
		if (binFile.is_open())
		{
			cv::Mat loaded = loadMat(&binFile);
			binFile.close();
			cout << "after: " << (int)loaded.at<uchar>(0, 0) << endl;
		}


		//cv::imshow("laugh", loaded);
		//cv::waitKey(0);

		return 0;
	}
	else if (string(argv[1]) == "--sobel")
	{
		cv::Mat src1 = cv::imread("testfiles/checkered0.jpg");
		cv::Mat img1 = resizeImg(src1, 300, 300);

		cv::Mat gray1;
		cv::cvtColor(img1, gray1, CV_BGR2GRAY);

		cv::Mat grad_x1, grad_y1;
		cv::Mat absGrad_x1, absGrad_y1;

		cv::Sobel(gray1, grad_x1, CV_16S, 2, 0, 3);
		cv::Sobel(gray1, grad_y1, CV_16S, 0, 2, 3);

		cv::convertScaleAbs(grad_x1, absGrad_x1);
		cv::convertScaleAbs(grad_y1, absGrad_y1);

		cv::Mat all1;
		cv::addWeighted(absGrad_x1, 0.5, absGrad_y1, 0.5, 0, all1);
		cv::imshow("absboth", all1);

		cv::Sobel(gray1, grad_x1, CV_8U, 2, 0, 3);
		cv::Sobel(gray1, grad_y1, CV_8U, 0, 2, 3);

		cv::addWeighted(grad_x1, 0.5, grad_y1, 0.5, 0, all1);
		cv::imshow("both", all1);

		cv::imshow("org", gray1);
		cv::imshow("x", grad_x1);
		cv::imshow("y", grad_y1);
		cv::imshow("absx", absGrad_x1);
		cv::imshow("absy", absGrad_y1);

		cv::waitKey();

	}
	else if (string(argv[1]) == "--window")
	{
		string query = "lök\nstolpe\nkadaver\n";

		string sTmp = query.substr(query.find('\n') + 1, query.length()  - query.find('\n'));
		string inputPath = sTmp.substr(0, sTmp.find('\n'));

		cout << sTmp << endl;
		cout << "---------------------------" << endl;
		cout << inputPath << endl;

		guiFrontend("readyFile2.xx");




	}
	else if (string(argv[1]) == "--rotate")
	{
		cv::Mat src1 = cv::imread("testfiles/checkered0.jpg");
		cv::Mat img1 = resizeImg(src1, 300, 300);

		cv::Mat hist;
		createGradiantHistogram(img1, hist, 4);

	}
	else if (string(argv[1]) == "--knug")
	{
		vector<ClothArticle*> *allArticles = readCatalogeFromFile("readyFile2.xx", false);
		clusterCataloge(allArticles, "Clusterer");

		cout << allArticles->at(0)->getClusterId() << endl;
		cout << allArticles->at(1)->getClusterId() << endl;
		cout << allArticles->at(2)->getClusterId() << endl;
		cout << allArticles->at(3)->getClusterId() << endl;
		cout << allArticles->at(4)->getClusterId() << endl;
		cout << allArticles->at(5)->getClusterId() << endl;


	}
	else if (string(argv[1]) == "--cluster")
	{
		const int MAX_CLUSTERS = 5;
		cv::Scalar colorTab[] =
		{
			cv::Scalar(0, 0, 255),
			cv::Scalar(0,255,0),
			cv::Scalar(255,100,100),
			cv::Scalar(255,0,255),
			cv::Scalar(0,255,255)
		};

		cv::Mat img(500, 500, CV_8UC3);
		cv::RNG rng(12345);

		while (true)
		{
			int k, clusterCount = rng.uniform(2, MAX_CLUSTERS + 1);
			int i, sampleCount = rng.uniform(1, 1001);
			cv::Mat points(sampleCount, 1, CV_32FC2), labels;

			clusterCount = MIN(clusterCount, sampleCount);
			cv::Mat centers;


			/* generate random sample from multigaussian distribution */
			for (k = 0; k < clusterCount; k++)
			{
				cv::Point center;
				center.x = rng.uniform(0, img.cols);
				center.y = rng.uniform(0, img.rows);
				cv::Mat pointChunk = points.rowRange(k*sampleCount / clusterCount,
					k == clusterCount - 1 ? sampleCount :
					(k + 1)*sampleCount / clusterCount);
				rng.fill(pointChunk, cv::RNG::NORMAL, cv::Scalar(center.x, center.y), cv::Scalar(img.cols*0.05, img.rows*0.05));
			}
			randShuffle(points, 1, &rng);
			kmeans(points, clusterCount, labels,
				cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0),
				3, cv::KMEANS_PP_CENTERS, centers);
			img = cv::Scalar::all(0);
			for (i = 0; i < sampleCount; i++)
			{
				int clusterIdx = labels.at<int>(i);
				cv::Point ipt = points.at<cv::Point2f>(i);
				circle(img, ipt, 2, colorTab[clusterIdx], cv::FILLED, cv::LINE_AA);
			}
			cout << centers << endl;
			for (i = 0; i < clusterCount; i++)
			{
				cv::Point ipt = cv::Point(centers.at<float>(i, 0), centers.at<float>(i, 1));
				circle(img, ipt, 2, cv::Scalar(175, 175, 175), cv::FILLED, cv::LINE_AA);
			}

			imshow("clusters", img);
			char key = (char)cv::waitKey();


			cv::Mat points2(5, 3, CV_32F), labels2;
			clusterCount = 2;
			cv::Mat centers2;


			points2.at<float>(0, 0) = 100;
			points2.at<float>(0, 1) = 150;
			points2.at<float>(0, 2) = 5;

			points2.at<float>(1, 0) = 250;
			points2.at<float>(1, 1) = 150;
			points2.at<float>(1, 2) = 2;

			points2.at<float>(2, 0) = 450;
			points2.at<float>(2, 1) = 350;
			points2.at<float>(2, 2) = 7;

			points2.at<float>(3, 0) = 300;
			points2.at<float>(3, 1) = 120;
			points2.at<float>(3, 2) = 3;

			points2.at<float>(4, 0) = 450;
			points2.at<float>(4, 1) = 150;
			points2.at<float>(4, 2) = 11;

			kmeans(points2, clusterCount, labels2,
				cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0),
				3, cv::KMEANS_PP_CENTERS, centers2);

			img = cv::Scalar::all(0);
			for (i = 0; i < 5; i++)
			{
				int clusterIdx = labels2.at<int>(i);
				cv::Point ipt = cv::Point(points2.at<float>(i, 0), points2.at<float>(i, 1));
				circle(img, ipt, (int)points2.at<float>(i, 2), colorTab[clusterIdx], cv::FILLED, cv::LINE_AA);
			}
			for (i = 0; i < 2; i++)
			{
				cv::Point ipt = cv::Point(centers2.at<float>(i, 0), centers2.at<float>(i, 1));
				circle(img, ipt, (int)centers2.at<float>(i, 2), cv::Scalar(175, 175, 175), cv::FILLED, cv::LINE_AA);
			}


			imshow("clusters", img);
			key = (char)cv::waitKey();
			if (key == 27 || key == 'q' || key == 'Q') // 'ESC'
				break;
		}



	}
	else if (string(argv[1]) == "--kaze")
	{
		const float inlier_threshold = 2.5f; // Distance threshold to identify inliers
		const float nn_match_ratio = 0.8f;   // Nearest neighbor matching ratio


		cv::Mat src1 = cv::imread("testfiles/shirt1.jpg");
		cv::Mat img1 = resizeImg(src1, 2, 2);
		cv::Mat src2 = cv::imread("testfiles/shirt2.jpg");
		cv::Mat img2 = resizeImg(src2);
		if (!src1.data)
			return -1;


		cv::Mat homography(cv::Size(3, 3), CV_32F, cv::Scalar(0));


		homography.at<float>(cv::Point(0, 0)) = 7.6285898e-01;
		homography.at<float>(cv::Point(1, 0)) = -2.9922929e-01;
		homography.at<float>(cv::Point(2, 0)) = 2.2567123e+02;
		homography.at<float>(cv::Point(0, 1)) = 3.3443473e-01;
		homography.at<float>(cv::Point(1, 1)) = 1.0143901e+00;
		homography.at<float>(cv::Point(2, 1)) = -7.6999973e+01;
		homography.at<float>(cv::Point(0, 2)) = 3.4663091e-04;
		homography.at<float>(cv::Point(1, 2)) = -1.4364524e-05;
		homography.at<float>(cv::Point(2, 2)) = 1.0000000e+00;


		vector<cv::KeyPoint> kpts1, kpts2;
		cv::Mat desc1, desc2;


		cout << kpts1.size() << endl;
		cv::Ptr<cv::Feature2D> akaze = cv::AKAZE::create();
		akaze->detectAndCompute(img1, cv::noArray(), kpts1, desc1);
		//akaze->detectAndCompute(img2, cv::noArray(), kpts2, desc2);
		cout << kpts1.size() << endl;

		cv::Mat imgK1;
		cv::drawKeypoints(img1, kpts1, imgK1);

		cv::imshow("AKAKAKAKAK", imgK1);
		cv::waitKey(0);

		return 0;

		/*
		cv::BFMatcher matcher(cv::NORM_HAMMING);
		vector< vector<cv::DMatch> > nn_matches;
		matcher.knnMatch(desc1, desc2, nn_matches, 2);

		vector<cv::KeyPoint> matched1, matched2, inliers1, inliers2;
		vector<cv::DMatch> good_matches;
		for (size_t i = 0; i < nn_matches.size(); i++) {
			cv::DMatch first = nn_matches[i][0];
			float dist1 = nn_matches[i][0].distance;
			float dist2 = nn_matches[i][1].distance;

			if (dist1 < nn_match_ratio * dist2) {
				matched1.push_back(kpts1[first.queryIdx]);
				matched2.push_back(kpts2[first.trainIdx]);
			}
		}

		for (unsigned i = 0; i < matched1.size(); i++) {
			cv::Mat col = cv::Mat::ones(3, 1, CV_64F);
			col.at<double>(0) = matched1[i].pt.x;
			col.at<double>(1) = matched1[i].pt.y;

			col = homography * col;
			col /= col.at<double>(2);
			double dist = sqrt(pow(col.at<double>(0) - matched2[i].pt.x, 2) +
				pow(col.at<double>(1) - matched2[i].pt.y, 2));

			if (dist < inlier_threshold) {
				int new_i = static_cast<int>(inliers1.size());
				inliers1.push_back(matched1[i]);
				inliers2.push_back(matched2[i]);
				good_matches.push_back(cv::DMatch(new_i, new_i, 0));
			}
		}

		cv::Mat res;
		drawMatches(img1, inliers1, img2, inliers2, good_matches, res);
		cv::imwrite("res.png", res);

		double inlier_ratio = inliers1.size() * 1.0 / matched1.size();
		cout << "A-KAZE Matching Results" << endl;
		cout << "*******************************" << endl;
		cout << "# Keypoints 1:                        \t" << kpts1.size() << endl;
		cout << "# Keypoints 2:                        \t" << kpts2.size() << endl;
		cout << "# Matches:                            \t" << matched1.size() << endl;
		cout << "# Inliers:                            \t" << inliers1.size() << endl;
		cout << "# Inliers Ratio:                      \t" << inlier_ratio << endl;
		cout << endl;
		*/


	}
	else if (string(argv[1]) == "--kov")
	{
		cv::Mat src = cv::imread("testfiles/lindex1.jpg");
		cv::Mat img = resizeImg(src);
		if (!src.data)
			return -1;

		cv::Mat bw;
		cv::cvtColor(img, bw, CV_BGR2GRAY);

		cv::Mat bin;
		cv::threshold(bw, bin, 240, 255, CV_THRESH_BINARY_INV);

		cv::Mat gau1, gau2;
		cv::GaussianBlur(bw, gau1, cv::Size(1, 1), 0);
		cv::GaussianBlur(bw, gau2, cv::Size(3, 3), 0);

		cv::Mat res;
		cv::absdiff(gau2, gau1, res);

		cv::equalizeHist(res, res);


		////////////////
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
			cv::Size(2 * 3/*erosion_size*/ + 1, 2 * 3/*erosion_size*/ + 1),
			cv::Point(3, 3/*erosion_size, erosion_size*/));

		cv::Mat erod;
		dilate(bin, erod, element);

		erode(erod, erod, element);
		////////////////



		cv::Mat out;
		onlyBackground(erod, out);
		out *= 255;

		cv::Mat edge = preformCanny(out, 80, 140);


		cv::Mat ssample = resizeImg(out, 50, 50);
		cv::Mat upsample;
		cv::resize(ssample, upsample, cv::Size(300, 300), 0.0, 0.0, cv::INTER_NEAREST);

		cv::imshow("ssample", ssample);
		cv::imshow("upsample", upsample);

		cv::imshow("out", out);
		cv::imshow("erod", erod);

		cv::imshow("bin", bin);
		cv::imshow("Gray", bw);
		cv::imshow("DoG", res);
		cv::imshow("edge", edge);

		cv::Mat kov;
		cv::bitwise_and(res, bin, kov);

		cv::Mat vok;
		cv::bitwise_and(res, edge, vok);


		cv::imshow("kov", kov);
		cv::imshow("vok", vok);


		cv::waitKey();
	}
	else if (string(argv[1]) == "--matt")
	{

		cv::Mat src = cv::imread("testfiles/shirt5.jpg");
		cv::Mat img = resizeImg(src);
		if (!src.data)
			return -1;

		cv::Mat bw;
		cv::cvtColor(img, bw, CV_BGR2GRAY);
		cv::imshow("kaf", bw);
		cv::waitKey();
		cv::threshold(bw, bw, 240, 255, CV_THRESH_BINARY_INV);



		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
			cv::Size(2 * 2/*erosion_size*/ + 1, 2 * 2/*erosion_size*/ + 1),
			cv::Point(2, 2/*erosion_size, erosion_size*/));

		/// Apply the erosion operation
		cv::Mat erod;
		dilate(bw, erod, element);

		erode(erod, erod, element);

		//cv::morphologyEx(bw, erod, cv::MORPH_CLOSE, element);

		cv::imshow("Erotion", erod);


		cv::imshow("laf", bw);
		cv::waitKey();

		cv::Mat thinned;
		thinning(erod, thinned);

		cv::Mat inSkel;
		erod.copyTo(inSkel);
		cv::Mat skelMorph = skeletonizeMorph(&inSkel);
		cv::bitwise_not(skelMorph, skelMorph);

		//thinned /= 255;
		cv::bitwise_not(thinned, thinned);
		thinned *= 255;

		cv::Mat sum;
		cv::bitwise_xor(thinned, erod, sum);

		cv::Mat sum2;
		cv::bitwise_xor(skelMorph, erod, sum2);

		cv::Mat sum3;
		cv::bitwise_and(skelMorph, thinned, sum3);
		cv::bitwise_xor(sum3, erod, sum3);

		cv::imshow("Sum3", sum3);
		cv::imshow("sum2", sum2);
		cv::imshow("skel", skelMorph);
		cv::imshow("thinned", thinned);
		cv::imshow("erod", erod);
		cv::imshow("sum", sum);
		cv::waitKey();
		return 0;

	}
	else
	{
		cout << "Invalid arguments, use -h for help.";
	}


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

	if (string(argv[1]) == "--SaRtest")
		svmANDrfTest("readyFile2.xx", "ClothingType");



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
#endif // MAIN




/**Tests a simple search in a cataloge, with the use of SVM or RF to filter out classes.
* 
* \params trainingFilename Path to the trainging cataloge.
* \params testFilename Path image query.
* \params testType The type of classifier that is going to be used as filter.
* \params loadModel Boolean indicating if the model should be loaded from file or not.
*/
void testModelWithImage(string trainingFilename, string testFilename, string testType, bool loadModel)
{
	vector<ClothArticle*> *allArticles = readCatalogeFromFile(trainingFilename, false);
	
	ClothArticle* testItem = new ClothArticle("Test0", testFilename, "Gra", "Top", -1);

	if(testType != "None")
	{
		cv::Ptr<cv::ml::SVM> model;
		if (testType == "Color" || testType == "All")
		{
			cv::Mat multVec;
			cv::Mat featVec = createFeatureVector(testItem);// , "Color");
			cv::Mat filtVec = createFilterVector(featVec.size(), "Color", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);

			if (!loadModel)
			{
				model = makeSVMModel(allArticles, "Color");
				model->save("ColorModel.xml");
			}
			else
			{
				model = cv::Algorithm::load<cv::ml::SVM>("ColorModel.xml");
			}

			float predictResponse = model->predict(multVec);

			cout << "Predicted: " << to_string(art_color((int)predictResponse)) << endl;
			testItem->setColor(art_color((int)predictResponse));
		}
		if (testType == "ClothingType" || testType == "All")
		{
			cv::Mat multVec;
			cv::Mat featVec = createFeatureVector(testItem);// , "Color");
			cv::Mat filtVec = createFilterVector(featVec.size(), "ClothingType", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);


			if (!loadModel)
			{
				model = makeSVMModel(allArticles, "ClothingType");
				model->save("ClTypeModel.xml");
			}
			else
			{
				model = cv::Algorithm::load<cv::ml::SVM>("ClTypeModel.xml");
			}

			float predictResponse = model->predict(multVec);

			cout << "Predicted: " << to_string(art_clType((int)predictResponse)) << endl;
			testItem->setClType(art_clType((int)predictResponse));
		}
		
	}

	vector<string> nn = findClosestNeighbours(allArticles, testItem, 11, "All", testType);

	cv::namedWindow("Query", 1);
	cv::imshow("Query", testItem->getImage());

	for (int i = 0; i < nn.size(); i++)
	{
		for (int j = 0; j < allArticles->size(); j++)
		{
			if (nn[i] == allArticles->at(j)->getId())
			{
				cv::namedWindow("Result # " + to_string(i + 1), 1);
				cv::imshow("Result # " + to_string(i + 1), allArticles->at(j)->getImage());
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

		cv::Mat noBluredges = preformCanny(binary, Config::get().CANNY_THRESH_LOW, Config::get().CANNY_THRESH_HIGH);

		cv::Mat imgBlur = preformGaussianBlur(imgGray);
		cv::Mat edges = preformCanny(imgBlur, Config::get().CANNY_THRESH_LOW, Config::get().CANNY_THRESH_HIGH);

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
	vector<ClothArticle*> *allArticles = readCatalogeFromFile(filename, false);

	int totSize = (int)allArticles->size();
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
				testArticles.push_back(allArticles->at(r));
			else
				trainingArticles.push_back(allArticles->at(r));
		}

		cv::Ptr<cv::ml::SVM> modelSVM;
		cv::Ptr<cv::ml::RTrees> modelRF;
		modelSVM = makeSVMModel(&trainingArticles, testType);
		modelRF = makeRTModel(&trainingArticles, testType);

		int svmHits = 0;
		int rtHits = 0;

		for (int i = 0; i < testArticles.size(); i++)
		{
			cv::Mat multVec;
			cv::Mat featVec = createFeatureVector(testArticles[i]);// , "Color");
			cv::Mat filtVec = createFilterVector(featVec.size(), testType, 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);

			float predictSVMResponse = modelSVM->predict(multVec);
			float predictRTResponse = modelRF->predict(multVec);

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
				
				if(false)
				{
					cout << "RF:  " << to_string(art_clType((int)predictRTResponse)) << endl;
					cout << "SVM: " << to_string(art_clType((int)predictSVMResponse)) << endl;
					cout << "Act: " << to_string(testArticles.at(i)->getClType()) << endl;

					cv::namedWindow("check", 1);
					cv::imshow("check", testArticles.at(i)->getImage());
					cv::waitKey(0);
				}
			}

		}
		cout << t << " RT Hits: " << rtHits << "\tRT Hitratio: " << (double)rtHits / (double)partSize << endl;
		cout << t << " SVM Hits: " << svmHits << "\tSVM Hitratio: " << (double)svmHits / (double)partSize << endl;

		bestRTHitRatio = max(bestRTHitRatio, (double)rtHits / (double)partSize);
		bestSVMHitRatio = max(bestSVMHitRatio, (double)svmHits / (double)partSize);

		totRTHits += rtHits;
		totSVMHits += svmHits;
	}

	for (int i = 0; i < allArticles->size(); i++)
	{
		delete(allArticles->at(i));
	}
	allArticles->clear();
	delete allArticles;

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