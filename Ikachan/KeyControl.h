#pragma once

enum KeyBind
{
	//The movement keys go in the order of left, right, up and down
	KEY_LEFT =      0x00000001,
	KEY_RIGHT =     0x00000002,
	KEY_UP =        0x00000004,
	KEY_DOWN =      0x00000008,
	// Swim and X button ig
	KEY_X =         0x00000010,
	KEY_Z =         0x00000020,
	//Left and right weapon switch keys
	KEY_S =         0x00000040,
	// Player 2 Swim and X buttons
	KEY_X_P2 =      0x00000080,
	KEY_Z_P2 =      0x00000100,
	// Player 2 Movement keys
	KEY_LEFT_P2 =   0x00000200,
	KEY_RIGHT_P2 =  0x00000400,
	KEY_UP_P2 =     0x00000800,
	KEY_DOWN_P2 =   0x00001000,
	KEY_SPACE =     0x00002000,
	//Escape key
	MOUSE_LEFT =    0x00004000,
	MOUSE_RIGHT =   0x00008000,
	KEY_F1 =        0x00010000,
	KEY_F2 =        0x00020000,
	KEY_ESCAPE =	0x00040000,
};

extern long gKey;
extern long gKeyTrg;
extern long gMouse;
extern long gMouseTrg;
extern long gMouseTrg2;

extern long lClick;
extern long rClick;

extern long gKeyOk;
extern long gKeyCancel;

extern long gKeySwim;
extern long gKeyMove;
extern long gKeyItem;

extern long gKeyLeft;
extern long gKeyUp;
extern long gKeyRight;
extern long gKeyDown;

extern long tKeySwim;
extern long tKeyMove;
extern long tKeyItem;

extern long tKeyLeft;
extern long tKeyUp;
extern long tKeyRight;
extern long tKeyDown;

void GetTrg();
