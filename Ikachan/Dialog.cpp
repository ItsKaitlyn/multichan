#include "Dialog.h"
#include "System.h"
#include <stdio.h>

//Options
struct OPTION
{
	int garbage; //unused and not written to
	int wnd_size;
	int framerate;
	int font_name;
};

const char* gOptionName = "Option.bin";

BOOL SaveOption(OPTION *option)
{
	//Open option file
	char path[MAX_PATH];
	sprintf(path, "%s\\%s", gModulePath, gOptionName);

	FILE *fp = fopen(path, "wb");
	if (fp == NULL)
		return FALSE;

	//Write options
	fwrite(option, sizeof(OPTION), 1, fp);
	fclose(fp);
	return TRUE;
}

BOOL LoadOption(OPTION *option)
{
	//Clear option struct
	memset(option, 0, sizeof(OPTION));

	//Open option file
	char path[MAX_PATH];
	sprintf(path, "%s\\%s", gModulePath, gOptionName);

	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return FALSE;

	//Read options
	fread(option, sizeof(OPTION), 1, fp);
	fclose(fp);
	return TRUE;
}

//Dialog functions
void InitDialog(HWND hDlg)
{
	//Window size labels
	const char* size_names[5] = {
		"Fullscreen",
		"Windowed (480x270)",
		"Windowed (960x540)",
		"Windowed (1440x810)",
		"Windowed (1920x1080)",
	};

	//Framerate labels
	const char* framerate_names[2] = {
		"50FPS",
		"60FPS",
	};

	//Read options
	OPTION option;
	if (!LoadOption(&option))
	{
		//Default options
		option.wnd_size = WS_FULLSCREEN;
		option.framerate = FS_60FRAME;
	}

	for (int i = 0; i < 5; i++)
		SendDlgItemMessage(hDlg, 1003, CB_ADDSTRING, 0, (LPARAM)size_names[i]);
	for (int i = 0; i < 2; i++)
		SendDlgItemMessage(hDlg, 1004, CB_ADDSTRING, 0, (LPARAM)framerate_names[i]);
		
	//Select options initially on dialog
	SendDlgItemMessage(hDlg, 1003, CB_SETCURSEL, (WPARAM)option.wnd_size, 0);
	SendDlgItemMessage(hDlg, 1004, CB_SETCURSEL, (WPARAM)option.framerate, 0);

}

void UseDialog(HWND hDlg)
{
	//Set properties from dialog
	gWndSize = (WND_SIZE)SendDlgItemMessage(hDlg, 1003, CB_GETCURSEL, 2, 0);
	gFramerateSpeed = (FRAMERATESPEED)SendDlgItemMessage(hDlg, 1004, CB_GETCURSEL, 2, 0);

	//Write options
	OPTION option;
	option.wnd_size = (int)gWndSize;
	option.framerate = (int)gFramerateSpeed;
	SaveOption(&option);
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case WM_INITDIALOG:
			InitDialog(hDlg);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case 2:
					EndDialog(hDlg, 0);
					break;
				case 1:
					UseDialog(hDlg);
					EndDialog(hDlg, 1);
					break;
			}
		default:
			return FALSE;
	}
	return TRUE;
}
