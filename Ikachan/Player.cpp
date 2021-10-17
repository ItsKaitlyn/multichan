#include "Player.h"
#include "Draw.h"
#include "System.h"
#include "Sound.h"
#include "EventScript.h"

MYCHAR gMC;

//Unit act functions
void ActMyChar_Normal(CARET *caret, CARET_SPAWNER *caret_spawner);
void ActMyChar_Dash(CARET *caret, CARET_SPAWNER *caret_spawner);
void ActMyChar_Ship(CARET *caret, CARET_SPAWNER *caret_spawner);

typedef void (*MYCHAR_ACT)(CARET*, CARET_SPAWNER*);
MYCHAR_ACT act[3] = {ActMyChar_Normal, ActMyChar_Dash, ActMyChar_Ship};

//Life and exp per level
short gMycLife[MAX_LEVEL + 1] = { 4, 8, 12, 18, 26, 34, 62 };
short gMycExp[MAX_LEVEL + 1] = { 8, 28, 52, 74, 102, 360, 852 };

void DamageMyChar(CARET_SPAWNER *caret_spawner, char damage)
{
	//Check if we're invulnerable
	if (gMC.shock == 0)
	{
		//Take damage
		gMC.shock = 100;
		gMC.life -= damage;
		if (gMC.life < 0)
			gMC.life = 0;
		
		//Show us how much damage we took
		int damage_i = FindCaretSpawner(caret_spawner);
		if (damage_i != NO_CARET)
		{
			CARET_SPAWNER *caretsp = &caret_spawner[damage_i];
			caretsp->cond = TRUE;
			caretsp->type = 2;
			caretsp->ani_no = 10 - damage;
			caretsp->num = 1;
			caretsp->x = gMC.x + 0x2000;
			caretsp->y = gMC.y - 0x1000;
			caretsp->rand_x = 1;
			caretsp->rand_y = 0;
		}
		
		if (gMC.life != 0)
		{
			//Play hurt sound
			PlaySoundObject(SOUND_ID_OUCH, SOUND_MODE_PLAY);
		}
		else
		{
			//Die
			PlaySoundObject(SOUND_ID_DEAD, SOUND_MODE_PLAY);
			gMC.cond = FALSE;
			gMC.dead = TRUE;
			
			//Create death effect
			int dead_i = FindCaretSpawner(caret_spawner);
			if (dead_i != NO_CARET)
			{
				CARET_SPAWNER *caretsp = &caret_spawner[dead_i];
				caretsp->cond = TRUE;
				caretsp->type = 0;
				caretsp->ani_no = 0;
				caretsp->num = 30;
				caretsp->x = gMC.x + 0x2000;
				caretsp->y = gMC.y + 0x2000;
				caretsp->rand_moveright = 0xC00;
				caretsp->rand_moveleft = -0xC00;
				caretsp->rand_movedown = 0x200;
				caretsp->rand_moveup = -0xC00;
				caretsp->rand_x = 1;
				caretsp->rand_y = 0;
			}
		}
	}
}

BYTE JudgeHitMyCharBlock(int x, int y, char flag)
{
	//Collide with block
	if ((flag & 1) && (flag & 2))
	{
		if ((gMC.x / 0x400) < (x * 16 + 15) && (gMC.y / 0x400) < (y * 16 + 12))
		{
			if (gMC.xm < -0x400)
				PlaySoundObject(SOUND_ID_HITHEAD, SOUND_MODE_PLAY);
			gMC.x = (x * 16 + 15) << 10;
			gMC.xm = 0;
			gMC.flag |= 1;
		}
		if ((gMC.y / 0x400) < (y * 16 + 15) && (gMC.x / 0x400) < (x * 16 + 12))
		{
			if (gMC.ym < -0x400)
				PlaySoundObject(SOUND_ID_HITHEAD, SOUND_MODE_PLAY);
			gMC.y = (y * 16 + 16) << 10;
			gMC.ym = 0;
			gMC.flag |= 2;
		}
	}
	
	if ((flag & 4) && (flag & 2))
	{
		if (((gMC.x + 0x3FF) / 0x400) > (x * 16 - 14) && (gMC.y / 0x400) < (y * 16 + 12))
		{
			if (gMC.xm < -0x400)
				PlaySoundObject(SOUND_ID_HITHEAD, SOUND_MODE_PLAY);
			gMC.x = (x * 16 - 14) << 10;
			gMC.xm = 0;
			gMC.flag |= 4;
		}
		if ((gMC.y / 0x400) < (y * 16 + 15) && (gMC.x / 0x400) > (x * 16 - 12))
		{
			if (gMC.ym < -0x400)
				PlaySoundObject(SOUND_ID_HITHEAD, SOUND_MODE_PLAY);
			gMC.y = (y * 16 + 16) << 10;
			gMC.ym = 0;
			gMC.flag |= 2;
		}
	}
	
	if ((flag & 1) && (flag & 8))
	{
		if ((gMC.x / 0x400) < (x * 16 + 15) && (gMC.y / 0x400) > (y * 16 - 12))
		{
			if (gMC.xm < -0x400)
				PlaySoundObject(SOUND_ID_HITHEAD, SOUND_MODE_PLAY);
			gMC.x = (x * 16 + 15) << 10;
			gMC.xm = 0;
			gMC.flag |= 1u;
		}
		if ((gMC.y / 0x400) >= (y * 16 - 16) && (gMC.x / 0x400) < (x * 16 + 12))
		{
			gMC.airborne = FALSE;
			gMC.y = (y * 16 - 16) << 10;
			if (gMC.ym > 0)
				gMC.ym = 0;
			gMC.flag |= 8;
		}
	}
	
	if ((flag & 4) && (flag & 8))
	{
		if (((gMC.x + 0x3FF) / 0x400) > (x * 16 - 14) && (gMC.y / 0x400) > (y * 16 - 12))
		{
			if (gMC.xm > 0x400)
				PlaySoundObject(SOUND_ID_HITHEAD, SOUND_MODE_PLAY);
			gMC.x = (x * 16 - 14) << 10;
			gMC.xm = 0;
			gMC.flag |= 4;
		}
		if ((gMC.y / 0x400) >= (y * 16 - 16) && (gMC.x / 0x400) > (x * 16 - 12))
		{
			gMC.airborne = FALSE;
			gMC.y = (y * 16 - 16) << 10;
			if (gMC.ym > 0)
				gMC.ym = 0;
			gMC.flag |= 8;
		}
	}
	return gMC.flag;
}

