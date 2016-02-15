#include "ImageSearcher.h"

vector<string> seekUsingImage(string catalogePath, string queryPath, int n)
{
	vector<ClothArticle*> allArticles = readCatalogeFromFile(catalogePath);

	ClothArticle* queryArticle = new ClothArticle("Input", queryPath, "Gra", "Top", -1);

	return findClosestNeighbours(allArticles, queryArticle, n, "None");;
}

vector<string> findClosestNeighbours(vector<ClothArticle*> allArticles, ClothArticle* query, int n, string testType)
{
	struct GreatTuple
	{
		bool operator()(const tuple<float, string>& lhs, const tuple<float, string>& rhs) const
		{
			return get<0>(lhs) > get<0>(rhs);
		}
	};

	priority_queue< tuple<float, string>, vector<tuple<float, string>>, GreatTuple > priQueue;

	Mat queryFeat = createFeatureVector(query, "Color+ClothingType");

	for (int i = 0; i < allArticles.size(); i++)
	{
		ClothArticle* curr = allArticles[i];
		if (testType == "ClothingType" && curr->getClType() == query->getClType() || testType == "Color" && curr->getColor() == query->getColor())
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
		//	cout << get<0>(priQueue.top()) << endl;
		priQueue.pop();
	}

	return topResults;
}

Mat createFeatureVector(ClothArticle* input, string testType)
{
	Mat fVec;
	ImageFeatures inpFeats = input->getImgFeats();

	if (testType == "Color")
	{
		fVec = Mat(1, 32 * 6, CV_32F);
		for (int j = 0; j < 6; j++)
		{
			Mat tmp;
			if (j<3)
			{
				tmp = inpFeats.getRGBHist(j);
			}
			else
			{
				tmp = inpFeats.getHSVHist(j - 3);
			}

			for (int k = 0; k < 32; k++)
			{
				fVec.at<float>(0, tmp.rows * j + k) = (float)tmp.at<float>(k, 0);
			}
		}
	}
	else if (testType == "ClothingType")
	{
		fVec = Mat(1, 2 * 100, CV_32FC1);

		Mat tmp = inpFeats.getEdgeHist(0);

		for (int j = 0; j < tmp.rows; j++)
		{
			fVec.at<float>(0, j) = tmp.at<float>(j, 0);
		}

		tmp = inpFeats.getEdgeHist(1);


		for (int j = 0; j < tmp.rows; j++)
		{
			fVec.at<float>(0, j + 100) = tmp.at<float>(j, 0);
		}
	}
	else if (testType == "Color+ClothingType" || testType == "ClothingType+Color")
	{
		fVec = Mat(1, 2 * 100 + 32 * 6, CV_32FC1);

		Mat tmp = inpFeats.getEdgeHist(0);

		for (int j = 0; j < tmp.rows; j++)
		{
			fVec.at<float>(0, j) = tmp.at<float>(j, 0);
		}

		tmp = inpFeats.getEdgeHist(1);

		for (int j = 0; j < tmp.rows; j++)
		{
			fVec.at<float>(0, j + 100) = tmp.at<float>(j, 0);
		}

		for (int j = 0; j < 6; j++)
		{
			if (j<3)
			{
				tmp = inpFeats.getRGBHist(j);
			}
			else
			{
				tmp = inpFeats.getHSVHist(j - 3);
			}

			for (int k = 0; k < 32; k++)
			{
				fVec.at<float>(0, tmp.rows * j + k + 200) = (float)tmp.at<float>(k, 0);
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

Ptr<TrainData> createTrainingData(vector<ClothArticle*> input, string classifierGroup)
{
	vector<int> labels; //color{ + clothType + sleeveType

	for (int i = 0; i < input.size(); i++)
	{
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

	Mat trainingDataMat(input.size(), dataMatFeature, CV_32FC1);
	for (int i = 0; i < labels.size(); i++)
	{
		Mat tmp = createFeatureVector(input[i], classifierGroup);

		for (int k = 0; k < tmp.cols; k++)
		{
			trainingDataMat.at<float>(i, k) = tmp.at<float>(0, k);
		}
	}

	Ptr<TrainData> tData = TrainData::create(trainingDataMat, SampleTypes::ROW_SAMPLE, labelsMat); // <-- Den här får fel data vid ClothingType

	return tData;
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
			dist += powf((fVec1.at<float>(i, j) - fVec2.at<float>(i, j)), 2);
		}
	}
	return sqrt(dist);
}