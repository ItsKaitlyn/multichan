#include "Generic.h"
#include "Draw.h"
#include "Sound.h"
#include "PixelScript.h"
#include <stdio.h>

BOOL IsKeyFile(const char *name)
{
	char path[MAX_PATH];
	FILE *fp;

	sprintf(path, "%s\\%s", gModulePath, name);

	fp = fopen(path, "rb");

	if (fp == NULL)
		return FALSE;

	fclose(fp);
	return TRUE;
}

void GetCompileDate(int *year, int *month, int *day)
{
	int i;
	char strMonth[0x10];

	const char *table[13] = {
		"XXX",
		"Jan",
		"Feb",
		"Mar",
		"Apr",
		"May",
		"Jun",
		"Jul",
		"Aug",
		"Sep",
		"Oct",
		"Nov",
		"Dec",
	};

	sscanf("Mar  8 2006", "%s %d %d", strMonth, day, year);	//The expansion of __DATE__ is not reproductible. (TODO: Think about changing this to be reproductible)
	
	for (i = 0; i < 12; ++i)	//This being 12 instead of 13 might be a bug, but it works anyway by accident
		if (!memcmp(&strMonth, table[i], 3))
			break;
	
	*month = i;
}

BOOL GetCompileVersion(int *v1, int *v2, int *v3, int *v4)
{
	unsigned int puLen;
	VS_FIXEDFILEINFO *lpBuffer;
	DWORD dwHandle;
	DWORD dwLen;
	char path[MAX_PATH];
	LPVOID lpData;
	BOOL bResult;

	lpData = NULL;
	bResult = FALSE;

	GetModuleFileName(NULL, path, sizeof(path));
	dwLen = GetFileVersionInfoSize(path, &dwHandle);

	if (dwLen == 0)
		goto fail;

	lpData = malloc(dwLen);

	if (lpData == NULL)
		goto fail;

	if (!GetFileVersionInfo(path, 0, dwLen, lpData))
		goto fail;

	if (!VerQueryValueA(lpData, "\\", (LPVOID*)&lpBuffer, &puLen))
		goto fail;

	*v1 = (unsigned short)(lpBuffer->dwFileVersionMS >> 16);
	*v2 = (unsigned short)(lpBuffer->dwFileVersionMS & 0xFFFF);
	*v3 = (unsigned short)(lpBuffer->dwFileVersionLS >> 16);
	*v4 = (unsigned short)(lpBuffer->dwFileVersionLS & 0xFFFF);
	bResult = TRUE;

fail:

	if (lpData != NULL)
		free(lpData);

	return bResult;
}

BOOL OpenSoundVolume(HWND hWnd)
{
	//Get system directory...
	char path[MAX_PATH];
	GetSystemDirectory(path, sizeof(path));

	//Get first Sndvol path
	char path2[MAX_PATH];
	sprintf(path2, "%s\\Sndvol.exe", path);

	//Cut off the file path...
	size_t i = strlen(path);
	while (path[i] != '\\')
		--i;
	path[i] = '\0';

	//And... uhhh... get another path to try to open from?
	char path3[MAX_PATH];
	sprintf(path3, "%s\\Sndvol.exe", path);

	//Open Sndvol I guess?
	INT_PTR error1 = (INT_PTR)ShellExecute(hWnd, "open", path2, NULL, NULL, SW_SHOW);
	INT_PTR error2 = (INT_PTR)ShellExecute(hWnd, "open", path3, NULL, NULL, SW_SHOW);

	if (error1 <= 32 && error2 <= 32)
		return FALSE;
	else
		return TRUE;
}

