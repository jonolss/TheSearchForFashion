/**
* @file
* @author Jonathan Olsson <jonolss92@gmail.com>
* @version 1.0
*
* @section LICENSE
*
* Here I will have information about License.
*
* @section DESCRIPTION
*
* Main part of the application, all relevant functions for image searching.
*
*/

#include "ImageSearcher.h"

HANDLE reciSlot;
HANDLE sendSlot;
unordered_map<string, string> hashTable;
LPCSTR g_szClassName = TEXT("myWindowClass");

/**Check if the path is valid.
*
* \param path The path that is being validated.
* \return Return true if path is valid, false otherwise.
*/
inline bool validPath(const string& path) {
	if (path == Config::get().TEST_FOLDER)
		return false;
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}


/**Makes a hashmap between an item's id and the path to its image. 
*
* \param cataloge Cataloge of items.
* \return A hashmap between id and path.
*/
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

/**Checks if search query string is valid.
* Old variant, not used for webb interface.
*
* \param query Search query string.
* \return Returns true if query is valid, false otherwise.
*/
bool validQuery(string query)
{
	istringstream iss(query);

	int count = 0;
	string line;
	while (getline(iss, line))
	{
		if (count == 0 && line != "imgSearch")
			return false;
		else if (count == 1 && !validPath(line))
			return false;
		else if (count == 2)
		{
			try
			{
				int n = stoi(line);
				if (n > Config::get().MAXIMUM_SEARCH_HITS)
					return false;
			}
			catch (exception e)
			{
				return false;
			}
		}
		else if (count == 3 && line == "")
			return false;
		else if (count == 4 && line == "")
			return false;
		else if (count == 5 && line == "")
			return false;
		else if (count > 5)
			return false;

		count++;
	}
	if (count < 5)
		return false;

	return true;
}

/**Checks if search query string is valid.
*
* \param query Search query string.
* \return Returns true if query is valid, false otherwise.
*/
bool validQuery2(string query)
{
	istringstream iss(query);

	int count = 0;
	string line;
	while (getline(iss, line, ';'))
	{
		cout << to_string(count) + ": " + line << endl;

		if (count == 0 && line != "imgSearch2")
			return false;
		else if (count == 1 && !validPath(line))
			return false;
		else if (count == 2 && line == "")
			return false;
		count++;
	}
	if (count < 3)
		return false;

	return true;
}

/**Creates a basic SECURITY_ATTRIBUTES
*
* \param se A SECURITY_ATTRIBUTES pointer.
*/
void createSemSecAtt(SECURITY_ATTRIBUTES *sa)
{
	DWORD dwRes, dwDisposition;
	PSID pEveryoneSID = NULL, pAdminSID = NULL;
	PACL pACL = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	EXPLICIT_ACCESS ea[1];
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld =
		SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
	LONG lRes;
	HKEY hkSub = NULL;

	if (!AllocateAndInitializeSid(&SIDAuthWorld, 1,
		SECURITY_WORLD_RID,
		0, 0, 0, 0, 0, 0, 0,
		&pEveryoneSID))
	{
		_tprintf(_T("AllocateAndInitializeSid Error %u\n"), GetLastError());
		goto Cleanup;
	}

	// Create a well-known SID for the Everyone group.
	if (!AllocateAndInitializeSid(&SIDAuthWorld, 1,
		SECURITY_WORLD_RID,
		0, 0, 0, 0, 0, 0, 0,
		&pEveryoneSID))
	{
		_tprintf(_T("AllocateAndInitializeSid Error %u\n"), GetLastError());
		goto Cleanup;
	}

	// Initialize an EXPLICIT_ACCESS structure for an ACE.
	// The ACE will allow Everyone read access to the key.
	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
	ea[0].grfAccessPermissions = KEY_ALL_ACCESS;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName = (LPTSTR)pEveryoneSID;


	// Create a new ACL that contains the new ACE.
	dwRes = SetEntriesInAcl(1, ea, NULL, &pACL);
	if (ERROR_SUCCESS != dwRes)
	{
		_tprintf(_T("SetEntriesInAcl Error %u\n"), GetLastError());
		goto Cleanup;
	}

	// Initialize a security descriptor.  
	pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR,
		SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (NULL == pSD)
	{
		_tprintf(_T("LocalAlloc Error %u\n"), GetLastError());
		goto Cleanup;
	}

	if (!InitializeSecurityDescriptor(pSD,
		SECURITY_DESCRIPTOR_REVISION))
	{
		_tprintf(_T("InitializeSecurityDescriptor Error %u\n"),
			GetLastError());
		goto Cleanup;
	}

	// Add the ACL to the security descriptor. 
	if (!SetSecurityDescriptorDacl(pSD,
		TRUE,     // bDaclPresent flag   
		pACL,
		FALSE))   // not a default DACL 
	{
		_tprintf(_T("SetSecurityDescriptorDacl Error %u\n"),
			GetLastError());
		goto Cleanup;
	}

	// Initialize a security attributes structure.
	sa->nLength = sizeof(SECURITY_ATTRIBUTES);
	sa->lpSecurityDescriptor = pSD;
	sa->bInheritHandle = FALSE;

Cleanup:

	if (pEveryoneSID)
		FreeSid(pEveryoneSID);
	if (pAdminSID)
		FreeSid(pAdminSID);
	if (pACL)
		LocalFree(pACL);
	if (pSD)
		LocalFree(pSD);
	if (hkSub)
		RegCloseKey(hkSub);

	return;


}

