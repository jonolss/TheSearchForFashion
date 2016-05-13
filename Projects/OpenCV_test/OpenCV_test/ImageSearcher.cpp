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
LPCWSTR g_szClassName = TEXT("myWindowClass");


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

/**Prints a simple text-based menu.
*
* \param message A message that is displayed under the menu.
*/
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

/**Text based interface loop for handling input and output of the user.
*
* /param catalogePath The path to cataloge of items.
*/
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
					cout << "Choose filters (0 - None, 1 - Same Color, 2 - Same Clothing Type, 3 - Silhouette , 4 - All): ";
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
							query += "Silhouette\n";
							valid = true;
						}
						else if (filter == "4")
						{
							query += "ClustColor\n";
						}
						else if (filter == "5")
						{
							query += "ClustClType\n";
						}
						else if (filter == "6")
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

				std::wstring stemp = string2wstring(query);
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


/**Checks if search query string is valid.
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

/**Creates a search query from input.
*
* \param path The path of the query.
* \param numRes The number of the results the query wants.
* \param featNum The type of features being used in the query.
* \param filtNum The type of filter being used in the query.
* \return A string of the query that can be directly send to the backend.
*/
inline string createQuery(LPTSTR path, LPTSTR numRes, int featNum[4], double featVal[4], int filtNum)
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

	
	if(featNum[0])
		out += "ClothingType,";
	if (featNum[1])
		out += "Silhouette,";
	if (featNum[2])
		out += "Pattern,";
	if (featNum[3])
		out += "Color,";
	if (featNum[4])
		out += "Template,";
	out += "\n";

	out += to_string(featVal[0]) + ",";
	out += to_string(featVal[1]) + ",";
	out += to_string(featVal[2]) + ",";
	out += to_string(featVal[3]) + ",";
	out += to_string(featVal[4]) + "\n";

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
		out += "Silhouette\n";
		break;
	case filtClustColor:
		out += "ClustColor\n";
		break;
	case filtClustClType:
		out += "ClustClType\n";
		break;
	case filtAll:
		out += "All\n";
		break;
	}

	return out;
}

