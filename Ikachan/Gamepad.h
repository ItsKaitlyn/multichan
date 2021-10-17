#pragma once

#include "WindowsWrapper.h"
#include <windows.h>

extern int gJoystickButtonTable[8];

struct JOYSTICK_STATUS
{
	BOOL bLeft;
	BOOL bRight;
	BOOL bUp;
	BOOL bDown;
	BOOL bButton[32];
};

struct CONFIG
{
	long joystick_button[8];
};

void DefaultConfigData(CONFIG *conf);


void ReleaseDirectInput(void);
BOOL InitDirectInput(HINSTANCE hinst, HWND hWnd);
BOOL GetJoystickStatus(JOYSTICK_STATUS *status);
BOOL ResetJoystickStatus(void);
