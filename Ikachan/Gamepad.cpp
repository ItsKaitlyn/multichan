#include "Gamepad.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#if defined(_MSC_VER) && _MSC_VER >= 1500	// Newer versions of Visual Studio don't support anything earlier than DirectInput8
#define DIRECTINPUT_VERSION 0x800
#else
#define DIRECTINPUT_VERSION 0x500
#endif
#include <dinput.h>

#include "WindowsWrapper.h"

typedef struct DirectInputPair
{
	LPDIRECTINPUTA lpDI;
	LPDIRECTINPUTDEVICE2A device;
} DirectInputPair;

static LPDIRECTINPUTDEVICE2A joystick;
static LPDIRECTINPUTA lpDI;

static int joystick_neutral_x;
static int joystick_neutral_y;

void DefaultConfigData(CONFIG *conf)
{
	// Clear old configuration data
	memset(conf, 0, sizeof(CONFIG));

	// Fun fact: The Linux port added this line:
	// conf->display_mode = 1;

	// Reset joystick settings (as these can't simply be set to 0)
	conf->joystick_button[0] = 2;
	conf->joystick_button[1] = 1;
	conf->joystick_button[2] = 5;
	conf->joystick_button[3] = 6;
	conf->joystick_button[4] = 3;
	conf->joystick_button[5] = 4;
	conf->joystick_button[6] = 6;
	conf->joystick_button[7] = 3;
}

void ReleaseDirectInput(void)
{
	if (joystick != NULL)
	{
		joystick->Release();
		joystick = NULL;
	}

	if (lpDI != NULL)
	{
		lpDI->Release();
		lpDI = NULL;
	}
}

// The original name for this function is unknown
BOOL SetDeviceAquire(BOOL aquire)
{
	if (aquire == TRUE)
	{
		if (joystick != NULL)
			joystick->Acquire();
	}
	else
	{
		if (joystick != NULL)
			joystick->Unacquire();
	}

	return TRUE;
}

BOOL HookAllDirectInputDevices(HWND hWnd);
BOOL __stdcall EnumDevices_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

BOOL InitDirectInput(HINSTANCE hinst, HWND hWnd)
{
#if defined(_MSC_VER) && _MSC_VER >= 1500
	if (DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8A, (LPVOID*)&lpDI, NULL) != DI_OK)
#else
	if (DirectInputCreateA(hinst, DIRECTINPUT_VERSION, &lpDI, NULL) != DI_OK)
#endif
		return FALSE;

	if (!HookAllDirectInputDevices(hWnd))
		return FALSE;

	return TRUE;
}

// The original name for this function is unknown
BOOL HookAllDirectInputDevices(HWND hWnd)
{
	DirectInputPair directinput_objects;

	directinput_objects.device = NULL;
	directinput_objects.lpDI = lpDI;

	lpDI->AddRef();
	lpDI->EnumDevices(4, EnumDevices_Callback, &directinput_objects, 1);

	if (directinput_objects.lpDI != NULL)
	{
		directinput_objects.lpDI->Release();
		directinput_objects.lpDI = NULL;
	}

	if (directinput_objects.device == NULL)
		return FALSE;

	joystick = directinput_objects.device;

	if (joystick->SetDataFormat(&c_dfDIJoystick) != DI_OK)	// c_dfDIJoystick might be incorrect
		return FALSE;

	if (joystick->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
		return FALSE;

	joystick->Acquire();

	return TRUE;
}

// The original name for this function is unknown
BOOL __stdcall EnumDevices_Callback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	static int already_ran;
	static DirectInputPair *directinput_objects;

	if ((already_ran & 1) == 0)
	{
		already_ran |= 1;
		directinput_objects = (DirectInputPair*)pvRef;
	}

	static LPDIRECTINPUTDEVICEA device;
	if (directinput_objects->lpDI->CreateDevice(lpddi->guidInstance, &device, NULL))
	{
		directinput_objects->device = NULL;
		return TRUE;
	}

	static LPDIRECTINPUTDEVICE2A _joystick;
	HRESULT res = device->QueryInterface(IID_IDirectInputDevice2A, (LPVOID*)&_joystick);

	if (res < 0)
	{
		joystick = NULL;
		return TRUE;
	}

	if (device != NULL)
	{
		device->Release();
		device = NULL;
	}

	directinput_objects->device = _joystick;

	char string[0x100];
	sprintf(string, "DeviceGUID = %lx\n", lpddi->guidInstance.Data1);
	OutputDebugStringA(string);

	return FALSE;
}

BOOL GetJoystickStatus(JOYSTICK_STATUS *status)
{
	DIJOYSTATE joystate;

	if (joystick == NULL)
		return FALSE;

	if (joystick->Poll())
		return FALSE;

	HRESULT res = joystick->GetDeviceState(sizeof(DIJOYSTATE), &joystate);
	if (res)
	{
		if (res == DIERR_INPUTLOST)
			SetDeviceAquire(0);
		else
			return FALSE;
	}

	for (int i = 0; i < 32; ++i)
	{
		if (joystate.rgbButtons[i] & 0x80)
			status->bButton[i] = TRUE;
		else
			status->bButton[i] = FALSE;
	}

	status->bDown = FALSE;
	status->bRight = FALSE;
	status->bUp = FALSE;
	status->bLeft = FALSE;

	if (joystate.lX < joystick_neutral_x - 10000)
		status->bLeft = TRUE;
	else if (joystate.lX > joystick_neutral_x + 10000)
		status->bRight = TRUE;

	if (joystate.lY < joystick_neutral_y - 10000)
		status->bUp = TRUE;
	else if (joystate.lY > joystick_neutral_y + 10000)
		status->bDown = TRUE;

	return TRUE;
}

BOOL ResetJoystickStatus(void)
{
	DIJOYSTATE joystate;

	if (joystick == NULL)
		return FALSE;

	if (joystick->Poll())
		return FALSE;

	HRESULT res = joystick->GetDeviceState(sizeof(DIJOYSTATE), &joystate);
	if (res)
	{
		if (res == DIERR_INPUTLOST)
			SetDeviceAquire(0);
		else
			return FALSE;
	}

	joystick_neutral_x = joystate.lX;
	joystick_neutral_y = joystate.lY;

	return TRUE;
}
