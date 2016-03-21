#include "ImageSearcher.h"

HANDLE reciSlot;
HANDLE sendSlot;
unordered_map<string, string> hashTable;

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

inline bool validPath(const string& path) {
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
		 << "$$   SAVE   Saves cataloge for faster starts.            $$" << endl
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
	vector<ClothArticle*> *tmp = readCatalogeFromFile(catalogePath, true);
	hashTable = makeIdToPathTable(*tmp);

	if (!MakeReciSlot(BACK_TO_FRONT_SLOT, &reciSlot))
		return;


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
		else if(inp == "2")
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
						query += "live0.jpg\n";
						valid = true;
					}
					else if (validPath(Config::get().TEST_FOLDER + inputPath))
					{
						inputPath = Config::get().TEST_FOLDER + inputPath;
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
					cout << "Input number of hits (maximum size = " + to_string(Config::get().MAXIMUM_SEARCH_HITS) + "): ";
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
						if (n > Config::get().MAXIMUM_SEARCH_HITS)
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
						cout << "Invalid input, input must be an integer smaller or equal to " + to_string(Config::get().MAXIMUM_SEARCH_HITS) << endl;
					}
				} while (!valid);

				valid = false;
				do
				{
					cout << "Choose feat vectors (0 - All, 1 - Only Color, 2 - Only Clothing Type, 3 - Only Pattern): ";
					string fVec;
					cin >> fVec;
					try 
					{
						if (fVec == "0")
						{
							query += "All\n";
							valid = true;
						}
						else if (fVec == "1")
						{
							query += "Color\n";
							valid = true;
						}
						else if (fVec == "2")
						{
							query += "ClothingType\n";
							valid = true;
						}
						else if (fVec == "3")
						{
							query += "Pattern\n";
							valid = true;
						}
						else
						{
							throw 2;
						}
						int b = stoi(fVec);
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


				valid = false;
				do
				{
					cout << "Choose filters (0 - None, 1 - Same Color, 2 - Same Clothing Type, 3 - Clusterer , 4 - All): ";
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
							query += "Clusterer\n";
							valid = true;
						}
						else if (filter == "4")
						{
							query += "All\n";
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
				cv::Mat tmpImg3 = resizeImg(tmpImg, 300, 300);
				cv::Mat tmpImg2 = resizeImg(tmpImg, 300, 300);

				cv::cvtColor(tmpImg2, tmpImg2, CV_BGR2GRAY);

				cv::threshold(tmpImg2, tmpImg2, 248, 255, CV_THRESH_BINARY_INV);
				//cv::bitwise_not(tmpImg2, tmpImg2);
				tmpImg2 = tmpImg2 * 255;

				cv::Mat out;
				onlyBackground(tmpImg2, out);
				out *= 255;

				cv::Mat edge = preformCanny(out, 80, 140);
				cv::Mat edge2 = preformCanny(tmpImg2, 80, 140);

				cv::namedWindow("Query EDGE", 1);
				cv::imshow("Query EDGE", edge);

				cv::namedWindow("Query EDGE2", 1);
				cv::imshow("Query EDGE2", edge2);

				cv::namedWindow("Query", 1);
				cv::imshow("Query", tmpImg3);
				for (int i = 0; i < results.size(); i++)
				{
					cout << results[i] << " : " << hashTable[results[i]] << endl;
					tmpImg = cv::imread(hashTable[results[i]], cv::IMREAD_UNCHANGED);
					cout << hashTable[results[i]] << endl;
					if (hashTable[results[i]].find(".png") != string::npos)
						filterAlphaArtifacts(&tmpImg);
					tmpImg2 = resizeImg(tmpImg, 300, 300);

					cv::Mat tmpImg3;
					cv::cvtColor(tmpImg2, tmpImg3, CV_BGR2GRAY);
					cv::threshold(tmpImg3, tmpImg3, 248, 255, CV_THRESH_BINARY_INV);
					cv::Mat tmpImg4;
					onlyBackground(tmpImg3, tmpImg4);
					tmpImg4 *= 255;
					tmpImg4 = preformGaussianBlur(tmpImg4);
					tmpImg4 = preformCanny(tmpImg4, 80, 140);

					
					

					cv::namedWindow("Result # " + to_string(i + 1), 1);
					cv::imshow("Result # " + to_string(i + 1), tmpImg2);
				}
				cv::waitKey(0);
				cv::destroyAllWindows();
			}
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

				CreateProcess(TEXT("./The_Search_For_Fashion.exe"),   // the path
					TEXT("The_Search_For_Fashion -b --embeded"),       // Command line
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
		else if (inp == "SAVE")
		{
			if (isOnline(FRONT_TO_BACK_SLOT, &sendSlot))
			{
				if (MakeSendSlot(FRONT_TO_BACK_SLOT, &sendSlot))
				{
					writeSlot(sendSlot, TEXT("SAVE\n"));
					CloseHandle(sendSlot);
				}
				message = "Saved cataloge.";
			}
			else
				message = "Backend is offline.";
		}
	}

}

enum {
	IDCL_LISTBOX = 200,
	IDBC_DEFPUSHBUTTON,
	IDBC_PUSHBUTTON,
	IDBC_AUTOCHECKBOX,
	IDBC_AUTORADIOBUTTON,
	IDBC_GROUPBOX,
	IDBC_ICON,
	IDBC_BITMAP
};

typedef std::basic_string<TCHAR> ustring;

LPCWSTR g_szClassName = TEXT("myWindowClass");

enum
{
	featAll = 100,
	featColor,
	featClType,
	featPattern,
	filtNone,
	filtColor,
	filtClType,
	filtCluster,
	filtAll,
	searchButton,
	onlineButton,
	res0, res1, res2,
	res3, res4, res5,
	res6, res7, res8,
	res9, res10, res11
};

HWND makeFeatButtons(const HWND hwnd, CREATESTRUCT *cs, int x, int y)
{
	HWND outGroup = CreateWindowEx(WS_EX_WINDOWEDGE,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("Feature"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_GROUPBOX,             //control style 
		x,												    //position: left
		y,													  //position: top
		320,												  //width
		40,													  //height
		hwnd,											       //parent window handle							  
		(HMENU)2,													//control's ID
		cs->hInstance,									      //application instance
		0);

	HWND but0 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("All"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,             //control style 
		10,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)featAll,													//control's ID
		cs->hInstance,									      //application instance
		0);

	HWND but1 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("Color"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,             //control style 
		85,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)featColor,													//control's ID
		cs->hInstance,									      //application instance
		0);

	HWND but2 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("ClType"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,             //control style 
		160,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)featClType,													//control's ID
		cs->hInstance,									      //application instance
		0);

	HWND but3 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("Pattern"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,             //control style 
		235,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)featPattern,													//control's ID
		cs->hInstance,									      //application instance
		0);
	return outGroup;
}

