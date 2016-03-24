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
* Some utillity functions that is helping with WIN API.
*
*/

#include "my_win_utills.h"


/**Converts strings to wstrings.
*
* \param s String that is being converted.
* \return The resulting wstring.
*/
std::wstring string2wstring(const std::string& s)
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

/**Sends a message to a mailslot.
*
* \param hSlot Handle for the mailslot.
* \param lpszMessage The message that is being sent.
* \return Return true if succesful, false otherwise.
*/
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

/**Reads a message sent to a mailslot.
*
* \param hSlot Handle for the mailslot.
* \return Returns the message that was sent.
*/
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

/**Makes a mailslot where one can recieve messages.
* 
* \param lpszSlotName The name of the mailslot.
* \param hSlot The handle that will hold the mailslot.
* \return Returns true if successful, false otherwise.
*/
BOOL WINAPI MakeReciSlot(LPTSTR lpszSlotName, HANDLE *hSlot) 
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

/**Makes a mailslot where one can send messages.
*
* \param lpszSlotName The name of the mailslot.
* \param hFile The handle that will hold the mailslot.
* \return Returns true if successful, false otherwise.
*/
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

/**Checks if the backend can recieve a new request.
*
* \param lpszSlotName The name of the mailslot.
* \param hFile The handle to the mailslot that is being connected to the backend. 
* \return Returns true if backend can recieve a new request, false otherwise.
*/
BOOL isOnline(LPTSTR lpszSlotName, HANDLE *hSlot)
{
	if (!MakeSendSlot(lpszSlotName, hSlot))
		return FALSE;
	BOOL ret = writeSlot(*hSlot, TEXT("STATUS\n"));
	CloseHandle(*hSlot);
	return ret;
}

/**Creates a group of radio buttons for the selection of features.
*
* \param hwnd The parent handle.
* \param cs Information about the parent.
* \param x The offset in the x direction for the whole group.
* \return Handle with the new group.
*/
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

	SendMessage(but0, BM_SETCHECK, (WPARAM)BST_CHECKED, NULL);

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

/**Creates a group of radio buttons for the selection of filters.
*
* \param hwnd The parent handle.
* \param cs Information about the parent.
* \param x The offset in the x direction for the whole group.
* \return Handle with the new group.
*/
HWND makeFiltButtons(const HWND hwnd, CREATESTRUCT *cs, int x, int y)
{
	HWND outGroup = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("Filter"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_GROUPBOX,             //control style 
		x,												    //position: left
		y,													  //position: top
		545,												  //width
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

	SendMessage(but0, BM_SETCHECK, (WPARAM)BST_CHECKED, NULL);

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
		_T("CluColor"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,             //control style 
		310,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)filtClustColor,													//control's ID
		cs->hInstance,									      //application instance
		0);

	HWND but5 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("CluClType"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,             //control style 
		385,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)filtClustClType,													//control's ID
		cs->hInstance,									      //application instance
		0);

	HWND but6 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("All"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,             //control style 
		460,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)filtAll,													//control's ID
		cs->hInstance,									      //application instance
		0);

	return outGroup;
}

/**Sets up the gui.
*
* \param hwnd The parent handle.
* \param cs Information about the parent.
*/
void OnCreate(const HWND hwnd, CREATESTRUCT *cs)
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
		WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_HASSTRINGS,
		10,
		90,
		300,
		20,
		hwnd,
		NULL,
		NULL,
		NULL);


	TCHAR Planets[12][3] =
	{
		TEXT("1"), TEXT("2"), TEXT("3"), TEXT("4"),
		TEXT("5"), TEXT("6"), TEXT("7"), TEXT("8"),
		TEXT("9"), TEXT("10"), TEXT("11"), TEXT("12")
	};

	TCHAR A[16];
	int  k = 0;

	memset(&A, 0, sizeof(A));
	for (k = 0; k < 12; k += 1)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)Planets[k]);

		// Add string to combobox.
		SendMessage(NumResText, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
	}

	// Send the CB_SETCURSEL message to display an initial item 
	//  in the selection field  
	SendMessage(NumResText, CB_SETCURSEL, (WPARAM)11, (LPARAM)0);


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
}

/**Checks which radio button in a gruop that is selected.
*
* \param groupHWND The handle of the group.
* \param first The id of the button with lowest id.
* \param last The id of the  button with highest id.
* \return Returns the id of the selected button. Returns 0 if no button was foun.
*/
int checkRadioGroup(HWND groupHWND, int first, int last)
{
	for (int i = first; i <= last; i++)
	{
		if (IsDlgButtonChecked(groupHWND, i))
			return i;
	}
	return 0;
}