BYTE JudgeHitMyCharSnack(int x, int y, BYTE flag, CARET_SPAWNER *caret_spawner, MAP *map)
{
	//Reset collision flag for some reason
	gMC.flag = 0;
	
	//Collide with block
	if ((flag & 1) && (flag & 2))
	{
		if ((gMC.x / 0x400) < (x * 16 + 15) && (gMC.y / 0x400) < (y * 16 + 12))
		{
			if (gMC.unit != 1)
				gMC.xm = 0;
			gMC.x = (x * 16 + 15) << 10;
			gMC.flag |= 1;
		}
		if ((gMC.y / 0x400) < (y * 16 + 15) && (gMC.x / 0x400) < (x * 16 + 12))
		{
			if (gMC.unit != 1)
				gMC.ym = 0;
			gMC.y = (y * 16 + 16) << 10;
			gMC.flag |= 2;
		}
	}
	
	if ((flag & 4) && (flag & 2))
	{
		if (((gMC.x + 0x3FF) / 0x400) > (x * 16 - 14) && (gMC.y / 0x400) < (y * 16 + 12))
		{
			if (gMC.unit != 1)
				gMC.xm = 0;
			gMC.x = (x * 16 - 14) << 10;
			gMC.flag |= 4;
		}
		if ((gMC.y / 0x400) < (y * 16 + 15) && (gMC.x / 0x400) > (x * 16 - 12))
		{
			if (gMC.unit != 1)
				gMC.ym = 0;
			gMC.y = (y * 16 + 16) << 10;
			gMC.flag |= 2;
		}
	}
	
	if ((flag & 1) && (flag & 8))
	{
		if ((gMC.x / 0x400) < (x * 16 + 15) && (gMC.y / 0x400) > (y * 16 - 12))
		{
			if (gMC.unit != 1)
				gMC.xm = 0;
			gMC.x = (x * 16 + 15) << 10;
			gMC.flag |= 1u;
		}
		if ((gMC.y / 0x400) >= (y * 16 - 16) && (gMC.x / 0x400) < (x * 16 + 12))
		{
			if (gMC.unit != 1)
			{
				gMC.airborne = FALSE;
				if (gMC.ym > 0)
					gMC.ym = 0;
			}
			gMC.y = (y * 16 - 16) << 10;
			gMC.flag |= 8;
		}
	}
	
	if ((flag & 4) && (flag & 8))
	{
		if (((gMC.x + 0x3FF) / 0x400) > (x * 16 - 14) && (gMC.y / 0x400) > (y * 16 - 12))
		{
			if (gMC.unit != 1)
				gMC.xm = 0;
			gMC.x = (x * 16 - 14) << 10;
			gMC.flag |= 4;
		}
		if ((gMC.y / 0x400) >= (y * 16 - 16) && (gMC.x / 0x400) > (x * 16 - 12))
		{
			if (gMC.unit != 1)
			{
				gMC.airborne = FALSE;
				if (gMC.ym > 0)
					gMC.ym = 0;
			}
			gMC.y = (y * 16 - 16) << 10;
			gMC.flag |= 8;
		}
	}
	
	//Break block
	if ((gMC.unit == 1 && gMC.flag != 8 && gMC.flag) || ((gMC.equip & 1) && (gMC.flag & 2)))
	{
		//Destroy block
		PlaySoundObject(SOUND_ID_CRASH, SOUND_MODE_PLAY);
		map->data[x + map->width * y] = 0;
		
		//Create bubbles
		int bubble_i = FindCaretSpawner(caret_spawner);
		if (bubble_i != NO_CARET)
		{
			CARET_SPAWNER *caretsp = &caret_spawner[bubble_i];
			caretsp->cond = TRUE;
			caretsp->type = 1;
			caretsp->ani_no = 0;
			caretsp->num = 5;
			caretsp->x = (x * 16 + 8) << 10;
			caretsp->y = (y * 16 + 8) << 10;
			caretsp->rand_moveright = 0x400;
			caretsp->rand_moveleft = -0x400;
			caretsp->rand_movedown = 0x400;
			caretsp->rand_moveup = -0x400;
			caretsp->rand_x = 8;
			caretsp->rand_y = 8;
		}
	}
	return gMC.flag;
}

void JudgeHitMyCharDamage(int x, int y, CARET_SPAWNER *caret_spawner)
{
	if ((gMC.x / 0x400) < (x * 16 + 10) &&
		(gMC.x / 0x400) > (x * 16 - 10) &&
		(gMC.y / 0x400) < (y * 16 + 10) &&
		(gMC.y / 0x400) > (y * 16 - 10))
	{
		//Take damage
		DamageMyChar(caret_spawner, 3);
	}
}

void JudgeHitMyCharVector(int x, int y, BYTE type)
{
	if ((gMC.x / 0x400) < (x * 16 + 8) &&
		(gMC.x / 0x400) >= (x * 16 - 8) &&
		(gMC.y / 0x400) < (y * 16 + 8) &&
		(gMC.y / 0x400) >= (y * 16 - 8))
	{
		switch (type)
		{
			case 0x60:
				gMC.xm -= 50;
				break;
			case 0x61:
				gMC.xm += 50;
				break;
			case 0x62:
				gMC.ym -= 50;
				break;
			case 0x63:
				gMC.ym += 50;
				break;
			default:
				return;
		}
	}
}

void JudgeHitMyCharItem(int x, int y, CARET_SPAWNER *caret_spawner, MAP *map)
{
	if ((gMC.x / 0x400) < (x * 16 + 8) &&
		(gMC.x / 0x400) > (x * 16 - 8) &&
		(gMC.y / 0x400) < (y * 16 + 8) &&
		(gMC.y / 0x400) > (y * 16 - 8))
	{
		//Remove item, play item sound, add exp and life
		map->data[x + map->width * y] = 0;
		PlaySoundObject(SOUND_ID_ITEM, SOUND_MODE_PLAY);
		gMC.life++;
		gMC.exp++;
		if (gMC.life > gMycLife[gMC.level])
			gMC.life = gMycLife[gMC.level];
		
		//Create '+1' experience indicator
		int exp_i = FindCaretSpawner(caret_spawner);
		if (exp_i != NO_CARET)
		{
			CARET_SPAWNER *caretsp = &caret_spawner[exp_i];
			caretsp->cond = TRUE;
			caretsp->type = 2;
			caretsp->ani_no = 11;
			caretsp->num = 1;
			caretsp->x = gMC.x + 0x2000;
			caretsp->y = gMC.y - 0x1000;
			caretsp->rand_x = 1;
			caretsp->rand_y = 0;
		}
		
		//Create stars
		int star_i = FindCaretSpawner(caret_spawner);
		if (star_i != NO_CARET)
		{
			CARET_SPAWNER *caretsp = &caret_spawner[star_i];
			caretsp->cond = TRUE;
			caretsp->type = 0;
			caretsp->ani_no = 0;
			caretsp->num = 4;
			caretsp->x = gMC.x + 0x2000;
			caretsp->y = gMC.y + 0x2000;
			caretsp->rand_moveright = 0x800;
			caretsp->rand_moveleft = -0x800;
			caretsp->rand_movedown = 0;
			caretsp->rand_moveup = -0x800;
			caretsp->rand_x = 1;
			caretsp->rand_y = 0;
		}
	}
}