typedef struct ReadData {
	LPWSTR fileName;
	queue<string> *jobs;
} READDATA, *PREADDATA;

#define BUFFER_SIZE 10000
DWORD WINAPI readingFromFile(LPVOID lpParam)
{
	PREADDATA arg = (PREADDATA)lpParam;

	LPWSTR fileName = arg->fileName;
	queue<string> *jobs = arg->jobs;
	
	DWORD cbRead;
	char inBuf[BUFFER_SIZE];

	cout << fileName << endl;

	while (true)
	{
		HANDLE inFile = NULL;
		while (inFile == NULL)
		{
			inFile = CreateFile(
				_TEXT("D:\\tsff_front2back"),//fileName, //_TEXT("D:\\test.txt"),  //_TEXT("\\.\\tsff_in"),
				GENERIC_READ | GENERIC_WRITE,
				0, //FILE_SHARE_WRITE |FILE_SHARE_WRITE,
				NULL,
				OPEN_ALWAYS, //CREATE_ALWAYS,
				FILE_ATTRIBUTE_HIDDEN | FILE_FLAG_DELETE_ON_CLOSE,
				NULL);

			if (inFile == NULL)
			{
				cout << "Couldn't open file {error: " << GetLastError() << "}." << endl;
				Sleep(10);
			}

			if (inFile == INVALID_HANDLE_VALUE)
			{
				cout << "INVALID_HANDLE_VALUE" << endl;
				cout << "Couldn't open file {error: " << GetLastError() << "}." << endl;
			}
		}
		
		memset(inBuf, 0, BUFFER_SIZE);
		ReadFile(inFile, inBuf, BUFFER_SIZE, &cbRead, NULL);

		string request = string(inBuf);
		/*
		if (request.length() != 0)
		{
			string stmp = request;
			int pos = stmp.find('\t');
			while (pos != string::npos)
			{
				jobs->push(stmp.substr(0, pos) + "\n");
				stmp = stmp.substr(pos + 1, stmp.length() - pos + 1);
				pos = stmp.find('\t');
			}
		}*/
		if (request.length() != 0)
		{
			string stmp = request;
			int pos = stmp.find('\n');
			while (pos != string::npos)
			{
				jobs->push(stmp.substr(0, pos) + ";");
				stmp = stmp.substr(pos + 1, stmp.length() - pos + 1);
				pos = stmp.find('\n');
			}
		}
		CloseHandle(inFile);
		Sleep(10);
	}
}


DWORD WINAPI readingFromFileV2(LPVOID lpParam)
{
	HANDLE inFile = CreateFile(
		_TEXT("D:\\tsff_front2back"), //_TEXT("D:\\test.txt"),  //_TEXT("\\.\\tsff_in"),
		GENERIC_READ,
		FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		CREATE_NEW, //CREATE_ALWAYS,
		FILE_ATTRIBUTE_HIDDEN | FILE_FLAG_DELETE_ON_CLOSE,
		NULL);

	if (inFile == NULL)
	{
		cout << "Couldn't open file {error: " << GetLastError() << "}." << endl;
	}



	DWORD cbRead;
	char inBuf[BUFFER_SIZE];

	while (true)
	{
		memset(inBuf, 0, BUFFER_SIZE);
		ReadFile(inFile, inBuf, BUFFER_SIZE, &cbRead, NULL);

		string request = string(inBuf);

		if (request.length() != 0)
		{
			cout << request << endl;
		}
		Sleep(10);
	}
}

