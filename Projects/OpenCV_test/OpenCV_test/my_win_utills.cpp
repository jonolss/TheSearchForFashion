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
		410,												  //width
		40,													  //height
		hwnd,											       //parent window handle							  
		(HMENU)2,													//control's ID
		cs->hInstance,									      //application instance
		0);

	HWND but0 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("ClType"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,             //control style 
		10,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)featClType,													//control's ID
		cs->hInstance,									      //application instance
		0);

	SendMessage(but0, BM_SETCHECK, (WPARAM)BST_CHECKED, NULL);

	HWND but1 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("Silhouette"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,             //control style 
		85,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)featSill,													//control's ID
		cs->hInstance,									      //application instance
		0);

	SendMessage(but1, BM_SETCHECK, (WPARAM)BST_CHECKED, NULL);

	HWND but2 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("Pattern"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,             //control style 
		160,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)featPattern,													//control's ID
		cs->hInstance,									      //application instance
		0);

	SendMessage(but2, BM_SETCHECK, (WPARAM)BST_CHECKED, NULL);

	HWND but3 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("Color"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,             //control style 
		235,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)featColor,													//control's ID
		cs->hInstance,									      //application instance
		0);

	SendMessage(but3, BM_SETCHECK, (WPARAM)BST_CHECKED, NULL);

	HWND but4 = CreateWindowEx(0,				  //extended styles
		_T("button"),											 //control 'class' name
		_T("Template"),											 //control caption
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,             //control style 
		310,												    //position: left
		20,													  //position: top
		75,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)featTemplate,													//control's ID
		cs->hInstance,									      //application instance
		0);

	SendMessage(but4, BM_SETCHECK, (WPARAM)BST_CHECKED, NULL);

	return outGroup;
}

void checkFeatGroup(int dst[5], const HWND hwnd, int offset)
{
	for (int i = 0; i < 5; i++)
	{
		dst[i] = IsDlgButtonChecked(hwnd, i + offset);
	}
}


HWND makeFeatValues(const HWND hwnd, CREATESTRUCT *cs, int x, int y)
{
	HWND outGroup = CreateWindowEx(WS_EX_WINDOWEDGE,				  //extended styles
		WC_STATIC,											 //control 'class' name
		_T("Feat Multiplier"),											 //control caption
		WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP,             //control style 
		x,												    //position: left
		y,													  //position: top
		410,												  //width
		45,													  //height
		hwnd,											       //parent window handle							  
		(HMENU)2,													//control's ID
		cs->hInstance,									      //application instance
		0);

	HWND val0 = CreateWindowEx(0,				  //extended styles
		WC_COMBOBOX,											 //control 'class' name
		TEXT(""),
		WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP,           //control style 
		10,												    //position: left
		20,													  //position: top
		45,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)featClType,													//control's ID
		cs->hInstance,									      //application instance
		0);

	SendMessage(val0, WM_SETTEXT, (WPARAM)NULL, (LPARAM)(string2wstring(to_string(Config::get().FEAT_MULT_CLTYPE).substr(0, 4)).c_str()));


	HWND val1 = CreateWindowEx(0,				  //extended styles
		WC_COMBOBOX,											 //control 'class' name
		TEXT(""),
		WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP,            //control style 
		85,												    //position: left
		20,													  //position: top
		45,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)featSill,													//control's ID
		cs->hInstance,									      //application instance
		0);

	SendMessage(val1, WM_SETTEXT, (WPARAM)NULL, (LPARAM)(string2wstring(to_string(Config::get().FEAT_MULT_SILLHOUETTE).substr(0, 4)).c_str()));


	HWND val2 = CreateWindowEx(0,				  //extended styles
		WC_COMBOBOX,											 //control 'class' name
		TEXT(""),
		WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP,          //control style 
		160,												    //position: left
		20,													  //position: top
		45,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)featPattern,													//control's ID
		cs->hInstance,									      //application instance
		0);

	SendMessage(val2, WM_SETTEXT, (WPARAM)NULL, (LPARAM)(string2wstring(to_string(Config::get().FEAT_MULT_PATTERN).substr(0, 4)).c_str()));


	HWND val3 = CreateWindowEx(0,				  //extended styles
		WC_COMBOBOX,											 //control 'class' name
		TEXT(""),
		WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP,         //control style 
		235,												    //position: left
		20,													  //position: top
		45,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)featColor,													//control's ID
		cs->hInstance,									      //application instance
		0);

	SendMessage(val3, WM_SETTEXT, (WPARAM)NULL, (LPARAM)(string2wstring(to_string(Config::get().FEAT_MULT_COLOR).substr(0, 4)).c_str()));


	HWND val4 = CreateWindowEx(0,				  //extended styles
		WC_COMBOBOX,											 //control 'class' name
		TEXT(""),
		WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_TABSTOP,         //control style 
		310,												    //position: left
		20,													  //position: top
		45,												  //width
		20,													  //height
		outGroup,											       //parent window handle							  
		(HMENU)featTemplate,													//control's ID
		cs->hInstance,									      //application instance
		0);

	SendMessage(val4, WM_SETTEXT, (WPARAM)NULL, (LPARAM)(string2wstring(to_string(Config::get().FEAT_MULT_TEMPLATE).substr(0, 4)).c_str())); //<-- den här får inte rätt siffra



	return outGroup;
}