/**Sends a search query to the backend.
*
* \param results The result of request.
* \param query The query being sent.
* \return Returns true if it succeded, else false.
*/
bool sendQuery(vector<string> &results, string query)
{
	std::wstring stemp = string2wstring(query);
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

/**Responds to messages sent by the OS/User.
*
*/
inline LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	HWND tmp;
	LPTSTR path = (LPTSTR)new char[32];
	LPTSTR numRes = (LPTSTR)new char[32];
	int featNum[] = { 0,0,0,0,0 };
	int filtNum = filtNone;
	double featVal[] = { 0.,0.,0.,0.,0. };
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
				checkFeatGroup(featNum, tmp, featClType);

				tmp = FindWindowEx(hwnd, NULL, _T("static"), _T("Feat Multiplier"));
				checkFeatValues(featVal, tmp);

				//tmp = FindWindowEx(hwnd, NULL, _T("button"), _T("Filter"));
				//filtNum = checkRadioGroup(tmp, filtNone, filtAll);

				cout << featVal[0] << endl;
				cout << featVal[1] << endl;
				cout << featVal[2] << endl;
				cout << featVal[3] << endl;
				cout << featVal[4] << endl;

				query = createQuery(path, numRes, featNum, featVal, filtNum);

				cout << query << endl;

				tmp = FindWindowEx(hwnd, NULL, _T("static"), NULL);
				tmp = FindWindowEx(hwnd, tmp, _T("static"), NULL);
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

					cv::Mat tmpImg3x;
					fixInternalPadding(tmpImg3, tmpImg3x);

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

					cv::destroyAllWindows();

//#define SHOW_EDGE
#ifdef SHOW_EDGE
					cv::namedWindow("Query EDGE", 1);
					cv::imshow("Query EDGE", edge);

					cv::namedWindow("Query EDGE2", 1);
					cv::imshow("Query EDGE2", edge2);
#endif
					cv::namedWindow("Query", 1);

#ifdef _PADDING
					cv::imshow("Query", tmpImg3x);
#else
					cv::imshow("Query", tmpImg3);
#endif
					for (int i = 0; i < results.size(); i++)
					{
						cout << results[i] << " : " << hashTable[results[i]] << endl;
						tmpImg = cv::imread(hashTable[results[i]], cv::IMREAD_UNCHANGED);
						cout << hashTable[results[i]] << endl;
						if (hashTable[results[i]].find(".png") != string::npos)
						{
							filterAlphaArtifacts(&tmpImg);
							cv::cvtColor(tmpImg, tmpImg, CV_BGRA2BGR);
						}
						tmpImg2 = resizeImg(tmpImg, 300, 300);

						cv::Mat tmpImg2x;
						fixInternalPadding(tmpImg2, tmpImg2x);

						cv::Mat tmpImg3;
						cv::cvtColor(tmpImg2x, tmpImg3, CV_BGR2GRAY);
						cv::threshold(tmpImg3, tmpImg3, 248, 255, CV_THRESH_BINARY_INV);
						cv::Mat tmpImg4;
						onlyBackground(tmpImg3, tmpImg4);
						tmpImg4 *= 255;
						tmpImg4 = preformGaussianBlur(tmpImg4);
						tmpImg4 = preformCanny(tmpImg4, 80, 140);

						cv::namedWindow("Result # " + to_string(i + 1), 1);
#ifdef _PADDING
						cv::imshow("Result # " + to_string(i + 1), tmpImg2x);
#else
						cv::imshow("Result # " + to_string(i + 1), tmpImg2);
#endif

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
			tmp = FindWindowEx(hwnd, tmp, _T("static"), NULL);
			if (isOnline(FRONT_TO_BACK_SLOT, &sendSlot))
				SendMessage(tmp, WM_SETTEXT, (WPARAM)0, (LPARAM)TEXT("Backend is online."));
			else
				SendMessage(tmp, WM_SETTEXT, (WPARAM)0, (LPARAM)TEXT("Backend is offline."));
			break;
		}
		break;
	case WM_CREATE:
		OnCreate(hwnd, reinterpret_cast<CREATESTRUCT*>(lParam));
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

/**Graphical interface loop for handling input and output of the user.
*
* /param catalogePath The path to cataloge of items.
* /return Returns 0 if closed correctly, else it returns an error.
*/
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
		return -1;
	}

	// Step 2: Creating the Window
	hwnd = CreateWindowEx(
		0,
		g_szClassName,
		TEXT("The Search For Fashion"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		550,
		400,
		NULL,
		NULL,
		NULL,
		NULL);



	if (hwnd == NULL)
	{
		MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error!"),
			MB_ICONEXCLAMATION | MB_OK);
		return -1;
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

#define BUFFER_SIZE 1000
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

	char inBuf[BUFFER_SIZE];
	char outBuf[BUFFER_SIZE];

	LPTSTR lpszPipename1 = TEXT("\\\\.\\pipe\\myNamedPipe1");
	LPTSTR lpszPipename2 = TEXT("\\\\.\\pipe\\myNamedPipe2");

	DWORD cbWritten;
	DWORD dwBytesToWrite = (DWORD)strlen(outBuf);

	DWORD cbRead;
	DWORD dwBytesToRead = BUFFER_SIZE;


	HANDLE hPipe1 = CreateNamedPipe(lpszPipename1, PIPE_ACCESS_DUPLEX/* | FILE_FLAG_OVERLAPPED*/, PIPE_TYPE_MESSAGE, 1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);
	HANDLE hPipe2 = CreateNamedPipe(lpszPipename2, PIPE_ACCESS_DUPLEX/* | FILE_FLAG_OVERLAPPED*/, PIPE_TYPE_MESSAGE, 1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);

	HANDLE mutexSem = CreateSemaphore(NULL, 1, 1, _TEXT("Global\\sem_mutex"));
	HANDLE syncSem = CreateSemaphore(NULL, 0, 1, _TEXT("Global\\sem_sync"));


	int count = 0;

	while (true)
	{
		cout << count++ << endl;
		vector<string> reqArgs;
		/*
		while (reqArgs.empty())
		{
			string request = readSlot(reciSlot);

			if (request == "FALSE")
				return 1;


			if (request != "")
			{
				char tmp;
				int pos = request.find('\n');
				while (pos != string::npos)
				{
					reqArgs.push_back(request.substr(0, pos));
					request = request.substr(pos + 1, request.length() - pos + 1);
					pos = request.find('\n');
				}
			}
			else
			{
				Sleep(1);
			}
		}
		*/
		
		
		cout << "Waiting for client..." << endl;
		ReleaseSemaphore(mutexSem, 1, NULL);
		ConnectNamedPipe(hPipe1, NULL);
		ReleaseSemaphore(syncSem, 1, NULL);
		ConnectNamedPipe(hPipe2, NULL);
		cout << "Pipes connected." << endl;

		memset(inBuf, 0, BUFFER_SIZE);
		ReadFile(hPipe1, inBuf, dwBytesToRead, &cbRead, NULL);

		string request = string(inBuf);

		string stmp = request;
		char ctmp;
		int pos = stmp.find('\n');
		while (pos != string::npos)
		{
			reqArgs.push_back(stmp.substr(0, pos));
			stmp = stmp.substr(pos + 1, stmp.length() - pos + 1);
			pos = stmp.find('\n');
		}

		string reqType = reqArgs[0];
		if (reqType == "imgSearch" && validQuery(request))
		{
			string path = reqArgs[1];
			int    n = stoi(reqArgs[2]);
			string fVecType = reqArgs[3];
			string fVecVals = reqArgs[4];  // <--- fixa in dom här till featFilter
			string filters = reqArgs[5];

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
				strcpy(tmp, fVecType.c_str());
				char *tmpPoint;
				tmpPoint = strtok(tmp, ",");
				while (tmpPoint != NULL)
				{
					fVecTypes.push_back(string(tmpPoint));
					tmpPoint = strtok(NULL, ",");
				}
			}

			vector<double> fVecValsD;
			{
				char tmp[120];
				strcpy(tmp, fVecVals.c_str());
				char *tmpPoint;
				tmpPoint = strtok(tmp, ",");
				while (tmpPoint != NULL)
				{
					fVecValsD.push_back(stod(string(tmpPoint)));
					tmpPoint = strtok(NULL, ",");
				}
			}


			//vector<string> closeNeigh = findClosestNeighbours(allArticles, queryArticle, n, fVecType, filters);
			vector<string> closeNeigh = findClosestNeighbours(allArticles, queryArticle, n, fVecTypes, fVecValsD, filters);

			string answer = "";
			for (int i = 0; i < closeNeigh.size(); i++)
			{
				answer += hashTable[closeNeigh[i]] + '\n';
			}

			memset(outBuf, 0, BUFFER_SIZE);
			strncpy(outBuf, answer.c_str(), BUFFER_SIZE);
			dwBytesToWrite = (DWORD)strlen(outBuf);
			WriteFile(hPipe2, outBuf, dwBytesToWrite, &cbWritten, NULL);
		}
		else
		{
			string answer = "Wrong input.\n";
			memset(outBuf, 0, BUFFER_SIZE);
			strncpy(outBuf, answer.c_str(), BUFFER_SIZE);
			dwBytesToWrite = (DWORD)strlen(outBuf);
			WriteFile(hPipe2, outBuf, dwBytesToWrite, &cbWritten, NULL);
		}

		DisconnectNamedPipe(hPipe1);
		DisconnectNamedPipe(hPipe2);
		cout << "Pipes disconnected." << endl;
		
	}

	CloseHandle(hPipe1);
	CloseHandle(hPipe2);
	CloseHandle(syncSem);
	CloseHandle(mutexSem);
}