/**Initialises the backend for the web-based interface.
* Starts a thread that continously reads a file where requests can be sent.
* The requests are then computed in the main thread and the results are sent to another file, where they can be read by the requesting program.
* Filenames are hard coded at the moment, requests should be written in "D:\\tsff_front2back" and results can be found in "D:\\tsff_back2front".
*
* \param catalogePath Path to the cataloge-file.
*
*/
int webBackend(string catalogePath)
{
	vector<ClothArticle*> *tmp = readCatalogeFromFile(catalogePath, true);
	hashTable = makeIdToPathTable(*tmp);

	vector<ClothArticle*> *allArticles;
	cv::Ptr<cv::ml::RTrees> colorModel;
	cv::Ptr<cv::ml::RTrees> clTypeModel;
	cv::Ptr<cv::ml::RTrees> clustSillModel;
	cv::Ptr<cv::ml::RTrees> clustColorModel;
	cv::Ptr<cv::ml::RTrees> clustClTypeModel;
	if (validPath(catalogePath + Config::get().SAVE_EXTENTION) && validPath(catalogePath + Config::get().MODEL_COLOR_EXTENTION) && validPath(catalogePath + Config::get().MODEL_CLTYPE_EXTENTION))
	{
		cout << Config::get().SAVE_EXTENTION << endl;
		cout << Config::get().MODEL_COLOR_EXTENTION << endl;
		cout << Config::get().MODEL_CLTYPE_EXTENTION << endl;
		allArticles = loadCataloge(catalogePath + Config::get().SAVE_EXTENTION);
		colorModel = cv::Algorithm::load<cv::ml::RTrees>(catalogePath + Config::get().MODEL_COLOR_EXTENTION);
		clTypeModel = cv::Algorithm::load<cv::ml::RTrees>(catalogePath + Config::get().MODEL_CLTYPE_EXTENTION);
	}
	else
	{
		allArticles = readCatalogeFromFile(catalogePath, false);
#ifdef _FILTERS
		clusterCataloge(allArticles, "Silhouette");
		clusterCataloge(allArticles, "ClustColor");
		clusterCataloge(allArticles, "ClustClType");
		colorModel = makeRTModel(allArticles, "Color");
		clTypeModel = makeRTModel(allArticles, "ClothingType");
		clustSillModel = makeRTModel(allArticles, "Silhouette");
		clustColorModel = makeRTModel(allArticles, "ClustColor");
		clustClTypeModel = makeRTModel(allArticles, "ClustClType");
		colorModel->save(catalogePath + Config::get().MODEL_COLOR_EXTENTION);
		clTypeModel->save(catalogePath + Config::get().MODEL_CLTYPE_EXTENTION);
#endif
	}

	
	char outBuf[BUFFER_SIZE];

	DWORD cbWritten;
	DWORD dwBytesToWrite = (DWORD)strlen(outBuf);


	int count = 0;
	
	
	//Gör den här till en egen tråd som konstant läser från input.
	//Gör en liknande för utskrift när huvudtråden gjort sitt.
	HANDLE readThread;
	DWORD readThreadId;
	

	
	queue<string> *jobs = new queue<string>();

	PREADDATA thArgs = (PREADDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(READDATA));
	thArgs->fileName = _TEXT(L"D:\\tsff_front2back");
	thArgs->jobs = jobs;

	readThread = CreateThread(
		NULL,
		0,
		readingFromFile,
		thArgs,
		0,
		&readThreadId
		);


	//Request loop, if no new request, waits for 10ms.
	while (true)
	{
		cout << count++ << endl;
		vector<string> reqArgs;
		
		while (jobs->empty())
		{
			Sleep(10);
		}
		
		//memset(inBuf, 0, BUFFER_SIZE);
		//ReadFile(hPipe1, inBuf, dwBytesToRead, &cbRead, NULL);

		//string request = string(inBuf);
		string request = jobs->front();
		jobs->pop();

		cout << request << endl;

		string stmp = request;
		char ctmp;
		int pos = stmp.find(';');//stmp.find('\n');
		while (pos != string::npos)
		{
			reqArgs.push_back(stmp.substr(0, pos));
			stmp = stmp.substr(pos + 1, stmp.length() - pos + 1);
			pos = stmp.find(';');//stmp.find('\n');
		}

		for (int i = 0; i < reqArgs.size(); i++)
		{
			cout << to_string(i) + ": " + reqArgs[i] << endl;
		}

		string reqType = reqArgs[0];
		if (reqType == "imgSearch2" && validQuery2(request)) //Current formating of requests and results.
		{
			string path = reqArgs[1];
			int    n = 12;                                                       //stoi(reqArgs[2]);
			string fVecType = "ClothingType,Silhouette,Pattern,Color,Template,"; //reqArgs[3];
			string fVecVals = "1,1,2,2,2";                                       //reqArgs[4];  // <--- fixa in dom här till featFilter
			string filters = "None";                                             //reqArgs[5];

			ClothArticle* queryArticle = new ClothArticle("Query", path, "Rod", "Top", -1);

#ifdef _FILTERS
			cv::Mat multVec;
			cv::Mat featVec = createFeatureVector(queryArticle);// , "Color");
			cv::Mat filtVec = createFilterVector(featVec.size(), "Color", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);
			queryArticle->setColor(art_color((int)colorModel->predict(multVec)));

			featVec = createFeatureVector(queryArticle);
			filtVec = createFilterVector(featVec.size(), "ClothingType", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);
			queryArticle->setClType(art_clType((int)clTypeModel->predict(multVec)));

			featVec = createFeatureVector(queryArticle);
			filtVec = createFilterVector(featVec.size(), "Silhouette", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);
			queryArticle->setClusterId((int)clustSillModel->predict(multVec));

			featVec = createFeatureVector(queryArticle);
			filtVec = createFilterVector(featVec.size(), "ClustColor", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);
			queryArticle->setClusterColor((int)clustColorModel->predict(multVec));

			featVec = createFeatureVector(queryArticle);
			filtVec = createFilterVector(featVec.size(), "ClustClType", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);
			queryArticle->setClusterClType((int)clustClTypeModel->predict(multVec));
#endif
			vector<string> fVecTypes;
			{
				char tmp[120];
				//strcpy(tmp, fVecType.c_str());
				strcpy_s(tmp, fVecType.c_str());
				char *tmpPoint;
				char **tmpContext = NULL;
				//tmpPoint = strtok(tmp, ",");
				tmpPoint = strtok_s(tmp, ",", tmpContext);
				while (tmpPoint != NULL)
				{
					fVecTypes.push_back(string(tmpPoint));
					//tmpPoint = strtok(NULL, ",");
					tmpPoint = strtok_s(NULL, ",", tmpContext);
				}
			}

			vector<double> fVecValsD;
			{
				char tmp[120];
				//strcpy(tmp, fVecVals.c_str());
				strcpy_s(tmp, fVecVals.c_str());
				char *tmpPoint;
				char **tmpContext = NULL;
				//tmpPoint = strtok(tmp, ",");
				tmpPoint = strtok_s(tmp, ",", tmpContext);
				while (tmpPoint != NULL)
				{
					fVecValsD.push_back(stod(string(tmpPoint)));
					//tmpPoint = strtok(NULL, ",");
					tmpPoint = strtok_s(NULL, ",", tmpContext);
				}
			}


			//vector<string> closeNeigh = findClosestNeighbours(allArticles, queryArticle, n, fVecType, filters);
			vector<string> closeNeigh = findClosestNeighbours(allArticles, queryArticle, n, fVecTypes, fVecValsD, filters);

			string answer = reqArgs[2] + ";";
			answer += reqArgs[1] + ";";
			for (int i = 0; i < closeNeigh.size(); i++)
			{
				answer += hashTable[closeNeigh[i]] + ';';
			}
			answer += '\n';

			HANDLE outFile = NULL;
			while (outFile == NULL || outFile == INVALID_HANDLE_VALUE)
			{
				outFile = CreateFile(
					_TEXT("D:\\tsff_back2front"),
					GENERIC_WRITE | GENERIC_READ,
					0, //FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_HIDDEN,
					NULL);

				if (outFile == NULL || outFile == INVALID_HANDLE_VALUE)
				{
					if (outFile == INVALID_HANDLE_VALUE)
					{
						cout << "INVALID_HANDLE_VALUE out Error: " << to_string(GetLastError()) << endl;
						CloseHandle(outFile);
					}
					Sleep(10);
				}

			}

			SetFilePointer(
				outFile,
				NULL,
				NULL,
				2);

			cout << "Sending Results" << endl;

			memset(outBuf, 0, BUFFER_SIZE);
			//strncpy(outBuf, answer.c_str(), BUFFER_SIZE);
			strncpy_s(outBuf, answer.c_str(), BUFFER_SIZE);
			dwBytesToWrite = (DWORD)strlen(outBuf);
			WriteFile(outFile, outBuf, dwBytesToWrite, &cbWritten, NULL);
			CloseHandle(outFile);
		}
		else if (reqType == "imgSearch" && validQuery(request)) //Old formating of requests and results.
		{
			string path = reqArgs[1];
			int    n = 12;                                                       //stoi(reqArgs[2]);
			string fVecType = "ClothingType,Silhouette,Pattern,Color,Template,"; //reqArgs[3];
			string fVecVals = "1,1,2,2,2";                                       //reqArgs[4];  // <--- fixa in dom här till featFilter
			string filters = "None";                                             //reqArgs[5];

			ClothArticle* queryArticle = new ClothArticle("Query", path, "Rod", "Top", -1);

#ifdef _FILTERS
			cv::Mat multVec;
			cv::Mat featVec = createFeatureVector(queryArticle);// , "Color");
			cv::Mat filtVec = createFilterVector(featVec.size(), "Color", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);
			queryArticle->setColor(art_color((int)colorModel->predict(multVec)));

			featVec = createFeatureVector(queryArticle);
			filtVec = createFilterVector(featVec.size(), "ClothingType", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);
			queryArticle->setClType(art_clType((int)clTypeModel->predict(multVec)));

			featVec = createFeatureVector(queryArticle);
			filtVec = createFilterVector(featVec.size(), "Silhouette", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);
			queryArticle->setClusterId((int)clustSillModel->predict(multVec));

			featVec = createFeatureVector(queryArticle);
			filtVec = createFilterVector(featVec.size(), "ClustColor", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);
			queryArticle->setClusterColor((int)clustColorModel->predict(multVec));

			featVec = createFeatureVector(queryArticle);
			filtVec = createFilterVector(featVec.size(), "ClustClType", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);
			queryArticle->setClusterClType((int)clustClTypeModel->predict(multVec));
#endif
			vector<string> fVecTypes;
			{
				char tmp[120];
				//strcpy(tmp, fVecType.c_str());
				strcpy_s(tmp, fVecType.c_str());
				char *tmpPoint;
				char **tmpContext = NULL;
				//tmpPoint = strtok(tmp, ",");
				tmpPoint = strtok_s(tmp, ",", tmpContext);
				while (tmpPoint != NULL)
				{
					fVecTypes.push_back(string(tmpPoint));
					//tmpPoint = strtok(NULL, ",");
					tmpPoint = strtok_s(NULL, ",", tmpContext);
				}
			}

			vector<double> fVecValsD;
			{
				char tmp[120];
				//strcpy(tmp, fVecVals.c_str());
				strcpy_s(tmp, fVecVals.c_str());
				char *tmpPoint;
				char **tmpContext = NULL;
				//tmpPoint = strtok(tmp, ",");
				tmpPoint = strtok_s(tmp, ",", tmpContext);
				while (tmpPoint != NULL)
				{
					fVecValsD.push_back(stod(string(tmpPoint)));
					//tmpPoint = strtok(NULL, ",");
					tmpPoint = strtok_s(NULL, ",", tmpContext);
				}
			}


			//vector<string> closeNeigh = findClosestNeighbours(allArticles, queryArticle, n, fVecType, filters);
			vector<string> closeNeigh = findClosestNeighbours(allArticles, queryArticle, n, fVecTypes, fVecValsD, filters);

			string answer = path + "\n";
			for (int i = 0; i < closeNeigh.size(); i++)
			{
				answer += hashTable[closeNeigh[i]] + '\n';
			}


			HANDLE outFile = NULL;
			while (outFile == NULL)
			{
				outFile = CreateFile(
					_TEXT("D:\\tsff_back2front"),
					GENERIC_WRITE | GENERIC_READ,
					0, //FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_HIDDEN,
					NULL);

				if (outFile == NULL)
					Sleep(10);

				if (outFile == INVALID_HANDLE_VALUE)
				{
					cout << "INVALID_HANDLE_VALUE out" << endl;
				}
			}


			cout << "Sending Results" << endl;
			//cout << answer << endl;

			memset(outBuf, 0, BUFFER_SIZE);
			//strncpy(outBuf, answer.c_str(), BUFFER_SIZE);
			strncpy_s(outBuf, answer.c_str(), BUFFER_SIZE);
			dwBytesToWrite = (DWORD)strlen(outBuf);
			WriteFile(outFile, outBuf, dwBytesToWrite, &cbWritten, NULL);
			CloseHandle(outFile);
		}
		else
		{
			string answer = "Wrong input.\n";
			cout << answer << endl;
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
	vector<ClothArticle*> *allArticles = readCatalogeFromFile(catalogePath, false);

	ClothArticle* queryArticle = new ClothArticle("Input", queryPath, "Rod", "Top", -1);

	return findClosestNeighbours(allArticles, queryArticle, n, "All", "None");
}

/**Finds the n closest neigboours to given input.
*
* \param allArticles All the articles that is going to be searched.
* \param query Article who's closest neighbours are being sought.
* \param n Number of closest neighbours being returned.
* \param testType Type of feature vector being used, e.g. "Color", "ClothingType" or "Color+ClothingType". 
* \return A vector on the n closest neighbours.
*/
vector<string> findClosestNeighbours(vector<ClothArticle*> *allArticles, ClothArticle* query, int n, string fVecType, string filterType)
{
	vector<string> tmp;
	tmp.push_back(fVecType);
	vector<double> tmp2;
	tmp2.push_back(1);
	tmp2.push_back(1);
	tmp2.push_back(1);
	tmp2.push_back(1);
	tmp2.push_back(1);
	return findClosestNeighbours(allArticles, query, n, tmp, tmp2, filterType);
}

vector<string> findClosestNeighbours(vector<ClothArticle*> *allArticles, ClothArticle* query, int n, vector<string> fVecType, vector<double> fVecVal, string filterType)
{
	struct GreatPair
	{
		bool operator()(const pair<double, string>& lhs, const pair<double, string>& rhs) const
		{
			return get<0>(lhs) > get<0>(rhs);
			//return get<0>(lhs) < get<0>(rhs);
		}
	};

	priority_queue< pair<double, string>, vector<pair<double, string>>, GreatPair > priQueue;

	cv::Mat queryFeat = createFeatureVector(query, fVecVal);
	cv::Mat filtVec = createFilterVector(queryFeat.size(), fVecType, 1.0f, 0.0f);

	for (int i = 0; i < allArticles->size(); i++)
	{
		ClothArticle* curr = allArticles->at(i);

		

		bool statement = (filterType == "None")
			|| (filterType == "ClothingType" && curr->getClType() == query->getClType())
			|| (filterType == "Color" && curr->getColor() == query->getColor())
			|| (filterType == "Silhouette" && curr->getClusterId() == query->getClusterId())
			|| (filterType == "ClustColor" && curr->getClusterColor() == query->getClusterColor())
			|| (filterType == "ClustClType" && curr->getClusterClType() == query ->getClusterClType())
			|| (filterType == "All" && curr->getClType() == query->getClType() && curr->getColor() == query->getColor());
		if (statement)
		{
			cv::Mat currFeat = createFeatureVector(curr, fVecVal); // , fVecType);
			
			//cout << curr->getClusterId() << " : " << curr->getClusterColor() << " : " << curr->getClusterClType() << endl;
			
			// <--- funkar inte här under pga att vect är 1-dimensionell, behöver vara 2-dim
			cv::Mat queryEdge(Config::get().EDGE_IMAGE_SIZE_XY, Config::get().EDGE_IMAGE_SIZE_XY, CV_8U);
			cv::Mat currEdge(Config::get().EDGE_IMAGE_SIZE_XY, Config::get().EDGE_IMAGE_SIZE_XY, CV_8U);

			cv::Mat queryEdgeVec = query->getImgFeats()->getEdgeVect()[1];
			cv::Mat currEdgeVec = curr->getImgFeats()->getEdgeVect()[1];
			for (int y = 0; y < Config::get().EDGE_IMAGE_SIZE_XY; y++)
			{
				for (int x = 0; x < Config::get().EDGE_IMAGE_SIZE_XY; x++)
				{
					queryEdge.at<uchar>(cv::Point(x, y)) = queryEdgeVec.at<uchar>(y * Config::get().EDGE_IMAGE_SIZE_XY + x);
					currEdge.at<uchar>(cv::Point(x, y)) = currEdgeVec.at<uchar>(y * Config::get().EDGE_IMAGE_SIZE_XY + x);
				}

			}


			pair<double, double> tmpltValue = make_pair(0, 0);
			for (int i = 0; i < fVecType.size(); i++)
			{
				if(fVecType[i] == "Template")
					tmpltValue = performTemplateMatching(currEdge, queryEdge);
			}
			
			double dist = calcEuclDist(queryFeat, currFeat, filtVec);
			double prioValue = dist + (1 / (abs(tmpltValue.first) + 1)) * fVecVal[4];
			
			pair<float, string> currPriEntry;
			//currPriEntry = make_pair(dist, curr->getId());
			currPriEntry = make_pair(prioValue, curr->getId());

			priQueue.push(currPriEntry);
		}

	}

	vector<string> topResults;

#ifndef _DEBUG
	cout << to_string(query->getClType()) << endl << to_string(query->getColor()) << endl;
	if(false)
	{
		for (int i = 0; i < n; i++)
		{
			topResults.push_back(get<1>(priQueue.top()));
			cout << get<0>(priQueue.top()) << endl;

			for (int k = 0; k < allArticles->size(); k++)
			{
				if (allArticles->at(k)->getId() == get<1>(priQueue.top()))
				{
					cv::namedWindow(to_string(i), 1);
					cv::imshow(to_string(i), allArticles->at(k)->getImage());
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

/**Creates a vector mask that can be used to filter a feature vector.
*
* \param vecSize The size of the mask.
* \param filtType The type of masks.
* \param posScale The positive scaling of the mask.
* \param negScale The negative scaling of the mask.
* \return A vector mask.
*/
cv::Mat createFilterVector(cv::Size vecSize, vector<string> filtType, float posScale, float negScale)
{
	cv::Mat filtVec(vecSize, CV_32FC1, cv::Scalar(0.0f));

	while (!filtType.empty())
	{
		string type = filtType.back();
		filtType.pop_back();

		int max, min;

		if (type != "Template")
		{
			if (type == "ClothingType")
			{
				min = 0;
				max = EDGE_FEATURE_SIZE;
			}
			else if (type == "Silhouette")
			{
				min = EDGE_FEATURE_SIZE;
				max = 2 * EDGE_FEATURE_SIZE;
			}
			else if (type == "Pattern")
			{
				min = 2 * EDGE_FEATURE_SIZE;
				max = 2 * EDGE_FEATURE_SIZE + Config::get().NUM_OF_GRAD_ANGS;
			}
			else if (type == "Color")
			{
				min = 2 * EDGE_FEATURE_SIZE + Config::get().NUM_OF_GRAD_ANGS;
				max = filtVec.cols;
			}

			for (int i = 0; i < filtVec.cols; i++)
			{
				if (min <= i  && i < max)
					filtVec.at<float>(0, i) = posScale;
			}
		}
	}
	return filtVec;
}

/**Creates a vector mask that can be used to filter a feature vector.
*
* \param vecSize The size of the mask.
* \param filtType The type of mask.
* \param posScale The positive scaling of the mask.
* \param negScale The negative scaling of the mask.
* \return A vector mask.
*/
cv::Mat createFilterVector(cv::Size vecSize, string filtType, float posScale, float negScale)
{
	vector<string> tmp;
	if (filtType != "All")
	{
		tmp.push_back("ClothingType");
		tmp.push_back("Silhouette");
		tmp.push_back("Pattern");
		tmp.push_back("Color");
	}
	else
	{
		tmp.push_back(filtType);
	}
	
	return createFilterVector(vecSize, tmp, posScale, negScale);
}

/**Creates a feature vector.
*
* \param input Article who's feature vector is going to be extracted.
* \return Feature vector of the given article.
*/
cv::Mat createFeatureVector(ClothArticle* input)
{
	vector<double> fVecVal;
	fVecVal.push_back(1);
	fVecVal.push_back(1);
	fVecVal.push_back(1);
	fVecVal.push_back(1);
	return createFeatureVector(input, fVecVal);
}

/**Creates a feature vector.
*
* \param input Article who's feature vector is going to be extracted.
* \param fVecVal Vector of scalars for the different features.
* \return Feature vector of the given article.
*/
cv::Mat createFeatureVector(ClothArticle* input, vector<double> fVecVal) //, string fVecType)
{
	cv::Mat fVec;
	ImageFeatures *inpFeats = input->getImgFeats();

	fVec = cv::Mat(1, 2 * EDGE_FEATURE_SIZE + Config::get().NUM_OF_GRAD_ANGS + 6 * 32, CV_32F);

	cv::Mat tmp = inpFeats->getEdgeVect(0);

	for (int j = 0; j < tmp.rows; j++)
	{
		fVec.at<float>(0, j) = tmp.at<float>(j, 0) * fVecVal[0];
	}

	tmp = inpFeats->getEdgeVect(1);

	for (int j = 0; j < tmp.rows; j++)
	{
		fVec.at<float>(0, j + EDGE_FEATURE_SIZE) = tmp.at<float>(j, 0) * fVecVal[1];
	}

	tmp = inpFeats->getEdgeVect(2);

	for (int j = 0; j < tmp.rows; j++)
	{
		fVec.at<float>(0, j + 2 * EDGE_FEATURE_SIZE) = tmp.at<float>(j, 0) * fVecVal[2];
	}

	for (int j = 0; j < 6; j++)
	{
		if (j<3)
		{
			tmp = inpFeats->getRGBHist(j);
		}
		else
		{
			tmp = inpFeats->getHSVHist(j - 3);
		}

		for (int k = 0; k < 32; k++)
		{
			fVec.at<float>(0, tmp.rows * j + k + 2 * EDGE_FEATURE_SIZE + Config::get().NUM_OF_GRAD_ANGS) = (float)tmp.at<float>(k, 0) * fVecVal[3];
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
cv::Ptr<cv::ml::SVM> makeSVMModel(vector<ClothArticle*> *input, string testType)
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
cv::Ptr<cv::ml::RTrees> makeRTModel(vector<ClothArticle*> *input, string testType)
{
	cv::Ptr<cv::ml::TrainData> tData = createTrainingData(input, testType);

	cv::Ptr<cv::ml::RTrees> rt = cv::ml::RTrees::create();

	rt->setMaxDepth(20);
	rt->setMinSampleCount(20);
	rt->setMaxCategories(40);

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
cv::Ptr<cv::ml::TrainData> createTrainingData(vector<ClothArticle*> *input, string classifierGroup)
{
	vector<int> labels; //color{ + clothType + sleeveType

	for (int i = 0; i < input->size(); i++)
	{
		if (classifierGroup == "Color")
			labels.push_back(input->at(i)->getColor());
		else if (classifierGroup == "ClothingType")
			labels.push_back(input->at(i)->getClType());
		else if (classifierGroup == "SleeveType")
			labels.push_back(input->at(i)->getSleeveType());
		else if (classifierGroup == "Silhouette")
			labels.push_back(input->at(i)->getClusterId());
		else if (classifierGroup == "ClustColor")
			labels.push_back(input->at(i)->getClusterColor());
		else if (classifierGroup == "ClustClType")
			labels.push_back(input->at(i)->getClusterClType());
	}

	cv::Mat labelsMat(labels.size(), 1, CV_32SC1);
	for (int i = 0; i < labels.size(); i++)
	{
		labelsMat.at<int>(i, 0) = labels[i];
	}

	int dataMatFeature = 2 * EDGE_FEATURE_SIZE + Config::get().NUM_OF_GRAD_ANGS + 6 * 32;

	cv::Mat tmpVec = createFeatureVector(input->at(0));
	cv::Mat filtVec = createFilterVector(tmpVec.size(), classifierGroup, 1.0f, 0.0f);
	cout << cv::countNonZero(filtVec) << endl;
	cv::Mat trainingDataMat(input->size(), dataMatFeature, CV_32FC1);
	for (int i = 0; i < labels.size(); i++)
	{
		cv::Mat multVec;
		cv::Mat featVec = createFeatureVector(input->at(i));
		cv::multiply(featVec, filtVec, multVec);
		if (i < 1)
		{
			;
			//cout << multVec << endl;
			//cout << cv::sum(multVec) << endl;
		}
		

		for (int k = 0; k < multVec.cols; k++)
		{
			trainingDataMat.at<float>(i, k) = multVec.at<float>(0, k);
		}
	}

	cv::Ptr<cv::ml::TrainData> tData = cv::ml::TrainData::create(trainingDataMat, cv::ml::SampleTypes::ROW_SAMPLE, labelsMat); // <-- Den här får fel data vid ClothingType

	return tData;
}

/**Clusteres the entities in a cataloge.
*
* \param cataloge Cataloge with entities that are going to be clustered.
* \param filtType Name of the filter that chooses what kind of features will be used. ("Silhouette", "ClustColor" or "ClustClType")
*/
void clusterCataloge(vector<ClothArticle*> *cataloge, string filtType)
{

	int clusterCount;
	if (filtType == "Silhouette")
		clusterCount = MIN(Config::get().MAXIMUM_CLUSTER_COUNT, cataloge->size());
	else if (filtType == "ClustColor")
		clusterCount = MIN(12, cataloge->size());
	else if (filtType == "ClustClType")
		clusterCount = MIN(8, cataloge->size());
		

	cv::Mat points(cataloge->size(), 2 * EDGE_FEATURE_SIZE + Config::get().NUM_OF_GRAD_ANGS + 6 * 32, CV_32F);
	cv::Mat labels;

	for (int i = 0; i < cataloge->size(); i++)
	{
		cv::Mat tmp1 = createFeatureVector(cataloge->at(i));
		cv::Mat tmp2 = createFilterVector(tmp1.size(), filtType, 1.0f, 0.0f);
		cv::Mat tmp3;
		cv::multiply(tmp1, tmp2, tmp3);
		tmp3.copyTo(points(cv::Rect(0, i, tmp3.cols, tmp3.rows)));
	}
	cout << points.size() << endl;

	cv::Mat centers;

	kmeans(points, clusterCount, labels,
		cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0),
		3, cv::KMEANS_PP_CENTERS, centers);

	int *list = (int*) calloc(clusterCount, sizeof(int));
	for (int i = 0; i < cataloge->size(); i++)
	{
		if(filtType == "Silhouette")
			cataloge->at(i)->setClusterId(labels.at<int>(i));
		else if(filtType == "ClustColor")
			cataloge->at(i)->setClusterColor(labels.at<int>(i));
		else if(filtType == "ClustClType")
			cataloge->at(i)->setClusterClType(labels.at<int>(i));
		list[labels.at<int>(i)]++;
	}

	cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl
		 << "% Size of clusters " << endl;
	for (int i = 0; i < clusterCount; i++)
	{
		cout << "% " << i + 1 << ": " << list[i] << endl;
	}
	cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	free(list);
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
		scale = cv::Mat(mat1.size(), CV_32FC1, cv::Scalar::all(0));
	}

	float dist = 0;
	for (int i = 0; i < mat1.rows; i++)
	{
		for (int j = 0; j < mat1.cols; j++)
		{
			if(scale.at<float>(i,j) != 0)
				dist += powf(scale.at<float>(i,j) * ((mat1.at<float>(i, j) - mat2.at<float>(i, j))), 2);
		}
	}
	return sqrt(dist);
}