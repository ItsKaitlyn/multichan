#pragma once
#include "Draw.h"
#include "PiyoPiyo.h"
#include <windows.h>

#define MAX_PSLINES ((WINDOW_HEIGHT + 24) / 24)

struct PIX_SCR
{
	char* data;
	DWORD p_read;
	DWORD size;
	int ypos_line[MAX_PSLINES];
	DWORD line;
	char scale;
	BOOLEAN end;
};

BOOL ReadPixelScript(PIX_SCR *ptx, const char* path);
void LoadPixelScript(PIX_SCR *ptx, const char* path, char scale);
int PixelScriptProc(PIX_SCR *ptx, PIYOPIYO_CONTROL *piyocont, BOOL ending);
void EndPixelScript(PIX_SCR *ptx);