void LoadGenericData()
{
	//Load surfaces
	// Player One loading
	MakeSurface_File("data\\Bmp\\PlayerOne\\OneChar.bmp", SURFACE_ID_MYCHAR);
	MakeSurface_File("data\\Bmp\\PlayerOne\\OneChar2.bmp", SURFACE_ID_MYCHAR2);
	MakeSurface_File("data\\Bmp\\PlayerOne\\OneChar3.bmp", SURFACE_ID_MYCHAR3);
	// Player Two loading
	MakeSurface_File("data\\Bmp\\PlayerTwo\\TwoChar.bmp", SURFACE_ID_TWOCHAR);
	MakeSurface_File("data\\Bmp\\PlayerTwo\\TwoChar2.bmp", SURFACE_ID_TWOCHAR2);
	MakeSurface_File("data\\Bmp\\PlayerTwo\\TwoChar3.bmp", SURFACE_ID_TWOCHAR3);
	// Other Files
	MakeSurface_File("data\\Bmp\\MsgBox.bmp", SURFACE_ID_MSGBOX);
	MakeSurface_File("data\\Bmp\\Editor\\Cursor.bmp", SURFACE_ID_CURSOR);
	MakeSurface_File("data\\Bmp\\Text\\YesNo.bmp", SURFACE_ID_YESNO);
	MakeSurface_File("data\\Bmp\\Text\\Status.bmp", SURFACE_ID_STATUS);
	MakeSurface_File("data\\Bmp\\Text\\StatusTwo.bmp", SURFACE_ID_STATUS_TWO);
	MakeSurface_File("data\\Bmp\\Text\\Figure.bmp", SURFACE_ID_FIGURE);
	MakeSurface_File("data\\Bmp\\Item.bmp", SURFACE_ID_ITEM);
	MakeSurface_File("data\\Bmp\\ItemBox.bmp", SURFACE_ID_ITEMBOX);
	MakeSurface_File("data\\Bmp\\Text\\Loading.bmp", SURFACE_ID_LOADING);
	MakeSurface_File("data\\Bmp\\Stage\\PrtBack.bmp", SURFACE_ID_PRTBACK);
	MakeSurface_File("data\\Bmp\\Stage\\PrtFilt.bmp", SURFACE_ID_PRTFILT);
	MakeSurface_File("data\\Bmp\\Stage\\PrtDir.bmp", SURFACE_ID_PRTDIR);
	MakeSurface_File("data\\Bmp\\Stage\\PrtBlock.bmp", SURFACE_ID_PRTBLOCK);
	MakeSurface_File("data\\Bmp\\Stage\\PrtDmg.bmp", SURFACE_ID_PRTDMG);
	MakeSurface_File("data\\Bmp\\Stage\\PrtItem.bmp", SURFACE_ID_PRTITEM);
	MakeSurface_File("data\\Bmp\\Stage\\PrtSnack.bmp", SURFACE_ID_PRTSNACK);
	MakeSurface_File("data\\Bmp\\Back.bmp", SURFACE_ID_BACK);
	MakeSurface_File("data\\Bmp\\Npc\\Hari.bmp", SURFACE_ID_HARI);
	MakeSurface_File("data\\Bmp\\Npc\\Isogin.bmp", SURFACE_ID_ISOGIN);
	MakeSurface_File("data\\Bmp\\Npc\\Kani.bmp", SURFACE_ID_KANI);
	MakeSurface_File("data\\Bmp\\Npc\\Sleep.bmp", SURFACE_ID_SLEEP);
	MakeSurface_File("data\\Bmp\\Npc\\Chibi.bmp", SURFACE_ID_CHIBI);
	MakeSurface_File("data\\Bmp\\Npc\\Hoshi.bmp", SURFACE_ID_HOSHI);
	MakeSurface_File("data\\Bmp\\Npc\\Dum.bmp", SURFACE_ID_DUM);
	MakeSurface_File("data\\Bmp\\Npc\\Carry.bmp", SURFACE_ID_CARRY);
	MakeSurface_File("data\\Bmp\\Npc\\Juel.bmp", SURFACE_ID_JUEL);
	MakeSurface_File("data\\Bmp\\Npc\\Ufo.bmp", SURFACE_ID_UFO);
	MakeSurface_File("data\\Bmp\\Npc\\Ironhead.bmp", SURFACE_ID_IRONHEAD);
	MakeSurface_File("data\\Bmp\\Star.bmp", SURFACE_ID_STAR);
	MakeSurface_File("data\\Bmp\\Bubble.bmp", SURFACE_ID_BUBBLE);
	MakeSurface_File("data\\Bmp\\Text\\Damage.bmp", SURFACE_ID_DAMAGE);
	MakeSurface_File("data\\Bmp\\Text\\LevelUp.bmp", SURFACE_ID_LEVELUP);
	MakeSurface_File("data\\Bmp\\Editor\\Editor.bmp", SURFACE_ID_EDITOR);
	MakeSurface_File("data\\Bmp\\Editor\\NpcType.bmp", SURFACE_ID_NPCTYPE);
	MakeSurface_File("data\\Bmp\\Opening.bmp", SURFACE_ID_OPENING);
	MakeSurface_File("data\\Bmp\\Art\\MaruAme.bmp", SURFACE_ID_MARUAME);
	MakeSurface_File("data\\Bmp\\Art\\Staff.bmp", SURFACE_ID_STAFF);
	MakeSurface_File("data\\Bmp\\Text\\End.bmp", SURFACE_ID_END);

	//Make generic surfaces
	MakeSurface_Generic(WINDOW_WIDTH, WINDOW_HEIGHT, SURFACE_ID_BACKUP);
	MakeSurface_Generic(272, 16, SURFACE_ID_TEXT0);
	MakeSurface_Generic(272, 16, SURFACE_ID_TEXT1);
	for (int i = 0; i < MAX_PSLINES; i++)
		MakeSurface_Generic(WINDOW_WIDTH, 16, i + SURFACE_ID_WORDS0);

	//Load sounds
	InitSoundObject("DASH", SOUND_ID_DASH);
	InitSoundObject("OUCH", SOUND_ID_OUCH);
	InitSoundObject("ITEM", SOUND_ID_ITEM);
	InitSoundObject("HITHEAD", SOUND_ID_HITHEAD);
	InitSoundObject("CRASH", SOUND_ID_CRASH);
	InitSoundObject("READY", SOUND_ID_READY);
	InitSoundObject("GO", SOUND_ID_GO);
	InitSoundObject("MESSAGE", SOUND_ID_MESSAGE);
	InitSoundObject("DEAD", SOUND_ID_DEAD);
	InitSoundObject("WIN", SOUND_ID_WIN);
	InitSoundObject("LEVELUP", SOUND_ID_LEVELUP);
	InitSoundObject("LIFEUP", SOUND_ID_LIFEUP);
	InitSoundObject("SAVE", SOUND_ID_SAVE);
	InitSoundObject("YESNO", SOUND_ID_YESNO);
	InitSoundObject("NODMG", SOUND_ID_NODMG);
	InitSoundObject("QUAKE", SOUND_ID_QUAKE);
	InitSoundObject("BOSSOUCH", SOUND_ID_BOSSOUCH);
	ChangeSoundFrequency(SOUND_ID_NODMG, 1200);
	ChangeSoundFrequency(SOUND_ID_QUAKE, 700);
	ChangeSoundFrequency(SOUND_ID_BOSSOUCH, 1200);
}