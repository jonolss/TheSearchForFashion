#include "ImageSearcher.h"

#define MODEL_COLOR_PATH  "ColorModel.xml"
#define MODEL_CLTYPE_PATH "ClTypeModel.xml"

#define BACK_TO_FRONT_SLOT TEXT("\\\\.\\mailslot\\btfs_mailslot")
#define FRONT_TO_BACK_SLOT TEXT("\\\\.\\mailslot\\ftbs_mailslot")

#define MAXIMUM_SEARCH_HITS 20

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

BOOL writeSlot(HANDLE hSlot, LPCWSTR lpszMessage) //skickar meddelanden
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
#ifdef _DEBUG
		printf("WriteFile failed with %d.\n", GetLastError());
#endif // !DEBUG
		return FALSE;
	}
#ifdef _DEBUG
	printf("Slot written to successfully.\n");
#endif // !DEBUG
	return TRUE;
}

string readSlot(HANDLE hSlot) // Läser meddelanden
{
	DWORD cbMessage, cMessage, cbRead;
	BOOL fResult;
	LPTSTR lpszBuffer;
	TCHAR achID[1024];
	DWORD cAllMessages;
	HANDLE hEvent;
	OVERLAPPED ov;

	cbMessage = cMessage = cbRead = 0;

	hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("ReadSlot"));
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
		return "";
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
	for (int i = 0; lpszBuffer[i] != '\0'; i++)
	{
		query += (char)lpszBuffer[i];
	}

	GlobalFree((HGLOBAL)lpszBuffer);
	CloseHandle(hEvent);
	return query;
}

