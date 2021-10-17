#pragma once
#include <windows.h>

#include "WindowsWrapper.h"

//Window size
enum WND_SIZE
{
	WS_FULLSCREEN,
	WS_480x270,
	WS_960x540,
	WS_1440x810,
	WS_1920x1080,
	WS_FUCKYOU,
};
extern WND_SIZE gWndSize;

//Framerate Speed
enum FRAMERATESPEED
{
	FS_50FRAME,
	FS_60FRAME,
};
extern FRAMERATESPEED gFramerateSpeed;

//Framerate Speed
enum ResolutionScale
{
	RS_NORMAL,
	RS_PLUS,
};
extern ResolutionScale gResolutionScale;

//Windows objects
extern HWND ghWnd;

//Filepath
extern char gModulePath[MAX_PATH];

int Random(int min, int max);
BOOL SystemTask();
