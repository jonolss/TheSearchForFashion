
#include "FrontEnd.h"

LPTSTR SlotNameFR = TEXT("\\\\.\\mailslot\\sample_mailslot");

BOOL WriteSlotFR(HANDLE hSlot, LPTSTR lpszMessage) // skickar meddelanden
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

bool frontLoop()  // TODO: Fixa meny för att skicka querys, gör så man kan ta emot svar.
{

	HANDLE hFile;

	hFile = CreateFile(SlotNameFR,
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

	WriteSlotFR(hFile, TEXT("hmtest2.jpg"));
	WriteSlotFR(hFile, TEXT("hmtest3.jpg"));

	Sleep(5000);

	WriteSlotFR(hFile, TEXT("hmtest3.jpg"));

	CloseHandle(hFile);
	
}