void HitMyCharMap(MAP *map, CARET_SPAWNER *caret_spawner)
{
	//Collision offsets and flags
	char offx[4];
	char offy[4];
	BOOLEAN flag1[4];
	BYTE flag2[4];
	
	offx[0] = 0;
	offx[1] = 1;
	offx[2] = 0;
	offx[3] = 1;
	
	offy[0] = 0;
	offy[1] = 0;
	offy[2] = 1;
	offy[3] = 1;
	
	flag1[0] = 0;
	flag1[1] = 0;
	flag1[2] = 1;
	flag1[3] = 1;
	
	flag2[0] = 1 | 2;
	flag2[1] = 4 | 2;
	flag2[2] = 8 | 1;
	flag2[3] = 8 | 4;
	
	//Collision position
	int x = gMC.x / 0x400 / 16;
	int y = gMC.y / 0x400 / 16;
	
	//Reset collision state
	signed char v19 = 4;
	gMC.flag = 0;
	
	for (int i = 0; i < 4; i++)
	{
		//Get tile at the position to check
		int part = map->data[(x + offx[i]) + map->width * (y + offy[i])];
		
		//Block collision
		if (part < 0x80 || part >= 0xA0)
			v19 -= flag1[i];
		else if ((JudgeHitMyCharBlock(x + offx[i], y + offy[i], flag2[i]) & 8) == 0)
			v19 -= flag1[i];
		//Vector collision
		if (part >= 0x60 && part < 0x80)
			JudgeHitMyCharVector(x + offx[i], y + offy[i], part);
		//Damage collision
		if (part >= 0xA0 && part < 0xC0)
			JudgeHitMyCharDamage(x + offx[i], y + offy[i], caret_spawner);
		//Item collision
		else if (part >= 0x40 && part < 0x60)
			JudgeHitMyCharItem(x + offx[i], y + offy[i], caret_spawner, map);
		//Snack collision
		else if (part < 0xE0 || part >= 0x100)
			v19 -= flag1[i];
		else if ((JudgeHitMyCharSnack(x + offx[i], y + offy[i], flag2[i], caret_spawner, map) & 8) == 0)
			v19 -= flag1[i];
	}
	
	//Check if we should level up
	if (gMC.exp >= gMycExp[gMC.level])
	{
		//Level up
		gMC.exp -= gMycExp[gMC.level];
		++gMC.level;
		if (gMC.level > MAX_LEVEL)
			gMC.level = MAX_LEVEL;
		else
			PlaySoundObject(SOUND_ID_LEVELUP, SOUND_MODE_PLAY);
		
		//Create 'Level Up' text
		int levup_i = FindCaretSpawner(caret_spawner);
		if (levup_i != NO_CARET)
		{
			CARET_SPAWNER *caretsp = &caret_spawner[levup_i];
			caretsp->cond = TRUE;
			caretsp->type = 3;
			caretsp->ani_no = 0;
			caretsp->num = 1;
			caretsp->x = gMC.x + 0x2000;
			caretsp->y = gMC.y - 0x1000;
			caretsp->rand_x = 1;
			caretsp->rand_y = 0;
		}
	}
	
	//Set airborne flag
	if (v19 <= 0)
		gMC.airborne = TRUE;
}

//Draw MyChar
void PutMyChar(FRAME *frame)
{
	static RECT rcMyChar[12] = {
		{  0,  0, 16, 16 },
		{ 16,  0, 32, 16 },
		{ 32,  0, 48, 16 },
		{ 48,  0, 64, 16 },

		{  0, 16, 16, 32 },
		{ 16, 16, 32, 32 },
		{ 32, 16, 48, 32 },
		{ 48, 16, 64, 32 },

		{  0, 32, 16, 48 },
		{ 16, 32, 32, 48 },
		{ 32, 32, 48, 48 },
		{ 48, 32, 64, 48 },
	};

	static RECT rcMyCharShip[12] = {
		{   0,  0,  40,  40 },
		{  40,  0,  80,  40 },
		{  80,  0, 120,  40 },
		{ 120,  0, 160,  40 },

		{   0, 40,  40,  80 },
		{  40, 40,  80,  80 },
		{  80, 40, 120,  80 },
		{ 120, 40, 160,  80 },

		{   0, 80,  40, 120 },
		{  40, 80,  80, 120 },
		{  80, 80, 120, 120 },
		{ 120, 80, 160, 120 },
	};
	
	BYTE frame_no = (gMC.direct * 4) + gMC.ani_no;
	if (gMC.equip & 8)
		PutBitmap3(&grcFull, (gMC.x / 0x400) - (frame->x / 0x400) - 12, (gMC.y / 0x400) - (frame->y / 0x400) - 12, &rcMyCharShip[frame_no], SURFACE_ID_MYCHAR3);
	else if (gMC.equip & 1)
		PutBitmap3(&grcFull, (gMC.x / 0x400) - (frame->x / 0x400), (gMC.y / 0x400) - (frame->y / 0x400), &rcMyChar[frame_no], SURFACE_ID_MYCHAR);
	else
		PutBitmap3(&grcFull, (gMC.x / 0x400) - (frame->x / 0x400), (gMC.y / 0x400) - (frame->y / 0x400), &rcMyChar[frame_no], SURFACE_ID_MYCHAR2);
}

void PutMyStatus()
{
	RECT rcStatus = { 0, 0, 88, 32 };
	PutBitmap3(&grcFull, 8, 8, &rcStatus, SURFACE_ID_STATUS);
	PutNumber(16, 8, gMC.level);
	PutNumber(16, 16, gMC.exp);
	PutNumber(48, 16, gMycExp[gMC.level]);
	PutNumber(16, 24, gMC.life);
	PutNumber(48, 24, gMycLife[gMC.level]);
}

//Update MyChar
int swim_xm[3] = { -0x200, 0x200, 0 };
int swim_ym[3] = { -0x200, -0x200, -0x2D4 };

int dash_xm[3] = { -0xC00, 0xC00, 0 };
int dash_ym[3] = { 0, 0, -0xC00 };