HWND makeFiltButtons(const HWND hwnd, CREATESTRUCT *cs, int x, int y)
{
	HWND outGroup = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("Filter"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_GROUPBOX,             //control style 
		x,												    //position: left
		y,													  //position: top
		395,												  //width
		40,													  //height
		hwnd,											       //parent window handle							  
		(HMENU)2,													//control's ID
		cs->hInstance,									      //application instance
		0);

	HWND but0 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("None"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,             //control style 
		10,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)filtNone,													//control's ID
		cs->hInstance,									      //application instance
		0);

	HWND but1 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("Color"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,             //control style 
		85,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)filtColor,										//control's ID
		cs->hInstance,									      //application instance
		0);

	HWND but2 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("ClType"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,             //control style 
		160,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)filtClType, 									//control's ID
		cs->hInstance,									      //application instance
		0);

	HWND but3 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("Cluster"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,             //control style 
		235,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)filtCluster,											//control's ID
		cs->hInstance,									      //application instance
		0);

	HWND but4 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("All"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,             //control style 
		310,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)filtAll,													//control's ID
		cs->hInstance,									      //application instance
		0);

	return outGroup;
}

int OnCreate(const HWND hwnd, CREATESTRUCT *cs)
{
	HWND PathStatic = CreateWindowEx(0,
		WC_STATIC,
		TEXT("Path to query:"),
		WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP,
		10,
		10,
		95,
		20,
		hwnd,
		NULL,
		NULL,
		NULL);

	HWND PathBox = CreateWindowEx(0,
		WC_COMBOBOX,
		TEXT(""),
		WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP,
		10,
		35,
		300,
		20,
		hwnd,
		NULL,
		NULL,
		NULL);

	SendMessage(PathBox, CB_SETCUEBANNER, (WPARAM)0, (LPARAM)TEXT("e.g. dress0.jpg"));

	HWND NumResStatic = CreateWindowEx(0,
		WC_STATIC,
		TEXT("Number of results (max 12):"),
		WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP,
		10,
		65,
		185,
		20,
		hwnd,
		NULL,
		NULL,
		NULL);

	HWND NumResText = CreateWindowEx(0,
		WC_COMBOBOX,
		TEXT(""),
		WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP,
		10,
		90,
		300,
		20,
		hwnd,
		NULL,
		NULL,
		NULL);

	SendMessage(NumResText, CB_SETCUEBANNER, (WPARAM)0, (LPARAM)TEXT("e.g. 12"));

	HWND featBox = makeFeatButtons(hwnd, cs, 10, 125);

	HWND filtBox = makeFiltButtons(hwnd, cs, 10, 175);

	HWND hWnd3 = CreateWindowEx(0,          //extended styles
		_T("button"),						//control 'class' name
		_T("Search"),						//control caption
		WS_VISIBLE | WS_CHILD,				//control style 
		10,							        //position: left
		220,							    //position: top
		50,									//width
		30,									//height
		hwnd,								//parent window handle							 
		(HMENU)searchButton,				//control's ID
		cs->hInstance,                      //application instance
		0);

	HWND msgBox = CreateWindowEx(0,         //extended styles
		_T("static"),						//control 'class' name
		_T(""),						    //control caption
		WS_VISIBLE | WS_CHILD,				//control style 
		70,							        //position: left
		225,							    //position: top
		300,								//width
		20,									//height
		hwnd,								//parent window handle							 
		0,							        //control's ID
		cs->hInstance,                      //application instance
		0);

	HWND hWnd4 = CreateWindowEx(0,          //extended styles
		_T("button"),						//control 'class' name
		_T("Online"),						//control caption
		WS_VISIBLE | WS_CHILD,				//control style 
		10,							        //position: left
		260,							    //position: top
		50,									//width
		30,									//height
		hwnd,								//parent window handle							 
		(HMENU)onlineButton,				//control's ID
		cs->hInstance,                      //application instance
		0);

	return 0;
}

