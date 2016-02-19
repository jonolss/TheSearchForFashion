#include "ImageSearcher.h"

/*
LPTSTR SlotName = TEXT("\\\\.\\mailslot\\sample_mailslot"); 

BOOL WriteSlot(HANDLE hSlot, LPTSTR lpszMessage) // skickar meddelanden
{
	BOOL fResult;
	DWORD cbWritten;

	fResult = WriteFile(hSlot,
		lpszMessage,
		(DWORD)(lstrlen(lpszMessage) + 1)*sizeof(TCHAR),
		&cbWritten,
		(LPOVERLAPPED)NULL);

	if (!fResult)
	{
		printf("WriteFile failed with %d.\n", GetLastError());
		return FALSE;
	}

	printf("Slot written to successfully.\n");

	return TRUE;
}

*/


LPTSTR SlotNameRet = TEXT("\\\\.\\mailslot\\sample_mailslot");

BOOL WriteSlot(HANDLE hSlotRet, LPTSTR lpszMessageRet) //skickar meddelanden
{
	BOOL fResult;
	DWORD cbWritten;

	fResult = WriteFile(hSlotRet,
		lpszMessageRet,
		(DWORD)(lstrlen(lpszMessageRet) + 1)*sizeof(TCHAR),
		&cbWritten,
		(LPOVERLAPPED)NULL);

	if (!fResult)
	{
		printf("WriteFile failed with %d.\n", GetLastError());
		return FALSE;
	}

	printf("Slot written to successfully.\n");

	return TRUE;
}


HANDLE hSlot;
LPTSTR SlotName = TEXT("\\\\.\\mailslot\\sample_mailslot");

string ReadSlot() // Läser meddelanden
{
	DWORD cbMessage, cMessage, cbRead;
	BOOL fResult;
	LPTSTR lpszBuffer;
	TCHAR achID[80];
	DWORD cAllMessages;
	HANDLE hEvent;
	OVERLAPPED ov;

	cbMessage = cMessage = cbRead = 0;

	hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("ExampleSlot"));
	if (NULL == hEvent)
		return "FALSE";
	ov.Offset = 0;
	ov.OffsetHigh = 0;
	ov.hEvent = hEvent;

	fResult = GetMailslotInfo(hSlot, // mailslot handle 
		(LPDWORD)NULL,               // no maximum message size 
		&cbMessage,                   // size of next message 
		&cMessage,                    // number of messages 
		(LPDWORD)NULL);              // no read time-out 

	if (!fResult)
	{
		printf("GetMailslotInfo failed with %d.\n", GetLastError());
		return "FALSE";
	}

	if (cbMessage == MAILSLOT_NO_MESSAGE)
	{
		printf("Waiting for a message...\n");
		return "TRUE";
	}
	
	lpszBuffer = (LPTSTR)GlobalAlloc(GPTR,
		lstrlen((LPTSTR)achID)*sizeof(TCHAR) + cbMessage);
	if (NULL == lpszBuffer)
		return "FALSE";
	lpszBuffer[0] = '\0';

	fResult = ReadFile(hSlot,
		lpszBuffer,
		cbMessage,
		&cbRead,
		&ov);

	if (!fResult)
	{
		printf("ReadFile failed with %d.\n", GetLastError());
		GlobalFree((HGLOBAL)lpszBuffer);
		return "FALSE";
	}

	string query = "";
	for (int i = 0; lpszBuffer[i] != '\0';i++)
	{
		query += (char)lpszBuffer[i];
	}

	GlobalFree((HGLOBAL)lpszBuffer);
	CloseHandle(hEvent);
	return query;
}

BOOL WINAPI MakeSlot(LPTSTR lpszSlotName) //Krävs för att kunna ta emot meddelanden
{
	hSlot = CreateMailslot(lpszSlotName,
		0,                             // no maximum message size 
		MAILSLOT_WAIT_FOREVER,         // no time-out for operations 
		(LPSECURITY_ATTRIBUTES)NULL); // default security

	if (hSlot == INVALID_HANDLE_VALUE)
	{
		printf("CreateMailslot failed with %d\n", GetLastError());
		return FALSE;
	}
	return TRUE;
}