void ActMyChar_Normal(CARET *caret, CARET_SPAWNER *caret_spawner)
{
	//Get direction
	gMC.direct = 2;
	if (gKey & KEY_LEFT)
		gMC.direct = 0;
	if (gKey & KEY_RIGHT)
		gMC.direct = 1;

	//Swim
	if ((gKeyTrg & KEY_Z) && gMC.swim_wait == 0)
	{
		//Swim sound and bubble
		int caret_i = FindCaret(caret);
		if (caret_i != NO_CARET)
		{
			PlaySoundObject(1, 1);
			caret += caret_i; //... What the hell?
			caret->type = 1;
			caret->xm = -swim_xm[gMC.direct];
			caret->ym = -swim_ym[gMC.direct];
			caret->cond = TRUE;
			caret->ani_no = 0;
			caret->ani_wait = 0;
			switch (gMC.direct)
			{
				case 0:
					caret->x = gMC.x + 14336;
					caret->y = gMC.y + 12288;
					break;
				case 1:
					caret->x = gMC.x + 2048;
					caret->y = gMC.y + 12288;
					break;
				case 2:
					caret->x = gMC.x + 9216;
					caret->y = gMC.y + 14336;
					break;
			}
		}

		//Play animation and disable for 8 frames
		gMC.swim_wait = 8;
		gMC.ani_no = 1;
		if (gMC.ani_wait < 100)
			gMC.ani_wait += 10;

		//Swim velocity
		gMC.xm += swim_xm[gMC.direct];
		gMC.ym += swim_ym[gMC.direct];
	}

	//Dash
	if ((gKey & KEY_Z) && (gMC.equip & 2))
	{
		//Play charged sound
		if (gMC.dash_wait == 31)
			PlaySoundObject(SOUND_ID_READY, SOUND_MODE_PLAY);

		//Charge dash
		if (++gMC.dash_wait > 32)
			gMC.dash_wait = 32;
	}
	else if (gMC.dash_wait == 32)
	{
		//Play dash sound
		PlaySoundObject(SOUND_ID_GO, SOUND_MODE_PLAY);

		//Dash bubble
		int caret_i = FindCaret(caret);
		if (caret_i != NO_CARET)
		{
			CARET *caretp = &caret[caret_i];
			caretp->type = 1;
			caretp->xm = dash_xm[gMC.direct] / -8;
			caretp->ym = dash_ym[gMC.direct] / -8;
			caretp->cond = TRUE;
			caretp->ani_no = 0;
			caretp->ani_wait = 0;
			switch (gMC.direct)
			{
				case 0:
					caretp->x = gMC.x + 14336;
					caretp->y = gMC.y + 12288;
					break;
				case 1:
					caretp->x = gMC.x + 2048;
					caretp->y = gMC.y + 12288;
					break;
				case 2:
					caretp->x = gMC.x + 9216;
					caretp->y = gMC.y + 14336;
					break;
			}
		}

		//Dash velocity and set to dash mode
		gMC.xm = dash_xm[gMC.direct];
		gMC.ym = dash_ym[gMC.direct];
		gMC.unit = 1;
	}
	else
	{
		gMC.dash_wait = 0;
	}
	
	//Animation timer
	if (gMC.ani_wait <= 0)
		gMC.ani_no = 0;
	else
		--gMC.ani_wait;

	//Swim timer
	if (gMC.swim_wait > 0)
		--gMC.swim_wait;

	//Dash animation
	if (gMC.dash_wait == 32)
		gMC.ani_no = 2;
	
	//Gravity
	if (gMC.ym < 0x800)
		gMC.ym += 20;
	
	//Speed limit
	if (gMC.ym < -0x800)
		gMC.ym = -0x800;
	if (gMC.ym > 0x800)
		gMC.ym = 0x800;
	if (gMC.xm < -0x800)
		gMC.xm = -0x800;
	if (gMC.xm > 0x800)
		gMC.xm = 0x800;
	
	//Friction
	if (gMC.xm > 0)
		gMC.xm -= 8;
	if (gMC.xm < 0)
		gMC.xm += 8;
	
	if (!gMC.airborne)
	{
		if (gMC.xm > 0)
			gMC.xm -= 24;
		if (gMC.xm < 0)
			gMC.xm += 24;
	}
	
	//Move
	if (gMC.xm >= 8 || gMC.xm <= -8)
		gMC.x += gMC.xm;
	gMC.y += gMC.ym;

	//Decrement timers
	if (gMC.shock)
		--gMC.shock;
	if (gMC.no_event)
		--gMC.no_event;
}

void ActMyChar_Dash(CARET *caret, CARET_SPAWNER *caret_spawner)
{
	//Decrease dash timer and stop dashing when depleted
	if (--gMC.dash_wait <= 0)
		gMC.unit = 0;
	
	//Dash bubble
	if ((gMC.dash_wait % 4) == 0)
	{
		int caret_i = FindCaret(caret);
		if (caret_i != NO_CARET)
		{
			CARET *caretp = &caret[caret_i];
			caretp->type = 1;
			caretp->xm = (dash_xm[gMC.direct] / -8) + Random(-0x200, 0x200);
			caretp->ym = (dash_ym[gMC.direct] / -8) + Random(-0x200, 0x200);
			caretp->cond = TRUE;
			caretp->ani_no = 0;
			caretp->ani_wait = 0;
			caretp->x = gMC.x + 0x2000;
			caretp->y = gMC.y + 0x2000;
		}
	}
	
	//Move and use dash animation
	gMC.x += gMC.xm;
	gMC.y += gMC.ym;
	gMC.ani_no = 3;

	//Decrement timers
	if (gMC.shock)
		--gMC.shock;
}

void ActMyChar_Ship(CARET *caret, CARET_SPAWNER *caret_spawner)
{
	//Create effect
	int caretsp_i = FindCaretSpawner(caret_spawner);
	if (caretsp_i != NO_CARET)
	{
		CARET_SPAWNER *caretsp = &caret_spawner[caretsp_i];
		caretsp->cond = TRUE;
		caretsp->type = 0;
		caretsp->ani_no = 0;
		caretsp->num = 1;
		caretsp->x = gMC.x + 0x2000;
		caretsp->y = gMC.y + 0x6000;
		caretsp->rand_moveright = 0xC00;
		caretsp->rand_moveleft = -0xC00;
		caretsp->rand_movedown = 0xC00;
		caretsp->rand_moveup = 0;
		caretsp->rand_x = 1;
		caretsp->rand_y = 0;
	}
	
	//Fly up
	gMC.xm = 0;
	gMC.ym -= 16;
	gMC.y += gMC.ym;
	gMC.ani_no = 3;
	if (gMC.shock != 0)
		gMC.shock = 0;
}

void ActMyChar(CARET *caret, CARET_SPAWNER *caret_spawner)
{
	act[gMC.unit](caret, caret_spawner);
}

//Initalize MyChar
void InitMyChar()
{
	gMC.cond = TRUE;
	gMC.equip = 0;
	gMC.dead = 0;
	gMC.level = 0;
	gMC.life = gMycLife[0];
	gMC.exp = 0;
	gMC.x = 0xA0000;
	gMC.y = 0x1A0000;
	gMC.ym = 0;
	gMC.xm = 0;
	gMC.airborne = 1;
	gMC.ani_wait = 0;
	gMC.ani_no = 0;
	gMC.direct = 0;
	gMC.flag = 0;
	gMC.unit = 0;
	gMC.shock = 0;
	gMC.no_event = 100;
	gMC.dash_wait = 0;
	gMC.swim_wait = 0;
	gMC.carry = 0;
}

// Player Two

TWOCHAR tMC;

