#ifndef MY_WIN_UTILLS_H
#define MY_WIN_UTILLS_H

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "Config.h"

#include <Windows.h>
#include <Winuser.h>
#include <CommCtrl.h>
#include <tchar.h>

#include <iostream>
#include <vector>
#include <string>

using namespace std;

typedef std::basic_string<TCHAR> ustring;

std::wstring string2wstring(const std::string& s);


enum
{
	featAll = 100,
	featClType,
	featSill,
	featPattern,
	featColor,
	featTemplate,
	filtNone,
	filtColor,
	filtClType,
	filtCluster,
	filtClustColor,
	filtClustClType,
	filtAll,
	searchButton,
	onlineButton,
	res0, res1, res2,
	res3, res4, res5,
	res6, res7, res8,
	res9, res10, res11
};

void checkFeatValues(double dst[5], const HWND hwnd);
void checkFeatGroup(int dst[5], const HWND hwnd, int offset);
HWND makeFeatValues(const HWND hwnd, CREATESTRUCT *cs, int x, int y);
HWND makeFeatButtons(const HWND hwnd, CREATESTRUCT *cs, int x, int y);
HWND makeFiltButtons(const HWND hwnd, CREATESTRUCT *cs, int x, int y);
void OnCreate(const HWND hwnd, CREATESTRUCT *cs);
int checkRadioGroup(HWND groupHWND, int first, int last);


#endif