bool t()
{
	vector<ClothArticle*> allArticles = readCatalogeFromFile("readyFile.xx");


	MakeSlot(SlotName);

	while (TRUE)
	{
		string res = ReadSlot();
		if (res == "FALSE")
			return false;
		else if (res != "TRUE" && res != "Message one for mailslot." && res != "Message two for mailslot." && res != "Message three for mailslot.")
		{
			ClothArticle* queryArticle = new ClothArticle("Input", res, "Rod", "Top", -1);
			vector<string> results = findClosestNeighbours(allArticles, queryArticle, 10, "None");
			cout << results[0] << endl;
		}
		else
		{
			cout << "Hej" << endl;
		}
		Sleep(1000);
	}
	

	/*
	HANDLE hFile;

	hFile = CreateFile(SlotName,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile failed with %d.\n", GetLastError());
		return FALSE;
	}

	WriteSlot(hFile, TEXT("Message one for mailslot."));
	WriteSlot(hFile, TEXT("Message two for mailslot."));

	Sleep(5000);

	WriteSlot(hFile, TEXT("Message three for mailslot."));

	CloseHandle(hFile);
	*/

	return true;
}


void engineLoop(string catalogePath)
{
	vector<ClothArticle*> allArticles = readCatalogeFromFile(catalogePath);

	cv::Ptr<cv::ml::RTrees> colorModel  = makeRTModel(allArticles, "Color");
	cv::Ptr<cv::ml::RTrees> clTypeModel = makeRTModel(allArticles, "ClothingType");
	
	while (true)
	{



	}

}



/**Finds the n closest neigboours to given files.
*
* \param catalogePath Path to file that holds all articles that is going to be searched.
* \param queryPath Path to an image who's closest neighbours are being sought.
* \param n Number of closest neighbours being returned.
* \return A vector on the n closest neighbours.
*/
vector<string> seekUsingImage(string catalogePath, string queryPath, int n)
{
	vector<ClothArticle*> allArticles = readCatalogeFromFile(catalogePath);

	ClothArticle* queryArticle = new ClothArticle("Input", queryPath, "Rod", "Top", -1);

	return findClosestNeighbours(allArticles, queryArticle, n, "None");
}

/**Finds the n closest neigboours to given input.
*
* \param allArticles All the articles that is going to be searched.
* \param query Article who's closest neighbours are being sought.
* \param n Number of closest neighbours being returned.
* \param testType Type of feature vector being used, e.g. "Color", "ClothingType" or "Color+ClothingType". 
* \return A vector on the n closest neighbours.
*/
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

	cv::Mat queryFeat = createFeatureVector(query, testType);

	for (int i = 0; i < allArticles.size(); i++)
	{
		ClothArticle* curr = allArticles[i];
		if (testType == "ClothingType" && curr->getClType() == query->getClType() || testType == "Color" && curr->getColor() == query->getColor())
		{
			cv::Mat currFeat = createFeatureVector(curr, testType);
			float dist = calcEuclDist(queryFeat, currFeat);

			tuple<float, string> currPriEntry;
			currPriEntry = make_tuple(dist, curr->getId());

			priQueue.push(currPriEntry);
		}
		else if (testType == "None")
		{
			cv::Mat currFeat = createFeatureVector(curr, "Color+ClothingType");
			float dist = calcEuclDist(queryFeat, currFeat);

			tuple<float, string> currPriEntry;
			currPriEntry = make_tuple(dist, curr->getId());

			priQueue.push(currPriEntry);
		}
		else if (testType == "All" && curr->getClType() == query->getClType() && curr->getColor() == query->getColor())
		{
			cv::Mat currFeat = createFeatureVector(curr, "Color+ClothingType");
			float dist = calcEuclDist(queryFeat, currFeat);

			tuple<float, string> currPriEntry;
			currPriEntry = make_tuple(dist, curr->getId());

			priQueue.push(currPriEntry);
		}
	}

	vector<string> topResults;

	if(false)
	{
		for (int i = 0; i < n; i++)
		{
			topResults.push_back(get<1>(priQueue.top()));
			cout << get<0>(priQueue.top()) << endl;

			for (int k = 0; k < allArticles.size(); k++)
			{
				if (allArticles[k]->getId() == get<1>(priQueue.top()))
				{
					cv::namedWindow(to_string(i), 1);
					cv::imshow(to_string(i), allArticles[k]->getImage());
				}
			}
			priQueue.pop();
		}
		cv::waitKey(0);
	}
	else
	{
		for (int i = 0; i < n; i++)
		{
			topResults.push_back(get<1>(priQueue.top()));
			cout << get<0>(priQueue.top()) << endl;
			priQueue.pop();
		}
	}
	
	return topResults;
}