//Unit act functions
void ActTwoChar_Normal(CARET *caret, CARET_SPAWNER *caret_spawner);
void ActTwoChar_Dash(CARET *caret, CARET_SPAWNER *caret_spawner);
void ActTwoChar_Ship(CARET *caret, CARET_SPAWNER *caret_spawner);

typedef void (*TWOCHAR_ACT)(CARET*, CARET_SPAWNER*);
TWOCHAR_ACT two_act[3] = {ActTwoChar_Normal, ActTwoChar_Dash, ActTwoChar_Ship};

//Life and exp per level
short tMycLife[MAX_LEVEL + 1] = { 4, 8, 12, 18, 26, 34, 62 };
short tMycExp[MAX_LEVEL + 1] = { 8, 28, 52, 74, 102, 360, 852 };

void DamageTwoChar(CARET_SPAWNER *caret_spawner, char damage)
{
	//Check if we're invulnerable
	if (tMC.shock == 0)
	{
		//Take damage
		tMC.shock = 100;
		tMC.life -= damage;
		if (tMC.life < 0)
			tMC.life = 0;
		
		//Show us how much damage we took
		int damage_i = FindCaretSpawner(caret_spawner);
		if (damage_i != NO_CARET)
		{
			CARET_SPAWNER *caretsp = &caret_spawner[damage_i];
			caretsp->cond = TRUE;
			caretsp->type = 2;
			caretsp->ani_no = 10 - damage;
			caretsp->num = 1;
			caretsp->x = tMC.x + 0x2000;
			caretsp->y = tMC.y - 0x1000;
			caretsp->rand_x = 1;
			caretsp->rand_y = 0;
		}
		
		if (tMC.life != 0)
		{
			//Play hurt sound
			PlaySoundObject(SOUND_ID_OUCH, SOUND_MODE_PLAY);
		}
		else
		{
			//Die
			PlaySoundObject(SOUND_ID_DEAD, SOUND_MODE_PLAY);
			tMC.cond = FALSE;
			tMC.dead = TRUE;
			
			//Create death effect
			int dead_i = FindCaretSpawner(caret_spawner);
			if (dead_i != NO_CARET)
			{
				CARET_SPAWNER *caretsp = &caret_spawner[dead_i];
				caretsp->cond = TRUE;
				caretsp->type = 0;
				caretsp->ani_no = 0;
				caretsp->num = 30;
				caretsp->x = tMC.x + 0x2000;
				caretsp->y = tMC.y + 0x2000;
				caretsp->rand_moveright = 0xC00;
				caretsp->rand_moveleft = -0xC00;
				caretsp->rand_movedown = 0x200;
				caretsp->rand_moveup = -0xC00;
				caretsp->rand_x = 1;
				caretsp->rand_y = 0;
			}
		}
	}
}

BYTE JudgeHitTwoCharBlock(int x, int y, char flag)
{
	//Collide with block
	if ((flag & 1) && (flag & 2))
	{
		if ((tMC.x / 0x400) < (x * 16 + 15) && (tMC.y / 0x400) < (y * 16 + 12))
		{
			if (tMC.xm < -0x400)
				PlaySoundObject(SOUND_ID_HITHEAD, SOUND_MODE_PLAY);
			tMC.x = (x * 16 + 15) << 10;
			tMC.xm = 0;
			tMC.flag |= 1;
		}
		if ((tMC.y / 0x400) < (y * 16 + 15) && (tMC.x / 0x400) < (x * 16 + 12))
		{
			if (tMC.ym < -0x400)
				PlaySoundObject(SOUND_ID_HITHEAD, SOUND_MODE_PLAY);
			tMC.y = (y * 16 + 16) << 10;
			tMC.ym = 0;
			tMC.flag |= 2;
		}
	}
	
	if ((flag & 4) && (flag & 2))
	{
		if (((tMC.x + 0x3FF) / 0x400) > (x * 16 - 14) && (tMC.y / 0x400) < (y * 16 + 12))
		{
			if (tMC.xm < -0x400)
				PlaySoundObject(SOUND_ID_HITHEAD, SOUND_MODE_PLAY);
			tMC.x = (x * 16 - 14) << 10;
			tMC.xm = 0;
			tMC.flag |= 4;
		}
		if ((tMC.y / 0x400) < (y * 16 + 15) && (tMC.x / 0x400) > (x * 16 - 12))
		{
			if (tMC.ym < -0x400)
				PlaySoundObject(SOUND_ID_HITHEAD, SOUND_MODE_PLAY);
			tMC.y = (y * 16 + 16) << 10;
			tMC.ym = 0;
			tMC.flag |= 2;
		}
	}
	
	if ((flag & 1) && (flag & 8))
	{
		if ((tMC.x / 0x400) < (x * 16 + 15) && (tMC.y / 0x400) > (y * 16 - 12))
		{
			if (tMC.xm < -0x400)
				PlaySoundObject(SOUND_ID_HITHEAD, SOUND_MODE_PLAY);
			tMC.x = (x * 16 + 15) << 10;
			tMC.xm = 0;
			tMC.flag |= 1u;
		}
		if ((tMC.y / 0x400) >= (y * 16 - 16) && (tMC.x / 0x400) < (x * 16 + 12))
		{
			tMC.airborne = FALSE;
			tMC.y = (y * 16 - 16) << 10;
			if (tMC.ym > 0)
				tMC.ym = 0;
			tMC.flag |= 8;
		}
	}
	
	if ((flag & 4) && (flag & 8))
	{
		if (((tMC.x + 0x3FF) / 0x400) > (x * 16 - 14) && (tMC.y / 0x400) > (y * 16 - 12))
		{
			if (tMC.xm > 0x400)
				PlaySoundObject(SOUND_ID_HITHEAD, SOUND_MODE_PLAY);
			tMC.x = (x * 16 - 14) << 10;
			tMC.xm = 0;
			tMC.flag |= 4;
		}
		if ((tMC.y / 0x400) >= (y * 16 - 16) && (tMC.x / 0x400) > (x * 16 - 12))
		{
			tMC.airborne = FALSE;
			tMC.y = (y * 16 - 16) << 10;
			if (tMC.ym > 0)
				tMC.ym = 0;
			tMC.flag |= 8;
		}
	}
	return tMC.flag;
}