int checkRadioGroup(HWND groupHWND, int first, int last)
{
	for (int i = first; i <= last; i++)
	{
		if (IsDlgButtonChecked(groupHWND, i))
			return i;
	}
	return 0;
}

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
		else if (count > 4)
			return false;

		count++;
	}
	if (count < 5)
		return false;

	return true;
}

string createQuery(LPTSTR path, LPTSTR numRes, int featNum, int filtNum)
{
	string out = "imgSearch\n";

	char tmp[32] = { '\0' };
	cout << (string)tmp << endl;
	int i;
	for (i = 0; path[i] != '\0'; i++)
	{
		tmp[i] = path[i];
	}
	tmp[i] = '\0';
	out += Config::get().TEST_FOLDER + string(tmp) + "\n";

	for (i = 0; numRes[i] != '\0'; i++)
	{
		tmp[i] = numRes[i];
	}
	tmp[i] = '\0';
	out += string(tmp) + "\n";

	switch (featNum)
	{
	case featAll:
		out += "All\n";
		break;
	case featColor:
		out += "Color\n";
		break;
	case featClType:
		out += "clType\n";
		break;
	case featPattern:
		out += "Pattern\n";
		break;
	}

	switch (filtNum)
	{
	case filtNone:
		out += "None\n";
		break;
	case filtColor:
		out += "Color\n";
		break;
	case filtClType:
		out += "ClothingType\n";
		break;
	case filtCluster:
		out += "Clusterer\n";
		break;
	case filtAll:
		out += "All\n";
		break;
	}

	return out;
}