/**Creates a feature vector.
*
* \param input Article who's feature vector is going to be extracted.
* \param testType Type of feature vector, e.g. "Color", "ClothingType" or "Color+ClothingType".
* \return Feature vector of the given article.
*/
cv::Mat createFeatureVector(ClothArticle* input, string testType)
{
	cv::Mat fVec;
	ImageFeatures inpFeats = input->getImgFeats();

	if (testType == "Color")
	{
		fVec = cv::Mat(1, 32 * 6, CV_32F);
		for (int j = 0; j < 6; j++)
		{
			cv::Mat tmp;
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
		fVec = cv::Mat(1, 2 * 100, CV_32FC1);

		cv::Mat tmp = inpFeats.getEdgeHist(0);

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
	else if (testType == "Color+ClothingType" || testType == "ClothingType+Color" || testType == "None" || testType == "All" )
	{
		fVec = cv::Mat(1, 2 * 100 + 32 * 6, CV_32FC1);

		cv::Mat tmp = inpFeats.getEdgeHist(0);

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

/**Creates a Support Vector Machine model.
*
* \param input Data that is going to be used for training the model.
* \param classifierGroup Type of classifiers, e.g. "Color" or "ClothingType".
* \return A trained Support Vector Machine model.
*/
cv::Ptr<cv::ml::SVM> makeSVMModel(vector<ClothArticle*> input, string testType)
{
	cv::Ptr<cv::ml::TrainData> tData = createTrainingData(input, testType);

	cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();

	svm->setType(cv::ml::SVM::C_SVC);
	//svm->setKernel(SVM::LINEAR);
	//svm->setKernel(SVM::POLY); svm->setDegree(2.0);
	svm->setKernel(cv::ml::SVM::CHI2);
	svm->setGamma(1);
	svm->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6));

	svm->train(tData, 0);

	return svm;
}

/**Creates a Random Trees model.
*
* \param input Data that is going to be used for training the model.
* \param classifierGroup Type of classifiers, e.g. "Color" or "ClothingType".
* \return A trained Random Trees model.
*/
cv::Ptr<cv::ml::RTrees> makeRTModel(vector<ClothArticle*> input, string testType)
{
	cv::Ptr<cv::ml::TrainData> tData = createTrainingData(input, testType);

	cv::Ptr<cv::ml::RTrees> rt = cv::ml::RTrees::create();

	rt->setMaxDepth(20);
	rt->setMinSampleCount(20);
	rt->setMaxCategories(20);

	rt->setCalculateVarImportance(false);
	rt->setRegressionAccuracy(0.0f);
	rt->setPriors(cv::Mat());

	rt->train(tData, 0);

	return rt;
}

/**Builds training data for creation of ML-models.
*
* \param input Data that is going to be used for training.
* \param classifierGroup Type of classifiers, e.g. "Color" or "ClothingType".
* \return Training data that can be used for creating ML-models.
*/
cv::Ptr<cv::ml::TrainData> createTrainingData(vector<ClothArticle*> input, string classifierGroup)
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

	cv::Mat labelsMat(labels.size(), 1, CV_32SC1);
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

	cv::Mat trainingDataMat(input.size(), dataMatFeature, CV_32FC1);
	for (int i = 0; i < labels.size(); i++)
	{
		cv::Mat tmp = createFeatureVector(input[i], classifierGroup);

		for (int k = 0; k < tmp.cols; k++)
		{
			trainingDataMat.at<float>(i, k) = tmp.at<float>(0, k);
		}
	}

	cv::Ptr<cv::ml::TrainData> tData = cv::ml::TrainData::create(trainingDataMat, cv::ml::SampleTypes::ROW_SAMPLE, labelsMat); // <-- Den här får fel data vid ClothingType

	return tData;
}


/**Calculates the euclidian distance between two 2D matrices of equal size.
*
* \param mat1 First Matrix.
* \param mat2 Second Matrix.
* \return The resulting matrix.
*/
float calcEuclDist(cv::Mat mat1, cv::Mat mat2)
{
	if (mat1.size() != mat2.size())
		return -1.0f;

	float dist = 0;
	for (int i = 0; i < mat1.rows; i++)
	{
		for (int j = 0; j < mat1.cols; j++)
		{
			dist += powf((mat1.at<float>(i, j) - mat2.at<float>(i, j)), 2);
		}
	}
	return sqrt(dist);
}