/**Backend loop that handles requests from the front end of the program.
*
* \param catalogePath The path to cataloge of items.
* \param embeded Should be true if started as a child process. False if it is started stand-alone.
* \return Return 0 if closed correctly, else it returns an error.
*/
int backend(string catalogePath, bool embeded)
{
	
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
			string path = reqArgs[1];
			int    n = stoi(reqArgs[2]);
			string fVecType = reqArgs[3];
			string fVecVals = reqArgs[4];  // <--- fixa in dom här till featFilter
			string filters = reqArgs[5];

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
				strcpy(tmp, fVecType.c_str());
				char *tmpPoint;
				tmpPoint = strtok(tmp, ",");
				while (tmpPoint != NULL)
				{
					fVecTypes.push_back(string(tmpPoint));
					tmpPoint = strtok(NULL, ",");
				}
			}

			vector<double> fVecValsD;
			{
				char tmp[120];
				strcpy(tmp, fVecVals.c_str());
				char *tmpPoint;
				tmpPoint = strtok(tmp, ",");
				while (tmpPoint != NULL)
				{
					fVecValsD.push_back(stod(string(tmpPoint)));
					tmpPoint = strtok(NULL, ",");
				}
			}
			

			//vector<string> closeNeigh = findClosestNeighbours(allArticles, queryArticle, n, fVecType, filters);
			vector<string> closeNeigh = findClosestNeighbours(allArticles, queryArticle, n, fVecTypes, fVecValsD, filters);

			string answer = "";
			for (int i = 0; i < closeNeigh.size(); i++)
			{
				answer += closeNeigh[i] + '\n';
			}

			if (MakeSendSlot(BACK_TO_FRONT_SLOT, &sendSlot))
			{
				std::wstring stemp = string2wstring(answer);
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
* \param testType Type of feature vector, e.g. "Color", "ClothingType" or "Color+ClothingType".
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