BYTE JudgeHitTwoCharSnack(int x, int y, BYTE flag, CARET_SPAWNER *caret_spawner, MAP *map)
{
	//Reset collision flag for some reason
	tMC.flag = 0;
	
	//Collide with block
	if ((flag & 1) && (flag & 2))
	{
		if ((tMC.x / 0x400) < (x * 16 + 15) && (tMC.y / 0x400) < (y * 16 + 12))
		{
			if (tMC.unit != 1)
				tMC.xm = 0;
			tMC.x = (x * 16 + 15) << 10;
			tMC.flag |= 1;
		}
		if ((tMC.y / 0x400) < (y * 16 + 15) && (tMC.x / 0x400) < (x * 16 + 12))
		{
			if (tMC.unit != 1)
				tMC.ym = 0;
			tMC.y = (y * 16 + 16) << 10;
			tMC.flag |= 2;
		}
	}
	
	if ((flag & 4) && (flag & 2))
	{
		if (((tMC.x + 0x3FF) / 0x400) > (x * 16 - 14) && (tMC.y / 0x400) < (y * 16 + 12))
		{
			if (tMC.unit != 1)
				tMC.xm = 0;
			tMC.x = (x * 16 - 14) << 10;
			tMC.flag |= 4;
		}
		if ((tMC.y / 0x400) < (y * 16 + 15) && (tMC.x / 0x400) > (x * 16 - 12))
		{
			if (tMC.unit != 1)
				tMC.ym = 0;
			tMC.y = (y * 16 + 16) << 10;
			tMC.flag |= 2;
		}
	}
	
	if ((flag & 1) && (flag & 8))
	{
		if ((tMC.x / 0x400) < (x * 16 + 15) && (tMC.y / 0x400) > (y * 16 - 12))
		{
			if (tMC.unit != 1)
				tMC.xm = 0;
			tMC.x = (x * 16 + 15) << 10;
			tMC.flag |= 1u;
		}
		if ((tMC.y / 0x400) >= (y * 16 - 16) && (tMC.x / 0x400) < (x * 16 + 12))
		{
			if (tMC.unit != 1)
			{
				tMC.airborne = FALSE;
				if (tMC.ym > 0)
					tMC.ym = 0;
			}
			tMC.y = (y * 16 - 16) << 10;
			tMC.flag |= 8;
		}
	}
	
	if ((flag & 4) && (flag & 8))
	{
		if (((tMC.x + 0x3FF) / 0x400) > (x * 16 - 14) && (tMC.y / 0x400) > (y * 16 - 12))
		{
			if (tMC.unit != 1)
				tMC.xm = 0;
			tMC.x = (x * 16 - 14) << 10;
			tMC.flag |= 4;
		}
		if ((tMC.y / 0x400) >= (y * 16 - 16) && (tMC.x / 0x400) > (x * 16 - 12))
		{
			if (tMC.unit != 1)
			{
				tMC.airborne = FALSE;
				if (tMC.ym > 0)
					tMC.ym = 0;
			}
			tMC.y = (y * 16 - 16) << 10;
			tMC.flag |= 8;
		}
	}
	
	//Break block
	if ((tMC.unit == 1 && tMC.flag != 8 && tMC.flag) || ((tMC.equip & 1) && (tMC.flag & 2)))
	{
		//Destroy block
		PlaySoundObject(SOUND_ID_CRASH, SOUND_MODE_PLAY);
		map->data[x + map->width * y] = 0;
		
		//Create bubbles
		int bubble_i = FindCaretSpawner(caret_spawner);
		if (bubble_i != NO_CARET)
		{
			CARET_SPAWNER *caretsp = &caret_spawner[bubble_i];
			caretsp->cond = TRUE;
			caretsp->type = 1;
			caretsp->ani_no = 0;
			caretsp->num = 5;
			caretsp->x = (x * 16 + 8) << 10;
			caretsp->y = (y * 16 + 8) << 10;
			caretsp->rand_moveright = 0x400;
			caretsp->rand_moveleft = -0x400;
			caretsp->rand_movedown = 0x400;
			caretsp->rand_moveup = -0x400;
			caretsp->rand_x = 8;
			caretsp->rand_y = 8;
		}
	}
	return tMC.flag;
}

void JudgeHitTwoCharDamage(int x, int y, CARET_SPAWNER *caret_spawner)
{
	if ((tMC.x / 0x400) < (x * 16 + 10) &&
		(tMC.x / 0x400) > (x * 16 - 10) &&
		(tMC.y / 0x400) < (y * 16 + 10) &&
		(tMC.y / 0x400) > (y * 16 - 10))
	{
		//Take damage
		DamageTwoChar(caret_spawner, 3);
	}
}

void JudgeHitTwoCharVector(int x, int y, BYTE type)
{
	if ((tMC.x / 0x400) < (x * 16 + 8) &&
		(tMC.x / 0x400) >= (x * 16 - 8) &&
		(tMC.y / 0x400) < (y * 16 + 8) &&
		(tMC.y / 0x400) >= (y * 16 - 8))
	{
		switch (type)
		{
			case 0x60:
				tMC.xm -= 50;
				break;
			case 0x61:
				tMC.xm += 50;
				break;
			case 0x62:
				tMC.ym -= 50;
				break;
			case 0x63:
				tMC.ym += 50;
				break;
			default:
				return;
		}
	}
}

void JudgeHitTwoCharItem(int x, int y, CARET_SPAWNER *caret_spawner, MAP *map)
{
	if ((tMC.x / 0x400) < (x * 16 + 8) &&
		(tMC.x / 0x400) > (x * 16 - 8) &&
		(tMC.y / 0x400) < (y * 16 + 8) &&
		(tMC.y / 0x400) > (y * 16 - 8))
	{
		//Remove item, play item sound, add exp and life
		map->data[x + map->width * y] = 0;
		PlaySoundObject(SOUND_ID_ITEM, SOUND_MODE_PLAY);
		tMC.life++;
		tMC.exp++;
		if (tMC.life > tMycLife[tMC.level])
			tMC.life = tMycLife[tMC.level];
		
		//Create '+1' experience indicator
		int exp_i = FindCaretSpawner(caret_spawner);
		if (exp_i != NO_CARET)
		{
			CARET_SPAWNER *caretsp = &caret_spawner[exp_i];
			caretsp->cond = TRUE;
			caretsp->type = 2;
			caretsp->ani_no = 11;
			caretsp->num = 1;
			caretsp->x = tMC.x + 0x2000;
			caretsp->y = tMC.y - 0x1000;
			caretsp->rand_x = 1;
			caretsp->rand_y = 0;
		}
		
		//Create stars
		int star_i = FindCaretSpawner(caret_spawner);
		if (star_i != NO_CARET)
		{
			CARET_SPAWNER *caretsp = &caret_spawner[star_i];
			caretsp->cond = TRUE;
			caretsp->type = 0;
			caretsp->ani_no = 0;
			caretsp->num = 4;
			caretsp->x = tMC.x + 0x2000;
			caretsp->y = tMC.y + 0x2000;
			caretsp->rand_moveright = 0x800;
			caretsp->rand_moveleft = -0x800;
			caretsp->rand_movedown = 0;
			caretsp->rand_moveup = -0x800;
			caretsp->rand_x = 1;
			caretsp->rand_y = 0;
		}
	}
}

