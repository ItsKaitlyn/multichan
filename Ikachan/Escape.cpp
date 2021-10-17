#include "Escape.h"

#include "WindowsWrapper.h"

#include "Draw.h"
#include "Game.h"
#include "System.h"

int Call_Escape(HWND hWnd)
{
	RECT rc = {0, 0, 207, 16};

	while (1)
	{
		// Get pressed keys
		GetTrg();

		if (gKeyTrg & KEY_ESCAPE) // Escape is pressed, quit game
		{
			gKeyTrg = 0;
			return 0;
		}
		if (gKeyTrg & KEY_F1) // F1 is pressed, continue
		{
			gKeyTrg = 0;
			return 1;
		}
		if (gKeyTrg & KEY_F2) // F2 is pressed, reset
		{
			gKeyTrg = 0;
			return 2;
		}

		// Draw screen
		CortBox(&grcFull, 0x000000);
		PutBitmap3(&grcFull, (WINDOW_WIDTH - 207) / 2, (WINDOW_HEIGHT - 16) / 2, &rc, SURFACE_ID_ESCAPE_MENU);

		if (!Flip_SystemTask(hWnd))
		{
			// Quit if window is closed
			gKeyTrg = 0;
			return 0;
		}
	}

	return 0;
}
