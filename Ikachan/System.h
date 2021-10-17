#pragma once
#include <windows.h>

#include "WindowsWrapper.h"

//Keys
#define MOUSE_RIGHT (0x00000001)
#define MOUSE_LEFT  (0x00000002)

#define KEY_LEFT  (0x00000001)
#define KEY_RIGHT (0x00000002)
#define KEY_UP    (0x00000004)
#define KEY_DOWN  (0x00000008)
#define KEY_SPACE (0x00000010)
#define KEY_X     (0x00000020)
#define KEY_Z     (0x00000040)
#define KEY_S     (0x00000080)
//add escape keys that totally aren't from cave story
#define KEY_ESCAPE (0x000000100)
#define KEY_F1 (0x00000200)
#define KEY_F2 (0x00000400)

#define KEY_LEFT_PLAYERTWO  (0x00000800)
#define KEY_RIGHT_PLAYERTWO (0x00001000)
#define KEY_UP_PLAYERTWO    (0x00002000)
#define KEY_DOWN_PLAYERTWO  (0x00004000)
#define KEY_X_PLAYERTWO     (0x00008000)
#define KEY_Z_PLAYERTWO     (0x00010000)

extern DWORD gKey, gKeyTrg;
extern DWORD gMouse, gMouseTrg, gMouseTrg2;

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