void HitTwoCharMap(MAP *map, CARET_SPAWNER *caret_spawner)
{
	//Collision offsets and flags
	char offx[4];
	char offy[4];
	BOOLEAN flag1[4];
	BYTE flag2[4];
	
	offx[0] = 0;
	offx[1] = 1;
	offx[2] = 0;
	offx[3] = 1;
	
	offy[0] = 0;
	offy[1] = 0;
	offy[2] = 1;
	offy[3] = 1;
	
	flag1[0] = 0;
	flag1[1] = 0;
	flag1[2] = 1;
	flag1[3] = 1;
	
	flag2[0] = 1 | 2;
	flag2[1] = 4 | 2;
	flag2[2] = 8 | 1;
	flag2[3] = 8 | 4;
	
	//Collision position
	int x = tMC.x / 0x400 / 16;
	int y = tMC.y / 0x400 / 16;
	
	//Reset collision state
	signed char v19 = 4;
	tMC.flag = 0;
	
	for (int i = 0; i < 4; i++)
	{
		//Get tile at the position to check
		int part = map->data[(x + offx[i]) + map->width * (y + offy[i])];
		
		//Block collision
		if (part < 0x80 || part >= 0xA0)
			v19 -= flag1[i];
		else if ((JudgeHitTwoCharBlock(x + offx[i], y + offy[i], flag2[i]) & 8) == 0)
			v19 -= flag1[i];
		//Vector collision
		if (part >= 0x60 && part < 0x80)
			JudgeHitTwoCharVector(x + offx[i], y + offy[i], part);
		//Damage collision
		if (part >= 0xA0 && part < 0xC0)
			JudgeHitTwoCharDamage(x + offx[i], y + offy[i], caret_spawner);
		//Item collision
		else if (part >= 0x40 && part < 0x60)
			JudgeHitTwoCharItem(x + offx[i], y + offy[i], caret_spawner, map);
		//Snack collision
		else if (part < 0xE0 || part >= 0x100)
			v19 -= flag1[i];
		else if ((JudgeHitTwoCharSnack(x + offx[i], y + offy[i], flag2[i], caret_spawner, map) & 8) == 0)
			v19 -= flag1[i];
	}
	
	//Check if we should level up
	if (tMC.exp >= tMycExp[tMC.level])
	{
		//Level up
		tMC.exp -= tMycExp[tMC.level];
		++tMC.level;
		if (tMC.level > MAX_LEVEL)
			tMC.level = MAX_LEVEL;
		else
			PlaySoundObject(SOUND_ID_LEVELUP, SOUND_MODE_PLAY);
		
		//Create 'Level Up' text
		int levup_i = FindCaretSpawner(caret_spawner);
		if (levup_i != NO_CARET)
		{
			CARET_SPAWNER *caretsp = &caret_spawner[levup_i];
			caretsp->cond = TRUE;
			caretsp->type = 3;
			caretsp->ani_no = 0;
			caretsp->num = 1;
			caretsp->x = tMC.x + 0x2000;
			caretsp->y = tMC.y - 0x1000;
			caretsp->rand_x = 1;
			caretsp->rand_y = 0;
		}
	}
	
	//Set airborne flag
	if (v19 <= 0)
		tMC.airborne = TRUE;
}

//Draw TwoChar
void PutTwoChar(FRAME *frame)
{
	static RECT rcTwoChar[12] = {
		{  0,  0, 16, 16 },
		{ 16,  0, 32, 16 },
		{ 32,  0, 48, 16 },
		{ 48,  0, 64, 16 },

		{  0, 16, 16, 32 },
		{ 16, 16, 32, 32 },
		{ 32, 16, 48, 32 },
		{ 48, 16, 64, 32 },

		{  0, 32, 16, 48 },
		{ 16, 32, 32, 48 },
		{ 32, 32, 48, 48 },
		{ 48, 32, 64, 48 },
	};

	static RECT rcTwoCharShip[12] = {
		{   0,  0,  40,  40 },
		{  40,  0,  80,  40 },
		{  80,  0, 120,  40 },
		{ 120,  0, 160,  40 },

		{   0, 40,  40,  80 },
		{  40, 40,  80,  80 },
		{  80, 40, 120,  80 },
		{ 120, 40, 160,  80 },

		{   0, 80,  40, 120 },
		{  40, 80,  80, 120 },
		{  80, 80, 120, 120 },
		{ 120, 80, 160, 120 },
	};
	
	BYTE frame_no = (tMC.direct * 4) + tMC.ani_no;
	if (tMC.equip & 8)
		PutBitmap3(&grcFull, (tMC.x / 0x400) - (frame->x / 0x400) - 12, (tMC.y / 0x400) - (frame->y / 0x400) - 12, &rcTwoCharShip[frame_no], SURFACE_ID_TWOCHAR3);
	else if (tMC.equip & 1)
		PutBitmap3(&grcFull, (tMC.x / 0x400) - (frame->x / 0x400), (tMC.y / 0x400) - (frame->y / 0x400), &rcTwoChar[frame_no], SURFACE_ID_TWOCHAR);
	else
		PutBitmap3(&grcFull, (tMC.x / 0x400) - (frame->x / 0x400), (tMC.y / 0x400) - (frame->y / 0x400), &rcTwoChar[frame_no], SURFACE_ID_TWOCHAR2);
}

void PutTwoStatus()
{
	RECT rcStatus = { 0, 0, 88, 32 };
	PutBitmap3(&grcFull, WINDOW_WIDTH - 64, 8, &rcStatus, SURFACE_ID_STATUS_TWO);
	PutNumber(WINDOW_WIDTH - 64, 8, tMC.level);
	PutNumber(WINDOW_WIDTH - 64, 16, tMC.exp);
	PutNumber(WINDOW_WIDTH - 64, 16, tMycExp[tMC.level]);
	PutNumber(WINDOW_WIDTH - 64, 24, tMC.life);
	PutNumber(WINDOW_WIDTH - 112, 24, tMycLife[tMC.level]);
}

//Update TwoChar
int p2_swim_xm[3] = { -0x200, 0x200, 0 };
int p2_swim_ym[3] = { -0x200, -0x200, -0x2D4 };

int p2_dash_xm[3] = { -0xC00, 0xC00, 0 };
int p2_dash_ym[3] = { 0, 0, -0xC00 };