BOOL WINAPI MakeReciSlot(LPTSTR lpszSlotName, HANDLE *hSlot) //Krävs för att kunna ta emot meddelanden
{
	*hSlot = CreateMailslot(lpszSlotName,
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

BOOL WINAPI MakeSendSlot(LPTSTR lpszSlotName, HANDLE *hFile)
{
	*hFile = CreateFile(lpszSlotName,
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

	return TRUE;
}

bool t()
{
	/*
	vector<ClothArticle*> allArticles = readCatalogeFromFile("readyFile.xx");


	MakeSlot(SlotName);

	while (TRUE)
	{
		string res = ReadSlot(SlotName);
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
	*/

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

BOOL isOnline(LPTSTR lpszSlotName, HANDLE *hSlot)
{
	if (!MakeSendSlot(lpszSlotName, hSlot))
		return FALSE;
	BOOL ret = writeSlot(*hSlot, TEXT("STATUS\n"));
	CloseHandle(*hSlot);
	return ret;
}

inline bool correctPath(const string& path) {
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

void printMainMenu(string message)
{ 
	cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl
		 << "$$                                                       $$" << endl
		 << "$$               THE SEARCH FOR FASHION                  $$" << endl
		 << "$$                                                       $$" << endl
		 << "$$                                                       $$" << endl
		 << "$$                                                       $$" << endl
		 << "$$                                                       $$" << endl
		 << "$$                                                       $$" << endl
		 << "$$   1.     Check status of backend.                     $$" << endl
		 << "$$   2.     Send a search query.                         $$" << endl
		 << "$$   START  Starts a backend process.                    $$" << endl
		 << "$$   END    Terminates backend process.                  $$" << endl
		 << "$$   end    Exits program.                               $$" << endl
		 << "$$                                                       $$" << endl
		 << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
	cout << message << endl;
}

unordered_map<string, string> makeIdToPathTable(vector<ClothArticle *> cataloge)
{
	unordered_map<string, string> hashTable;

	while (!cataloge.empty())
	{
		ClothArticle *tmp = cataloge.back();
		hashTable.insert(make_pair(tmp->getId(), tmp->getPath()));
		delete tmp;
		cataloge.pop_back();
	}
	return hashTable;
}

void frontend(string catalogePath)
{
	unordered_map<string, string> hashTable;
	hashTable = makeIdToPathTable(readCatalogeFromFile(catalogePath, true));

	HANDLE reciSlot;
	if (!MakeReciSlot(BACK_TO_FRONT_SLOT, &reciSlot))
		return;
	HANDLE sendSlot;


	string message = "";
	string inp = "";
	while (inp != "end")
	{
		printMainMenu(message);
		cin >> inp;
		if (inp == "1")
		{
			if (isOnline(FRONT_TO_BACK_SLOT, &sendSlot))
				message = "Backend is online.";
			else
				message = "Backend is offline.";
		}
		else if (inp == "END")
		{
			if (isOnline(FRONT_TO_BACK_SLOT, &sendSlot))
			{
				if (MakeSendSlot(FRONT_TO_BACK_SLOT, &sendSlot))
				{
					writeSlot(sendSlot, TEXT("END\n"));
					CloseHandle(sendSlot);
				}
				message = "Terminating backend.";
			}
			else
				message = "Backend is already offline.";
		}
		else if (inp == "START")
		{
			if (isOnline(FRONT_TO_BACK_SLOT, &sendSlot))
			{
				message = "Backend already up and running.";
			}
			else
			{
				STARTUPINFO si;
				PROCESS_INFORMATION pi;

				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				ZeroMemory(&pi, sizeof(pi));

				CreateProcess(TEXT("./OpenCV_test.exe"),   // the path
					TEXT("OpenCV_test.exe -b --embeded"),       // Command line
					NULL,           // Process handle not inheritable
					NULL,           // Thread handle not inheritable
					FALSE,          // Set handle inheritance to FALSE
					0,              // No creation flags
					NULL,           // Use parent's environment block
					NULL,           // Use parent's starting directory 
					&si,            // Pointer to STARTUPINFO structure
					&pi             // Pointer to PROCESS_INFORMATION structure
					);
				// Close process and thread handles. 
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				message = "Starting backend.";
			}
		}
		else if("2")
		{
			if(!isOnline(FRONT_TO_BACK_SLOT, &sendSlot))
			{
				message = "Couldn't send request, backend is offline.";
			}
			else if (MakeSendSlot(FRONT_TO_BACK_SLOT, &sendSlot))
			{
				string query = "imgSearch\n";
				string inputPath;

				bool valid = false;
				do
				{
					cout << "Input path to search query: ";
					cin >> inputPath;

					if (inputPath == "Defualt")
					{
						query += "./hmtest3.jpg\n";
						valid = true;
					}
					else if (correctPath(inputPath))
					{
						query += inputPath + '\n';
						valid = true;
					}
					else
					{
						cout << "Incorrect path." << endl;
					}
				} while (!valid);

				valid = false;
				do
				{
					cout << "Input number of hits (maximum size = " + to_string(MAXIMUM_SEARCH_HITS) + "): ";
					string inputHits;
					cin >> inputHits;

					if (inputHits == "Defualt")
					{
						query += "5\n";
						valid = true;
					}
					try
					{
						int n = stoi(inputHits);
						if (n > MAXIMUM_SEARCH_HITS)
							throw 2;
						query += inputHits + '\n';
						valid = true;
					}
					catch (exception e)
					{
						cout << "Invalid input, input must be an integer." << endl;
					}
					catch (int e)
					{
						cout << "Invalid input, input must be an integer smaller or equal to " + to_string(MAXIMUM_SEARCH_HITS) << endl;
					}
				} while (!valid);

				valid = false;
				do
				{
					cout << "Choose filters (0 - None, 1 - Same Color, 2 - Same Clothing Type, 3 - Color and ClothType at same time," << endl 
						 << "4 - Only Color Vectors, 5 - Only Shape Vectors): ";
					string filter;
					cin >> filter;
					try 
					{
						if (filter == "0")
						{
							query += "None\n";
							valid = true;
						}
						else if (filter == "1")
						{
							query += "Color\n";
							valid = true;
						}
						else if (filter == "2")
						{
							query += "ClothingType\n";
							valid = true;
						}
						else if (filter == "3")
						{
							query += "All\n";
							valid = true;
						}
						else if (filter == "4")
						{
							query += "Color_NoML\n";
							valid = true;
						}
						else if (filter == "5")
						{
							query += "ClType_NoML\n";
							valid = true;
						}
						else
						{
							throw 2;
						}
						int b = stoi(filter);
					}
					catch (exception e)
					{
						cout << "Invalid input, must be integer." << endl;
					}
					catch (int e)
					{
						cout << "Invalid input, invalid integer value." << endl;
					}
				} while (!valid);


				string heb = "imgSearch\n";
				heb += "./hmtest3.jpg\n";
				heb += "5\n";
				heb += "All\n";

				cout << query << endl;
				cout << heb << endl;

				std::wstring stemp = s2ws(query);
				LPCWSTR lquery = stemp.c_str();
				writeSlot(sendSlot, lquery);
				CloseHandle(sendSlot);

				vector<string> results;
				while (results.empty())
				{
					string request = readSlot(reciSlot);

					if (request == "FALSE")
						return;

					if (request != "")
					{
						char tmp;
						int pos = request.find('\n');
						while (pos != string::npos)
						{
							results.push_back(request.substr(0, pos));
							request = request.substr(pos + 1, request.length() - pos + 1);
							pos = request.find('\n');
						}
					}
					else
					{
						Sleep(1);
					}
				}

				cv::Mat tmpImg = cv::imread(inputPath, cv::IMREAD_UNCHANGED);
				cv::Mat tmpImg2 = resizeImg(tmpImg, 300, 300);
				cv::namedWindow("Query", 1);
				cv::imshow("Query", tmpImg2);
				for (int i = 0; i < results.size(); i++)
				{
					cout << results[i] << " : " << hashTable[results[i]] << endl;
					tmpImg = cv::imread(hashTable[results[i]], cv::IMREAD_UNCHANGED);
					cout << hashTable[results[i]] << endl;
					if (hashTable[results[i]].find(".png") != string::npos)
						filterAlphaArtifacts(&tmpImg);
					tmpImg2 = resizeImg(tmpImg, 300, 300);
					cv::namedWindow("Result # " + to_string(i + 1), 1);
					cv::imshow("Result # " + to_string(i + 1), tmpImg2);
				}
				cv::waitKey(0);
				cv::destroyAllWindows();
			}
		}
	}

}

int backend(string catalogePath, bool embeded, bool loadModel)
{
	
	vector<ClothArticle*> allArticles = readCatalogeFromFile(catalogePath, false);

	cv::Ptr<cv::ml::RTrees> colorModel = makeRTModel(allArticles, "Color");
	cv::Ptr<cv::ml::RTrees> clTypeModel = makeRTModel(allArticles, "ClothingType");
	if (!loadModel)
	{
		colorModel = makeRTModel(allArticles, "Color");
		colorModel->save(MODEL_COLOR_PATH);
		clTypeModel = makeRTModel(allArticles, "ClothingType");
		clTypeModel->save(MODEL_CLTYPE_PATH);
	}
	else
	{
		colorModel = cv::Algorithm::load<cv::ml::RTrees>(MODEL_COLOR_PATH);
		clTypeModel = cv::Algorithm::load<cv::ml::RTrees>(MODEL_CLTYPE_PATH);
	}


	HANDLE reciSlot;
	if (!MakeReciSlot(FRONT_TO_BACK_SLOT, &reciSlot))
		return 1;
	HANDLE sendSlot;
	
	while (true)
	{

		vector<string> reqArgs;
		
		if(!embeded)
		{
			cout << "Awaiting new query..." << endl;
		}

		while (reqArgs.empty())
		{
			string request = readSlot(reciSlot);

			if (request == "FALSE")
				return 1;


			if(request != "")
			{
				char tmp;
				int pos = request.find('\n');
				while (pos != string::npos)
				{
					reqArgs.push_back(request.substr(0, pos));
					request = request.substr(pos+1, request.length() - pos+1);
					pos = request.find('\n');
				}
			}
			else
			{
				Sleep(1);
			}
		}
		
		string reqType = reqArgs[0];
		if (reqType == "END")
		{
			return 0;
		}
		else if (reqType == "imgSearch")
		{
			string path    = reqArgs[1];
			int    n       = stoi(reqArgs[2]);
			string filters = reqArgs[3];

			ClothArticle* queryArticle = new ClothArticle("Query", path, "Rod", "Top", -1);

			cv::Mat featVec = createFeatureVector(queryArticle, "Color");
			queryArticle->setColor(art_color((int)colorModel->predict(featVec)));

			featVec = createFeatureVector(queryArticle, "ClothingType");
			queryArticle->setClType(art_clType((int)clTypeModel->predict(featVec)));


			vector<string> closeNeigh = findClosestNeighbours(allArticles, queryArticle, n, filters);

			string answer = "";
			for (int i = 0; i < closeNeigh.size(); i++)
			{
				answer += closeNeigh[i] + '\n';
			}

			if (MakeSendSlot(BACK_TO_FRONT_SLOT, &sendSlot))
			{
				std::wstring stemp = s2ws(answer);
				LPCWSTR ans = stemp.c_str();
				writeSlot(sendSlot, ans);
				CloseHandle(sendSlot);
			}
		}
		else if (reqType == "STATUS")
		{
			;
		}
		else
		{
			if (MakeSendSlot(BACK_TO_FRONT_SLOT, &sendSlot))
			{
				writeSlot(sendSlot, TEXT("Invalid input.\n"));
				CloseHandle(sendSlot); 
			}
		}

		
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
	vector<ClothArticle*> allArticles = readCatalogeFromFile(catalogePath, false);

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
			float dist = calcEuclDist(queryFeat, currFeat, cv::Mat());

			tuple<float, string> currPriEntry;
			currPriEntry = make_tuple(dist, curr->getId());

			priQueue.push(currPriEntry);
		}
		else if (testType == "ClType_NoML" || testType == "Color_NoML")
		{
			cv::Mat currFeat = createFeatureVector(curr, testType);
			float dist = calcEuclDist(queryFeat, currFeat, cv::Mat());

			tuple<float, string> currPriEntry;
			currPriEntry = make_tuple(dist, curr->getId());

			priQueue.push(currPriEntry);
		}
		else if (testType == "None")
		{
			cv::Mat currFeat = createFeatureVector(curr, "Color+ClothingType");
			float dist = calcEuclDist(queryFeat, currFeat, cv::Mat());

			tuple<float, string> currPriEntry;
			currPriEntry = make_tuple(dist, curr->getId());

			priQueue.push(currPriEntry);
		}
		else if (testType == "All" && curr->getClType() == query->getClType() && curr->getColor() == query->getColor())
		{
			cv::Mat currFeat = createFeatureVector(curr, "Color+ClothingType");
			float dist = calcEuclDist(queryFeat, currFeat, cv::Mat());

			tuple<float, string> currPriEntry;
			currPriEntry = make_tuple(dist, curr->getId());

			priQueue.push(currPriEntry);
		}
	}

	vector<string> topResults;

#ifdef _DEBUG
	cout << to_string(query->getClType()) << endl << to_string(query->getColor()) << endl;
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
		int maxIter = (n < priQueue.size() ? n : priQueue.size());
		for (int i = 0; i < maxIter; i++)
		{
			topResults.push_back(get<1>(priQueue.top()));
			cout << get<0>(priQueue.top()) << endl;
			priQueue.pop();
		}
	}
#else

	int maxIter = (n < priQueue.size() ? n : priQueue.size());
	for (int i = 0; i < maxIter; i++)
	{
		topResults.push_back(get<1>(priQueue.top()));
		priQueue.pop();
	}

#endif


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

	if (testType == "Color" || testType == "Color_NoML")
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
	else if (testType == "ClothingType" || testType == "ClType_NoML")
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
float calcEuclDist(cv::Mat mat1, cv::Mat mat2, cv::Mat scale)
{
	if (mat1.size() != mat2.size())
		return -1.0f;

	if (scale.size() == cv::Size(0,0))
	{
		scale = cv::Mat(mat1.size(), CV_32FC1, cv::Scalar::all(1));
	}

	float dist = 0;
	for (int i = 0; i < mat1.rows; i++)
	{
		for (int j = 0; j < mat1.cols; j++)
		{
			dist += powf(scale.at<float>(i,j) * ((mat1.at<float>(i, j) - mat2.at<float>(i, j))), 2);
		}
	}
	return sqrt(dist);
}