bool sendQuery(vector<string> &results, string query)
{
	std::wstring stemp = s2ws(query);
	LPCWSTR lquery = stemp.c_str();
	writeSlot(sendSlot, lquery);
	CloseHandle(sendSlot);

	while (results.empty())
	{
		string request = readSlot(reciSlot);

		if (request == "FALSE")
			return false;

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
	return true;
}

// Step 4: the Window Procedure
inline LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	HWND tmp;
	LPTSTR path = (LPTSTR)new char[32];
	LPTSTR numRes = (LPTSTR)new char[32];
	int featNum = 0;
	int filtNum = 0;
	string query;

	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		case searchButton:
			if (!isOnline(FRONT_TO_BACK_SLOT, &sendSlot))
			{
				tmp = FindWindowEx(hwnd, NULL, _T("static"), NULL);
				tmp = FindWindowEx(hwnd, tmp, _T("static"), NULL);
				tmp = FindWindowEx(hwnd, tmp, _T("static"), NULL);
				SendMessage(tmp, WM_SETTEXT, (WPARAM)0, (LPARAM)TEXT("Couldn't send request, backend is offline."));
			}
			else if (MakeSendSlot(FRONT_TO_BACK_SLOT, &sendSlot))
			{
				tmp = FindWindowEx(hwnd, NULL, WC_COMBOBOX, NULL);
				GetWindowText(tmp, path, 32);

				tmp = FindWindowEx(hwnd, tmp, WC_COMBOBOX, NULL);
				GetWindowText(tmp, numRes, 32);

				tmp = FindWindowEx(hwnd, NULL, _T("button"), _T("Feature"));
				featNum = checkRadioGroup(tmp, featAll, featPattern);

				tmp = FindWindowEx(hwnd, NULL, _T("button"), _T("Filter"));
				filtNum = checkRadioGroup(tmp, filtNone, filtAll);


				query = createQuery(path, numRes, featNum, filtNum);

				tmp = FindWindowEx(hwnd, NULL, _T("static"), NULL);
				tmp = FindWindowEx(hwnd, tmp, _T("static"), NULL);
				tmp = FindWindowEx(hwnd, tmp, _T("static"), NULL);
				if (validQuery(query))
				{
					SendMessage(tmp, WM_SETTEXT, (WPARAM)0, (LPARAM)TEXT("Initiating search."));

					vector<string> results;
					cout << sendQuery(results, query) << endl;

					SendMessage(tmp, WM_SETTEXT, (WPARAM)0, (LPARAM)TEXT("Showcasing results."));

					string sTmp = query.substr(query.find('\n') + 1, query.length() - query.find('\n'));
					string inputPath = sTmp.substr(0, sTmp.find('\n'));
					cv::Mat tmpImg = cv::imread(inputPath, cv::IMREAD_UNCHANGED);
					cv::Mat tmpImg3 = resizeImg(tmpImg, 300, 300);
					cv::Mat tmpImg2 = resizeImg(tmpImg, 300, 300);

					cv::cvtColor(tmpImg2, tmpImg2, CV_BGR2GRAY);

					cv::threshold(tmpImg2, tmpImg2, 248, 255, CV_THRESH_BINARY_INV);
					//cv::bitwise_not(tmpImg2, tmpImg2);
					tmpImg2 = tmpImg2 * 255;

					cv::Mat out;
					onlyBackground(tmpImg2, out);
					out *= 255;

					cv::Mat edge = preformCanny(out, 80, 140);
					cv::Mat edge2 = preformCanny(tmpImg2, 80, 140);

					cv::namedWindow("Query EDGE", 1);
					cv::imshow("Query EDGE", edge);

					cv::namedWindow("Query EDGE2", 1);
					cv::imshow("Query EDGE2", edge2);

					cv::namedWindow("Query", 1);
					cv::imshow("Query", tmpImg3);
					for (int i = 0; i < results.size(); i++)
					{
						cout << results[i] << " : " << hashTable[results[i]] << endl;
						tmpImg = cv::imread(hashTable[results[i]], cv::IMREAD_UNCHANGED);
						cout << hashTable[results[i]] << endl;
						if (hashTable[results[i]].find(".png") != string::npos)
							filterAlphaArtifacts(&tmpImg);
						tmpImg2 = resizeImg(tmpImg, 300, 300);

						cv::Mat tmpImg3;
						cv::cvtColor(tmpImg2, tmpImg3, CV_BGR2GRAY);
						cv::threshold(tmpImg3, tmpImg3, 248, 255, CV_THRESH_BINARY_INV);
						cv::Mat tmpImg4;
						onlyBackground(tmpImg3, tmpImg4);
						tmpImg4 *= 255;
						tmpImg4 = preformGaussianBlur(tmpImg4);
						tmpImg4 = preformCanny(tmpImg4, 80, 140);

						cv::namedWindow("Result # " + to_string(i + 1), 1);
						cv::imshow("Result # " + to_string(i + 1), tmpImg2);
					}
					cv::waitKey(0);
					cv::destroyAllWindows();

					SendMessage(tmp, WM_SETTEXT, (WPARAM)0, (LPARAM)TEXT(""));
				}
				else
				{
					SendMessage(tmp, WM_SETTEXT, (WPARAM)0, (LPARAM)TEXT("Wrong input, try again."));
				}
			}
			break;
		case onlineButton:
			tmp = FindWindowEx(hwnd, NULL, _T("static"), NULL);
			tmp = FindWindowEx(hwnd, tmp, _T("static"), NULL);
			tmp = FindWindowEx(hwnd, tmp, _T("static"), NULL);
			if (isOnline(FRONT_TO_BACK_SLOT, &sendSlot))
				SendMessage(tmp, WM_SETTEXT, (WPARAM)0, (LPARAM)TEXT("Backend is online."));
			else
				SendMessage(tmp, WM_SETTEXT, (WPARAM)0, (LPARAM)TEXT("Backend is offline."));
			break;
		}
		break;
	case WM_CREATE:
		return OnCreate(hwnd, reinterpret_cast<CREATESTRUCT*>(lParam));
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int guiFrontend(string catalogePath)
{
	
	vector<ClothArticle*> *tmp = readCatalogeFromFile(catalogePath, true);
	hashTable = makeIdToPathTable(*tmp);

	if (!MakeReciSlot(BACK_TO_FRONT_SLOT, &reciSlot))
		return -1;


	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	//Step 1: Registering the Window Class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = NULL;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error!"),
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Step 2: Creating the Window
	hwnd = CreateWindowEx(
		0,
		g_szClassName,
		TEXT("The title of my window"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		400,
		350,
		NULL,
		NULL,
		NULL,
		NULL);



	if (hwnd == NULL)
	{
		MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error!"),
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}


	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	// Step 3: The Message Loop
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}


int backend(string catalogePath, bool embeded, bool loadModel)
{
	
	vector<ClothArticle*> *allArticles;
	cv::Ptr<cv::ml::RTrees> colorModel;
	cv::Ptr<cv::ml::RTrees> clTypeModel;
	cv::Ptr<cv::ml::RTrees> clusterModel;
	if (validPath(catalogePath + Config::get().SAVE_EXTENTION) && validPath(catalogePath + Config::get().MODEL_COLOR_EXTENTION) && validPath(catalogePath + Config::get().MODEL_CLTYPE_EXTENTION))
	{
		cout << Config::get().SAVE_EXTENTION + "Badaboj" << endl;
		cout << Config::get().MODEL_COLOR_EXTENTION << endl;
		cout << Config::get().MODEL_CLTYPE_EXTENTION << endl;
		allArticles = loadCataloge(catalogePath + Config::get().SAVE_EXTENTION);
		colorModel = cv::Algorithm::load<cv::ml::RTrees>(catalogePath + Config::get().MODEL_COLOR_EXTENTION);
		clTypeModel = cv::Algorithm::load<cv::ml::RTrees>(catalogePath + Config::get().MODEL_CLTYPE_EXTENTION);
	}
	else
	{
		allArticles = readCatalogeFromFile(catalogePath, false);
		clusterCataloge(allArticles, "Clusterer");
		colorModel = makeRTModel(allArticles, "Color");
		clTypeModel = makeRTModel(allArticles, "ClothingType");
		clusterModel = makeRTModel(allArticles, "Clusterer");
		colorModel->save(catalogePath + Config::get().MODEL_COLOR_EXTENTION);
		clTypeModel->save(catalogePath + Config::get().MODEL_CLTYPE_EXTENTION);
	}

	/*
	vector<cv::Mat> hist = allArticles->at(0)->getImgFeats()->getEdgeHists();
	cout << hist.size() << endl;
	cout << all2->at(0)->getImgFeats()->getEdgeHists()[0].at<float>(0,0) << endl;
	cout << allArticles->at(0)->getImgFeats()->getEdgeHists()[0].at<float>(0, 0) << endl;

	cout << all2->size() << endl;
	cout << allArticles->size() << endl;
	*/


	if (!MakeReciSlot(FRONT_TO_BACK_SLOT, &reciSlot))
		return 1;
	
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
			string path     = reqArgs[1];
			int    n        = stoi(reqArgs[2]);
			string fVecType = reqArgs[3];
			string filters  = reqArgs[4];


			ClothArticle* queryArticle = new ClothArticle("Query", path, "Rod", "Top", -1);

			cv::Mat multVec;
			cv::Mat featVec = createFeatureVector(queryArticle);// , "Color");
			cv::Mat filtVec = createFilterVector(featVec.size(), "Color", 1.0f, 0.0f);
			cout << filtVec << endl;
			cv::multiply(featVec, filtVec, multVec);
			queryArticle->setColor(art_color((int)colorModel->predict(multVec)));

			featVec = createFeatureVector(queryArticle);
			filtVec = createFilterVector(featVec.size(), "ClothingType", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);
			queryArticle->setClType(art_clType((int)clTypeModel->predict(multVec)));

			featVec = createFeatureVector(queryArticle);
			filtVec = createFilterVector(featVec.size(), "Clusterer", 1.0f, 0.0f);
			cv::multiply(featVec, filtVec, multVec);
			queryArticle->setClusterId((int)clusterModel->predict(multVec));


			vector<string> closeNeigh = findClosestNeighbours(allArticles, queryArticle, n, fVecType, filters);

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
		else if (reqType == "SAVE")
		{
			saveCataloge(allArticles, catalogePath + ".sv");
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
	vector<ClothArticle*> *allArticles = readCatalogeFromFile(catalogePath, false);

	ClothArticle* queryArticle = new ClothArticle("Input", queryPath, "Rod", "Top", -1);

	return findClosestNeighbours(allArticles, queryArticle, n, "All", "None");
}


vector<string> oldseekUsingImage(string catalogePath, string queryPath, int n)
{
	vector<ClothArticle*> *allArticles = readCatalogeFromFile(catalogePath, false);

	ClothArticle* queryArticle = new ClothArticle("Input", queryPath, "Rod", "Top", -1);

	return oldfindClosestNeighbours(allArticles, queryArticle, n, "None");
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
	struct GreatTuple
	{
		bool operator()(const tuple<float, string>& lhs, const tuple<float, string>& rhs) const
		{
			return get<0>(lhs) > get<0>(rhs);
		}
	};

	priority_queue< tuple<float, string>, vector<tuple<float, string>>, GreatTuple > priQueue;

	cv::Mat queryFeat = createFeatureVector(query);
	cv::Mat filtVec = createFilterVector(queryFeat.size(), fVecType, 1.0f, 0.0f);

	for (int i = 0; i < allArticles->size(); i++)
	{
		ClothArticle* curr = allArticles->at(i);

		

		bool statement = (filterType == "None")
			|| (filterType == "ClothingType" && curr->getClType() == query->getClType())
			|| (filterType == "Color" && curr->getColor() == query->getColor())
			|| (filterType == "Clusterer" && curr->getClusterId() == query->getClusterId())
			|| (filterType == "All" && curr->getClType() == query->getClType() && curr->getColor() == query->getColor());
		if (statement)
		{
			cv::Mat currFeat = createFeatureVector(curr); // , fVecType);
			
			cout << to_string(curr->getClType()) << endl;
			float dist = calcEuclDist(queryFeat, currFeat, filtVec);

			tuple<float, string> currPriEntry;
			currPriEntry = make_tuple(dist, curr->getId());

			priQueue.push(currPriEntry);
		}

		/*
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
		*/
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


vector<string> oldfindClosestNeighbours(vector<ClothArticle*> *allArticles, ClothArticle* query, int n, string testType)
{
	struct GreatTuple
	{
		bool operator()(const tuple<float, string>& lhs, const tuple<float, string>& rhs) const
		{
			return get<0>(lhs) > get<0>(rhs);
		}
	};

	priority_queue< tuple<float, string>, vector<tuple<float, string>>, GreatTuple > priQueue;

	cv::Mat queryFeat = oldcreateFeatureVector(query, testType);

	for (int i = 0; i < allArticles->size(); i++)
	{
		ClothArticle* curr = allArticles->at(i);

		if (testType == "ClothingType" && curr->getClType() == query->getClType() || testType == "Color" && curr->getColor() == query->getColor())
		{
		cv::Mat currFeat = oldcreateFeatureVector(curr, testType);
		float dist = calcEuclDist(queryFeat, currFeat, cv::Mat());

		tuple<float, string> currPriEntry;
		currPriEntry = make_tuple(dist, curr->getId());

		priQueue.push(currPriEntry);
		}
		else if (testType == "ClType_NoML" || testType == "Color_NoML")
		{
		cv::Mat currFeat = oldcreateFeatureVector(curr, testType);
		float dist = calcEuclDist(queryFeat, currFeat, cv::Mat());

		tuple<float, string> currPriEntry;
		currPriEntry = make_tuple(dist, curr->getId());

		priQueue.push(currPriEntry);
		}
		else if (testType == "None")
		{
		cv::Mat currFeat = oldcreateFeatureVector(curr, "Color+ClothingType");
		float dist = calcEuclDist(queryFeat, currFeat, cv::Mat());

		tuple<float, string> currPriEntry;
		currPriEntry = make_tuple(dist, curr->getId());

		priQueue.push(currPriEntry);
		}
		else if (testType == "All" && curr->getClType() == query->getClType() && curr->getColor() == query->getColor())
		{
		cv::Mat currFeat = oldcreateFeatureVector(curr, "Color+ClothingType");
		float dist = calcEuclDist(queryFeat, currFeat, cv::Mat());

		tuple<float, string> currPriEntry;
		currPriEntry = make_tuple(dist, curr->getId());

		priQueue.push(currPriEntry);
		}
	}

	vector<string> topResults;

#ifdef _DEBUG
	cout << to_string(query->getClType()) << endl << to_string(query->getColor()) << endl;
	if (false)
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


cv::Mat createFilterVector(cv::Size vecSize, string filtType, float posScale, float negScale)
{
	cv::Mat filtVec(vecSize, CV_32FC1, cv::Scalar(1.0f));
	if (filtType != "All")
	{
		if (filtType == "Color")
		{
			for (int i = 0; i < filtVec.cols; i++)
			{
				filtVec.at<float>(0, i) = i >= 2 * EDGE_FEATURE_SIZE + Config::get().NUM_OF_GRAD_ANGS ? posScale : negScale;
			}
		}
		else if (filtType == "ClothingType")
		{
			for (int i = 0; i < filtVec.cols; i++)
			{
				filtVec.at<float>(0, i) = i < 2 * EDGE_FEATURE_SIZE ? posScale : negScale;
			}
		}
		else if (filtType == "Pattern")
		{
			for (int i = 0; i < filtVec.cols; i++)
			{
				filtVec.at<float>(0, i) = i >= 2 * EDGE_FEATURE_SIZE && i < 2 * EDGE_FEATURE_SIZE + Config::get().NUM_OF_GRAD_ANGS ? posScale : negScale;
			}
		}
		else if (filtType == "Clusterer")
		{
			for (int i = 0; i < filtVec.cols; i++)
			{
				filtVec.at<float>(0, i) = i < 2 * EDGE_FEATURE_SIZE && i >= EDGE_FEATURE_SIZE ? posScale : negScale;
			}
		}
		else
		{
			cout << "Wrong" << endl;
		}
	}
	else
	{
		for (int i = 0; i < filtVec.cols; i++)
		{
			//cout << "COLOR: " << (float)(32 * 6) / (float)filtVec.cols << endl;
			//cout << "CLTYPE: " << (float)(2 * EDGE_FEATURE_SIZE) / (float)filtVec.cols << endl;
			//cout << "PATTERN: " << (float)Config::get().NUM_OF_GRAD_ANGS / (float)filtVec.cols << endl;
			//cout << "TOTAL: " << (float)(32 * 6) / (float)filtVec.cols + (float)(2 * EDGE_FEATURE_SIZE) / (float)filtVec.cols + (float)Config::get().NUM_OF_GRAD_ANGS / (float)filtVec.cols << endl;

			if (i >= 2 * EDGE_FEATURE_SIZE + Config::get().NUM_OF_GRAD_ANGS)
				filtVec.at<float>(0, i) = 1. / (float)(6 * 3);
			else if (i < 2 * EDGE_FEATURE_SIZE)
				filtVec.at<float>(0, i) = 1. / (float)(2 * 3);
			else
				filtVec.at<float>(0, i) = 1. / (float)(Config::get().NUM_OF_GRAD_ANGS * 3);
		}
	}

	return filtVec;
}

/**Creates a feature vector.
*
* \param input Article who's feature vector is going to be extracted.
* \param testType Type of feature vector, e.g. "Color", "ClothingType" or "Color+ClothingType".
* \return Feature vector of the given article.
*/
cv::Mat createFeatureVector(ClothArticle* input) //, string fVecType)
{
	cv::Mat fVec;
	ImageFeatures *inpFeats = input->getImgFeats();

	fVec = cv::Mat(1, 2 * EDGE_FEATURE_SIZE + Config::get().NUM_OF_GRAD_ANGS + 6 * 32, CV_32F);

	cv::Mat tmp = inpFeats->getEdgeVect(0);

	for (int j = 0; j < tmp.rows; j++)
	{
		fVec.at<float>(0, j) = tmp.at<float>(j, 0);
	}

	tmp = inpFeats->getEdgeVect(1);
	//tmp = inpFeats->getBinVect(0);

	for (int j = 0; j < tmp.rows; j++)
	{
		fVec.at<float>(0, j + EDGE_FEATURE_SIZE) = tmp.at<float>(j, 0);
	}

	tmp = inpFeats->getEdgeVect(2);
	for (int j = 0; j < tmp.rows; j++)
	{
		fVec.at<float>(0, j + 2 * EDGE_FEATURE_SIZE) = tmp.at<float>(j, 0);
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
			fVec.at<float>(0, tmp.rows * j + k + 2 * EDGE_FEATURE_SIZE + Config::get().NUM_OF_GRAD_ANGS) = (float)tmp.at<float>(k, 0);
		}
	}

	return fVec;
}


cv::Mat oldcreateFeatureVector(ClothArticle* input, string fVecType)
{
	cv::Mat fVec;
	ImageFeatures *inpFeats = input->getImgFeats();

	if (fVecType == "Color")
	{
		fVec = cv::Mat(1, 32 * 6, CV_32F);
		for (int j = 0; j < 6; j++)
		{
			cv::Mat tmp;
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
				fVec.at<float>(0, tmp.rows * j + k) = (float)tmp.at<float>(k, 0);
			}
		}
	}
	else if (fVecType == "ClothingType")
	{
		fVec = cv::Mat(1, 2 * EDGE_FEATURE_SIZE, CV_32FC1);

		cv::Mat tmp = inpFeats->getEdgeVect(0);

		for (int j = 0; j < tmp.rows; j++)
		{
			fVec.at<float>(0, j) = tmp.at<float>(j, 0);
		}

		tmp = inpFeats->getEdgeVect(1);


		for (int j = 0; j < tmp.rows; j++)
		{
			fVec.at<float>(0, j + EDGE_FEATURE_SIZE) = tmp.at<float>(j, 0);
		}
	}
	else if (fVecType == "All" )
	{
		fVec = cv::Mat(1, 2 * EDGE_FEATURE_SIZE + 32 * 6, CV_32FC1);

		cv::Mat tmp = inpFeats->getEdgeVect(0);

		for (int j = 0; j < tmp.rows; j++)
		{
			fVec.at<float>(0, j) = tmp.at<float>(j, 0);
		}

		tmp = inpFeats->getEdgeVect(1);

		for (int j = 0; j < tmp.rows; j++)
		{
			fVec.at<float>(0, j + EDGE_FEATURE_SIZE) = tmp.at<float>(j, 0);
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
				fVec.at<float>(0, tmp.rows * j + k + 2 * EDGE_FEATURE_SIZE) = (float)tmp.at<float>(k, 0);
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
		else if (classifierGroup == "Clusterer")
			labels.push_back(input->at(i)->getClusterId());
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

		for (int k = 0; k < multVec.cols; k++)
		{
			trainingDataMat.at<float>(i, k) = multVec.at<float>(0, k);
		}
	}

	cv::Ptr<cv::ml::TrainData> tData = cv::ml::TrainData::create(trainingDataMat, cv::ml::SampleTypes::ROW_SAMPLE, labelsMat); // <-- Den här får fel data vid ClothingType

	return tData;
}


cv::Ptr<cv::ml::TrainData> oldcreateTrainingData(vector<ClothArticle*> *input, string classifierGroup)
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
		dataMatFeature = EDGE_FEATURE_SIZE * 2;
	}

	cv::Mat trainingDataMat(input->size(), dataMatFeature, CV_32FC1);
	for (int i = 0; i < labels.size(); i++)
	{
		cv::Mat tmp = oldcreateFeatureVector(input->at(i), classifierGroup);

		for (int k = 0; k < tmp.cols; k++)
		{
			trainingDataMat.at<float>(i, k) = tmp.at<float>(0, k);
		}
	}

	cv::Ptr<cv::ml::TrainData> tData = cv::ml::TrainData::create(trainingDataMat, cv::ml::SampleTypes::ROW_SAMPLE, labelsMat); // <-- Den här får fel data vid ClothingType

	return tData;
}


void clusterCataloge(vector<ClothArticle*> *cataloge, string filtType)
{

	int clusterCount = MIN(Config::get().MAXIMUM_CLUSTER_COUNT,cataloge->size());

	cv::Mat points(cataloge->size(), 4 * EDGE_FEATURE_SIZE + 6 * 32, CV_32F);
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
		cataloge->at(i)->setClusterId(labels.at<int>(i));
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