void ActTwoChar_Normal(CARET *caret, CARET_SPAWNER *caret_spawner)
{
	//Get direction
	tMC.direct = 2;
	if (gKey & KEY_LEFT_PLAYERTWO)
		tMC.direct = 0;
	if (gKey & KEY_RIGHT_PLAYERTWO)
		tMC.direct = 1;

	//Swim
	if ((gKeyTrg & KEY_Z_PLAYERTWO) && tMC.swim_wait == 0)
	{
		//Swim sound and bubble
		int caret_i = FindCaret(caret);
		if (caret_i != NO_CARET)
		{
			PlaySoundObject(1, 1);
			caret += caret_i; //... What the hell?
			caret->type = 1;
			caret->xm = -p2_swim_xm[tMC.direct];
			caret->ym = -p2_swim_ym[tMC.direct];
			caret->cond = TRUE;
			caret->ani_no = 0;
			caret->ani_wait = 0;
			switch (tMC.direct)
			{
				case 0:
					caret->x = tMC.x + 14336;
					caret->y = tMC.y + 12288;
					break;
				case 1:
					caret->x = tMC.x + 2048;
					caret->y = tMC.y + 12288;
					break;
				case 2:
					caret->x = tMC.x + 9216;
					caret->y = tMC.y + 14336;
					break;
			}
		}

		//Play animation and disable for 8 frames
		tMC.swim_wait = 8;
		tMC.ani_no = 1;
		if (tMC.ani_wait < 100)
			tMC.ani_wait += 10;

		//Swim velocity
		tMC.xm += p2_swim_xm[tMC.direct];
		tMC.ym += p2_swim_ym[tMC.direct];
	}

	//Dash
	if ((gKey & KEY_Z_PLAYERTWO) && (tMC.equip & 2))
	{
		//Play charged sound
		if (tMC.dash_wait == 31)
			PlaySoundObject(SOUND_ID_READY, SOUND_MODE_PLAY);

		//Charge dash
		if (++tMC.dash_wait > 32)
			tMC.dash_wait = 32;
	}
	else if (tMC.dash_wait == 32)
	{
		//Play dash sound
		PlaySoundObject(SOUND_ID_GO, SOUND_MODE_PLAY);

		//Dash bubble
		int caret_i = FindCaret(caret);
		if (caret_i != NO_CARET)
		{
			CARET *caretp = &caret[caret_i];
			caretp->type = 1;
			caretp->xm = p2_dash_xm[tMC.direct] / -8;
			caretp->ym = p2_dash_ym[tMC.direct] / -8;
			caretp->cond = TRUE;
			caretp->ani_no = 0;
			caretp->ani_wait = 0;
			switch (tMC.direct)
			{
				case 0:
					caretp->x = tMC.x + 14336;
					caretp->y = tMC.y + 12288;
					break;
				case 1:
					caretp->x = tMC.x + 2048;
					caretp->y = tMC.y + 12288;
					break;
				case 2:
					caretp->x = tMC.x + 9216;
					caretp->y = tMC.y + 14336;
					break;
			}
		}

		//Dash velocity and set to dash mode
		tMC.xm = p2_dash_xm[tMC.direct];
		tMC.ym = p2_dash_ym[tMC.direct];
		tMC.unit = 1;
	}
	else
	{
		tMC.dash_wait = 0;
	}
	
	//Animation timer
	if (tMC.ani_wait <= 0)
		tMC.ani_no = 0;
	else
		--tMC.ani_wait;

	//Swim timer
	if (tMC.swim_wait > 0)
		--tMC.swim_wait;

	//Dash animation
	if (tMC.dash_wait == 32)
		tMC.ani_no = 2;
	
	//Gravity
	if (tMC.ym < 0x800)
		tMC.ym += 20;
	
	//Speed limit
	if (tMC.ym < -0x800)
		tMC.ym = -0x800;
	if (tMC.ym > 0x800)
		tMC.ym = 0x800;
	if (tMC.xm < -0x800)
		tMC.xm = -0x800;
	if (tMC.xm > 0x800)
		tMC.xm = 0x800;
	
	//Friction
	if (tMC.xm > 0)
		tMC.xm -= 8;
	if (tMC.xm < 0)
		tMC.xm += 8;
	
	if (!tMC.airborne)
	{
		if (tMC.xm > 0)
			tMC.xm -= 24;
		if (tMC.xm < 0)
			tMC.xm += 24;
	}
	
	//Move
	if (tMC.xm >= 8 || tMC.xm <= -8)
		tMC.x += tMC.xm;
	tMC.y += tMC.ym;

	//Decrement timers
	if (tMC.shock)
		--tMC.shock;
	if (tMC.no_event)
		--tMC.no_event;
}

void ActTwoChar_Dash(CARET *caret, CARET_SPAWNER *caret_spawner)
{
	//Decrease dash timer and stop dashing when depleted
	if (--tMC.dash_wait <= 0)
		tMC.unit = 0;
	
	//Dash bubble
	if ((tMC.dash_wait % 4) == 0)
	{
		int caret_i = FindCaret(caret);
		if (caret_i != NO_CARET)
		{
			CARET *caretp = &caret[caret_i];
			caretp->type = 1;
			caretp->xm = (p2_dash_xm[tMC.direct] / -8) + Random(-0x200, 0x200);
			caretp->ym = (p2_dash_ym[tMC.direct] / -8) + Random(-0x200, 0x200);
			caretp->cond = TRUE;
			caretp->ani_no = 0;
			caretp->ani_wait = 0;
			caretp->x = tMC.x + 0x2000;
			caretp->y = tMC.y + 0x2000;
		}
	}
	
	//Move and use dash animation
	tMC.x += tMC.xm;
	tMC.y += tMC.ym;
	tMC.ani_no = 3;

	//Decrement timers
	if (tMC.shock)
		--tMC.shock;
}

void ActTwoChar_Ship(CARET *caret, CARET_SPAWNER *caret_spawner)
{
	//Create effect
	int caretsp_i = FindCaretSpawner(caret_spawner);
	if (caretsp_i != NO_CARET)
	{
		CARET_SPAWNER *caretsp = &caret_spawner[caretsp_i];
		caretsp->cond = TRUE;
		caretsp->type = 0;
		caretsp->ani_no = 0;
		caretsp->num = 1;
		caretsp->x = tMC.x + 0x2000;
		caretsp->y = tMC.y + 0x6000;
		caretsp->rand_moveright = 0xC00;
		caretsp->rand_moveleft = -0xC00;
		caretsp->rand_movedown = 0xC00;
		caretsp->rand_moveup = 0;
		caretsp->rand_x = 1;
		caretsp->rand_y = 0;
	}
	
	//Fly up
	tMC.xm = 0;
	tMC.ym -= 16;
	tMC.y += tMC.ym;
	tMC.ani_no = 3;
	if (tMC.shock != 0)
		tMC.shock = 0;
}

void ActTwoChar(CARET *caret, CARET_SPAWNER *caret_spawner)
{
	two_act[tMC.unit](caret, caret_spawner);
}

//Initalize TwoChar
void InitTwoChar()
{
	tMC.cond = TRUE;
	tMC.equip = 0;
	tMC.dead = 0;
	tMC.level = 0;
	tMC.life = tMycLife[0];
	tMC.exp = 0;
	tMC.x = 0xA0000;
	tMC.y = 0x1A0000;
	tMC.ym = 0;
	tMC.xm = 0;
	tMC.airborne = 1;
	tMC.ani_wait = 0;
	tMC.ani_no = 0;
	tMC.direct = 0;
	tMC.flag = 0;
	tMC.unit = 0;
	tMC.shock = 0;
	tMC.no_event = 100;
	tMC.dash_wait = 0;
	tMC.swim_wait = 0;
	tMC.carry = 0;
}