void checkFeatValues(double dst[5], const HWND hwnd)
{
	LPTSTR data = (LPTSTR)new char[32];;
	HWND hwndTmp = FindWindowEx(hwnd, NULL, WC_COMBOBOX, NULL);
	GetWindowText(hwndTmp, data, 32);

	char tmp[32] = { '\0' };
	int i;
	for (i = 0; data[i] != '\0'; i++)
	{
		tmp[i] = data[i];
	}
	tmp[i] = '\0';
	dst[0] = atof(tmp);


	hwndTmp = FindWindowEx(hwnd, hwndTmp, WC_COMBOBOX, NULL);
	GetWindowText(hwndTmp, data, 32);

	for (i = 0; data[i] != '\0'; i++)
	{
		tmp[i] = data[i];
	}
	tmp[i] = '\0';
	dst[1] = atof(tmp);


	hwndTmp = FindWindowEx(hwnd, hwndTmp, WC_COMBOBOX, NULL);
	GetWindowText(hwndTmp, data, 32);

	for (i = 0; data[i] != '\0'; i++)
	{
		tmp[i] = data[i];
	}
	tmp[i] = '\0';
	dst[2] = atof(tmp);


	hwndTmp = FindWindowEx(hwnd, hwndTmp, WC_COMBOBOX, NULL);
	GetWindowText(hwndTmp, data, 32);

	for (i = 0; data[i] != '\0'; i++)
	{
		tmp[i] = data[i];
	}
	tmp[i] = '\0';
	dst[3] = atof(tmp);


	hwndTmp = FindWindowEx(hwnd, hwndTmp, WC_COMBOBOX, NULL);
	GetWindowText(hwndTmp, data, 32);

	for (i = 0; data[i] != '\0'; i++)
	{
		tmp[i] = data[i];
	}
	tmp[i] = '\0';
	dst[4] = atof(tmp);
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


	TCHAR elements[12][3] =
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
		//wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)elements[k]); //This is commented to take away an error.

		// Add string to combobox.
		SendMessage(NumResText, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
	}

	// Send the CB_SETCURSEL message to display an initial item 
	//  in the selection field  
	SendMessage(NumResText, CB_SETCURSEL, (WPARAM)11, (LPARAM)0);


	HWND featBox = makeFeatButtons(hwnd, cs, 10, 125);

	HWND featValBox = makeFeatValues(hwnd, cs, 10, 175);

	//HWND filtBox = makeFiltButtons(hwnd, cs, 10, 225);

	HWND hWnd3 = CreateWindowEx(0,          //extended styles
		_T("button"),						//control 'class' name
		_T("Search"),						//control caption
		WS_VISIBLE | WS_CHILD,				//control style 
		10,							        //position: left
		270,							    //position: top
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
		275,							    //position: top
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
		310,							    //position: top
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

