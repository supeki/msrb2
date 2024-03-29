// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: f_finale.c,v 1.3 2000/08/03 17:57:41 bpereira Exp $
//
// Copyright (C) 1993-1996 by id Software, Inc.
// Portions Copyright (C) 1998-2000 by DooM Legacy Team.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//
// $Log: f_finale.c,v $
// Revision 1.3  2000/08/03 17:57:41  bpereira
// no message
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      Game completion, final screen animation.
//
//-----------------------------------------------------------------------------


#include "doomdef.h"
#include "doomstat.h"
#include "am_map.h"
#include "dstrings.h"
#include "d_main.h"
#include "f_finale.h"
#include "g_game.h"
#include "hu_stuff.h"
#include "r_local.h"
#include "s_sound.h"
#include "i_video.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

// Stage of animation:
// 0 = text, 1 = art screen
static int finalestage, finalecount;

static int timetonext; // Delay between screen changes
static int finaletextcount;
static int animtimer; // Used for some animation timings

static int deplete, stoptimer;

#define TEXTSPEED 3
#define TEXTWAIT 250

static const char* finaletext = NULL;
static const char* finaleflat;
static boolean keypressed = false;

static patch_t* background;
static patch_t* currentanim;
static patch_t* nextanim;
static patch_t* first;
static patch_t* second;
static patch_t* third;

// De-Demo'd Title Screen
static patch_t* ttbanner; // white banner with "robo blast" and "2"
static patch_t* ttwing; // wing background
static patch_t* ttsonic; // "SONIC"
static patch_t* ttswave1; // Title Sonics
static patch_t* ttswave2;
static patch_t* ttswip1;
static patch_t* ttsprep1;
static patch_t* ttsprep2;
static patch_t* ttspop1;
static patch_t* ttspop2;
static patch_t* ttspop3;
static patch_t* ttspop4;
static patch_t* ttspop5;
static patch_t* ttspop6;
static patch_t* ttspop7;

static void F_SkyScroll(void);

// Stage of animation:
//  0 = text, 1 = art screen, 2 = character cast

#define TEXTSPEED       3
#define TEXTWAIT        250

void    F_StartCast (void);
void    F_CastTicker (void);
boolean F_CastResponder (event_t *ev);
void    F_CastDrawer (void);

//
// F_StartFinale
//
void F_StartFinale (void)
{
    gamestate = GS_FINALE;

    // Okay - IWAD dependend stuff.
    // This has been changed severly, and
    //  some stuff might have changed in the process.
    switch ( gamemode )
    {

      // DOOM 1 - E1, E3 or E4, but each nine missions
      case shareware:
      case registered:
      case retail:
      {
        S_ChangeMusic(mus_victor, true);

        switch (gameepisode)
        {
          case 1:
            finaleflat = text[FLOOR4_8_NUM];
            finaletext = E1TEXT;
            break;
          case 2:
            finaleflat = text[SFLR6_1_NUM];
            finaletext = E2TEXT;
            break;
          case 3:
            finaleflat = text[MFLR8_4_NUM];
            finaletext = E3TEXT;
            break;
          case 4:
            finaleflat = text[MFLR8_3_NUM];
            finaletext = E4TEXT;
            break;
          default:
            // Ouch.
            break;
        }
        break;
      }

      // DOOM II and missions packs with E1, M34
      case commercial:
      {
		  if(xmasmode && gamemap == 5)
			S_ChangeMusic(mus_read_m, true);

          switch (gamemap)
          {
            case 4: // Tails 09-02-2001
              finaleflat = text[SLIME16_NUM];
              finaletext = C1TEXT;
              break;
            case 5: // Tails 09-02-2001
              finaleflat = text[RROCK14_NUM];
              finaletext = C2TEXT;
              break;
            case 24: // Tails 09-02-2001
              finaleflat = text[RROCK07_NUM];
              finaletext = C3TEXT;
              break;
            case 25: // Tails 09-02-2001
              finaleflat = text[RROCK17_NUM];
              finaletext = C4TEXT;
              break;
            case 26: // Tails 09-02-2001
              finaleflat = text[RROCK13_NUM];
              finaletext = C5TEXT;
              break;
            case 27: // Tails 09-02-2001
              finaleflat = text[RROCK19_NUM];
              finaletext = C6TEXT;
              break;
            default:
              // Ouch.
              break;
          }
          break;
      }


      // Indeterminate.
      default:
        S_ChangeMusic(mus_read_m, true);
        finaleflat = "F_SKY1"; // Not used anywhere else.
        finaletext = C1TEXT;  // FIXME - other text, music?
        break;
    }

    finalestage = 0;
    finalecount = 0;

}

void F_StartTitleScreen(void)
{
	gamestate = GS_TITLESCREEN;
	CON_ClearHUD();

	// IWAD dependent stuff.

	S_ChangeMusic(mus_dm2ttl, false);

	finalecount = 0;
	finalestage = 0;
	animtimer = 0;

	ttbanner = W_CachePatchName("TTBANNER", PU_LEVEL);
	ttwing = W_CachePatchName("TTWING", PU_LEVEL);
	ttsonic = W_CachePatchName("TTSONIC", PU_LEVEL);
	ttswave1 = W_CachePatchName("TTSWAVE1", PU_LEVEL);
	ttswave2 = W_CachePatchName("TTSWAVE2", PU_LEVEL);
	ttswip1 = W_CachePatchName("TTSWIP1", PU_LEVEL);
	ttsprep1 = W_CachePatchName("TTSPREP1", PU_LEVEL);
	ttsprep2 = W_CachePatchName("TTSPREP2", PU_LEVEL);
	ttspop1 = W_CachePatchName("TTSPOP1", PU_LEVEL);
	ttspop2 = W_CachePatchName("TTSPOP2", PU_LEVEL);
	ttspop3 = W_CachePatchName("TTSPOP3", PU_LEVEL);
	ttspop4 = W_CachePatchName("TTSPOP4", PU_LEVEL);
	ttspop5 = W_CachePatchName("TTSPOP5", PU_LEVEL);
	ttspop6 = W_CachePatchName("TTSPOP6", PU_LEVEL);
	ttspop7 = W_CachePatchName("TTSPOP7", PU_LEVEL);
}

boolean F_Responder (event_t *event)
{
    if (finalestage == 2)
        return F_CastResponder (event);

    return false;
}


//
// F_Ticker
//
void F_Ticker (void)
{
    int         i;

    // check for skipping
    if ( (gamemode == commercial)
      && ( finalecount > 50) )
    {
      // go on to the next level
      for (i=0 ; i<MAXPLAYERS ; i++)
        if (players[i].cmd.buttons)
          break;

      if (i < MAXPLAYERS)
      {
        if (gamemap == 30)
          F_StartCast ();
        else
        {
           gameaction = ga_worlddone;
           finalecount = MININT;    // wait until map is lunched
        }
      }
    }

    // advance animation
    finalecount++;

    if (finalestage == 2)
    {
        F_CastTicker ();
        return;
    }

    if ( gamemode == commercial)
        return;

    if (!finalestage && (unsigned)finalecount>strlen (finaletext)*TEXTSPEED + TEXTWAIT)
    {
        finalecount = 0;
        finalestage = 1;
        wipegamestate = -1;             // force a wipe
        if (gameepisode == 3)
            S_StartMusic (mus_bunny);
    }
}

void F_TitleScreenTicker(void)
{
	finalecount++;
	finalestage += 8;
}


//
// F_TextWrite
//
void F_TextWrite (void)
{
    int         w;
    int         count;
    char*       ch;
    int         c;
    int         cx;
    int         cy;

    // erase the entire screen to a tiled background
    V_DrawFlatFill(0,0,vid.width,vid.height,W_GetNumForName(finaleflat));

    V_MarkRect (0, 0, vid.width, vid.height);

// Tails DRAW A FULL PIC INSTEAD OF FLAT!
    switch (gamemap)
	{
		case 4:
			V_DrawScaledPatch (0,0,0,W_CacheLumpName ("SEGALOGO", PU_CACHE));
			break;
        case 5:
			V_DrawScaledPatch (0,0,0,W_CacheLumpName ("BOSSBACK", PU_CACHE));
			break;
        default:
			break;
	}

    // draw some of the text onto the screen
    cx = 10;
    cy = 10;
    ch = finaletext;

    count = (finalecount - 10)/TEXTSPEED;
    if (count < 0)
        count = 0;
    for ( ; count ; count-- )
    {
        c = *ch++;
        if (!c)
            break;
        if (c == '\n')
        {
            cx = 10;
            cy += 11;
            continue;
        }

        c = toupper(c) - HU_FONTSTART;
        if (c < 0 || c> HU_FONTSIZE)
        {
            cx += 4;
            continue;
        }

        w = SHORT (hu_font[c]->width);
        if (cx+w > vid.width)
            break;
        V_DrawScaledPatch(cx, cy, 0, hu_font[c]); // Tails 12-12-2001
        cx+=w;
    }

}

//
// Final DOOM 2 animation
// Casting by id Software.
//   in order of appearance
//
typedef struct
{
    char                *name;
    mobjtype_t  type;
} castinfo_t;

castinfo_t      castorder[] = {
    {NULL, MT_BLUECRAWLA},
    {NULL, MT_REDCRAWLA},
    {NULL, MT_BLUECRAWLA},
    {NULL, MT_GARGOYLE},
    {NULL, MT_SNOWFLAKE},
    {NULL, MT_BLUECRAWLA},
    {NULL, MT_JETTBOMBER}, // Tails 08-18-2001
    {NULL, MT_REDCRAWLA},
    {NULL, MT_REDCRAWLA},
    {NULL, MT_REDCRAWLA},
    {NULL, MT_DETON},
    {NULL, MT_REDCRAWLA},
    {NULL, MT_REDCRAWLA},
    {NULL, MT_EGGMOBILE},
    {NULL, MT_JETTGUNNER}, // Tails 09-03-2001
    {NULL, MT_EGGMOBILE},
    {NULL, MT_PLAYER},

    {NULL,0}
};

int             castnum;
int             casttics;
state_t*        caststate;
boolean         castdeath;
int             castframes;
int             castonmelee;
boolean         castattacking;


//
// F_StartCast
//


void F_StartCast (void)
{
    int i;

    for(i=0;i<17;i++)
       castorder[i].name = text[CC_ZOMBIE_NUM+i];

    wipegamestate = -1;         // force a screen wipe
    castnum = 0;
    caststate = &states[mobjinfo[castorder[castnum].type].seestate];
    casttics = caststate->tics;
    castdeath = false;
    finalestage = 2;
    castframes = 0;
    castonmelee = 0;
    castattacking = false;
    S_ChangeMusic(mus_evil, true);
}


//
// F_CastTicker
//
void F_CastTicker (void)
{
    int         st;
    int         sfx;

    if (--casttics > 0)
        return;                 // not time to change state yet

    if (caststate->tics == -1 || caststate->nextstate == S_NULL)
    {
        // switch from deathstate to next monster
        castnum++;
        castdeath = false;
        if (castorder[castnum].name == NULL)
            castnum = 0;
        if (mobjinfo[castorder[castnum].type].seesound)
            S_StartSound (NULL, mobjinfo[castorder[castnum].type].seesound);
        caststate = &states[mobjinfo[castorder[castnum].type].seestate];
        castframes = 0;
    }
    else
    {
        // just advance to next state in animation
        if (caststate == &states[S_PLAY_ATK1])
            goto stopattack;    // Oh, gross hack!
        st = caststate->nextstate;
        caststate = &states[st];
        castframes++;

        // sound hacks....
        switch (st)
        {
          case S_PLAY_ATK1:     sfx = sfx_dshtgn; break;
          case S_POSS_RUN1:     sfx = sfx_menu1; break; // Tails 06-13-2000
          case S_SPOS_RUN1:     sfx = sfx_shotgn; break; // Tails 06-13-2000
//          case S_VILE_ATK2:     sfx = sfx_vilatk; break;
//          case S_SKEL_FIST2:    sfx = sfx_skeswg; break;
//          case S_SKEL_FIST4:    sfx = sfx_skepch; break;
//          case S_SKEL_MISS2:    sfx = sfx_bkpoof; break;
//          case S_FATT_ATK8:
//          case S_FATT_ATK5:
//          case S_FATT_ATK2:     sfx = sfx_lvpass; break;
//          case S_CPOS_ATK2:
//          case S_CPOS_ATK3:
//          case S_CPOS_ATK4:     sfx = sfx_shotgn; break;
//          case S_TROO_ATK3:     sfx = sfx_claw; break;
//          case S_SARG_ATK2:     sfx = sfx_sgtatk; break;
//          case S_BOSS_ATK2:
//          case S_BOS2_ATK2:
//          case S_HEAD_ATK2:     sfx = sfx_lvpass; break; // Tails 08-18-2001
//          case S_SKULL_ATK2:    sfx = sfx_sklatk; break;
//          case S_SPID_ATK2:
//          case S_SPID_ATK3:     sfx = sfx_shotgn; break;
//          case S_BSPI_ATK2:     sfx = sfx_plasma; break;
//          case S_EGGMOBILE_ATK2:
//          case S_EGGMOBILE_ATK2:
          case S_EGGMOBILE_ATK2:    sfx = sfx_rlaunc; break;
          case S_DISS:     sfx = sfx_sklatk; break;
          default: sfx = 0; break;
        }

        if (sfx)
            S_StartSound (NULL, sfx);
    }

    if (castframes == 12)
    {
        // go into attack frame
        castattacking = true;
        if (castonmelee)
            caststate=&states[mobjinfo[castorder[castnum].type].meleestate];
        else
            caststate=&states[mobjinfo[castorder[castnum].type].missilestate];
        castonmelee ^= 1;
        if (caststate == &states[S_NULL])
        {
            if (castonmelee)
                caststate=
                    &states[mobjinfo[castorder[castnum].type].meleestate];
            else
                caststate=
                    &states[mobjinfo[castorder[castnum].type].missilestate];
        }
    }

    if (castattacking)
    {
        if (castframes == 24
            ||  caststate == &states[mobjinfo[castorder[castnum].type].seestate] )
        {
          stopattack:
            castattacking = false;
            castframes = 0;
            caststate = &states[mobjinfo[castorder[castnum].type].seestate];
        }
    }

    casttics = caststate->tics;
    if (casttics == -1)
        casttics = 15;
}


//
// F_CastResponder
//

boolean F_CastResponder (event_t* ev)
{
    if (ev->type != ev_keydown)
        return false;

    if (castdeath)
        return true;                    // already in dying frames

    // go into death frame
    castdeath = true;
    caststate = &states[mobjinfo[castorder[castnum].type].deathstate];
    casttics = caststate->tics;
    castframes = 0;
    castattacking = false;
    if (mobjinfo[castorder[castnum].type].deathsound)
        S_StartSound (NULL, mobjinfo[castorder[castnum].type].deathsound);

    return true;
}


#define CASTNAME_Y   180                // where the name of actor is drawn
void F_CastPrint (char* text)
{
    V_DrawString ((BASEVIDWIDTH-V_StringWidth (text))/2, CASTNAME_Y, text);
}


//
// F_CastDrawer
//
void V_DrawPatchFlipped (int x, int y, int scrn, patch_t *patch);

void F_CastDrawer (void)
{
    spritedef_t*        sprdef;
    spriteframe_t*      sprframe;
    int                 lump;
    boolean             flip;
    patch_t*            patch;

    // erase the entire screen to a background
    //V_DrawPatch (0,0,0, W_CachePatchName ("BOSSBACK", PU_CACHE));
    D_PageDrawer ("BOSSBACK");

    F_CastPrint (castorder[castnum].name);

    // draw the current frame in the middle of the screen
    sprdef = &sprites[caststate->sprite];
    sprframe = &sprdef->spriteframes[ caststate->frame & FF_FRAMEMASK];
    lump = sprframe->lumppat[0];      //Fab: see R_InitSprites for more
    flip = (boolean)sprframe->flip[0];

    patch = W_CachePatchNum (lump, PU_CACHE);

    if (flip)
        V_DrawScaledPatchFlipped (BASEVIDWIDTH>>1,170,0,patch);
    else
        V_DrawScaledPatch (BASEVIDWIDTH>>1,170,0,patch);
}


//
// F_DrawPatchCol
//
static void F_DrawPatchCol (int           x,
  patch_t*      patch,
  int           col )
{
    column_t*   column;
    byte*       source;
    byte*       dest;
    byte*       desttop;
    int         count;

    column = (column_t *)((byte *)patch + LONG(patch->columnofs[col]));
    desttop = screens[0]+x*vid.dupx;

    // step through the posts in a column
    while (column->topdelta != 0xff )
    {
        source = (byte *)column + 3;
        dest = desttop + column->topdelta*vid.width;
        count = column->length;

        while (count--)
        {
            int dupycount=vid.dupy;

            while(dupycount--)
            {
                int dupxcount=vid.dupx;
                while(dupxcount--)
                     *dest++ = *source;

                dest += (vid.width-vid.dupx);
            }
            source++;
        }
        column = (column_t *)(  (byte *)column + column->length + 4 );
    }
}


//
// F_BunnyScroll
//
void F_BunnyScroll (void)
{
    int         scrolled;
    int         x;
    patch_t*    p1;
    patch_t*    p2;
    char        name[10];
    int         stage;
    static int  laststage;

    p1 = W_CachePatchName ("PFUB2", PU_LEVEL);
    p2 = W_CachePatchName ("PFUB1", PU_LEVEL);

    V_MarkRect (0, 0, vid.width, vid.height);

    scrolled = 320 - (finalecount-230)/2;
    if (scrolled > 320)
        scrolled = 320;
    if (scrolled < 0)
        scrolled = 0;
    //faB:do equivalent for hw mode ?
    if (rendermode==render_soft)
    {
        for ( x=0 ; x<320 ; x++)
        {
            if (x+scrolled < 320)
                F_DrawPatchCol (x, p1, x+scrolled);
            else
                F_DrawPatchCol (x, p2, x+scrolled - 320);
        }
    }
    else
    {
        if( scrolled>0 )
            V_DrawScaledPatch(320-scrolled,0, 0, p2 );
        if( scrolled<320 )
            V_DrawScaledPatch(-scrolled,0, 0, p1 );
    }

    if (finalecount < 1130)
        return;
    if (finalecount < 1180)
    {
        V_DrawScaledPatch ((320-13*8)/2,
                           (200-8*8)/2,0, W_CachePatchName ("END0",PU_CACHE));
        laststage = 0;
        return;
    }

    stage = (finalecount-1180) / 5;
    if (stage > 6)
        stage = 6;
    if (stage > laststage)
    {
        S_StartSound (NULL, sfx_menu1);
        laststage = stage;
    }

    sprintf (name,"END%i",stage);
    V_DrawScaledPatch ((320-13*8)/2, (200-8*8)/2,0, W_CachePatchName (name,PU_CACHE));
}


//
// F_Drawer
//
void F_Drawer (void)
{
    if (finalestage == 2)
    {
        F_CastDrawer ();
        return;
    }

    if (!finalestage)
        F_TextWrite ();
    else
    {
        switch (gameepisode)
        {
          case 1:
            if ( gamemode == retail )
              V_DrawScaledPatch (0,0,0,
                         W_CachePatchName(text[CREDIT_NUM],PU_CACHE));
            else
              V_DrawScaledPatch (0,0,0,
                         W_CachePatchName(text[HELP2_NUM],PU_CACHE));
            break;
          case 2:
            V_DrawScaledPatch(0,0,0,
                        W_CachePatchName(text[VICTORY2_NUM],PU_CACHE));
            break;
          case 3:
            F_BunnyScroll ();
            break;
          case 4:
            V_DrawScaledPatch (0,0,0,
                         W_CachePatchName(text[ENDPIC_NUM],PU_CACHE));
            break;
        }
    }

}

static void F_SkyScroll(void)
{
	int scrolled, x, mx, fakedwidth;
	patch_t* pat;

	pat = W_CachePatchName("TITLESKY", PU_CACHE);

	animtimer = finalecount % 64;

	fakedwidth = vid.width / vid.dupx;

	scrolled = 320 - (animtimer)*5;
	if(scrolled > 320)
		scrolled = 320;
	if(scrolled < 0)
		scrolled = 0;

	if(rendermode == render_soft)
	{
		for(x = 0, mx = 0; x < fakedwidth; x++, mx++)
		{
			if(mx == 320)
				mx = 0;

			if(mx + scrolled < 320)
				F_DrawPatchCol(x, pat, mx + scrolled);
			else
				F_DrawPatchCol(x, pat, mx + scrolled - 320);
		}
	}
#ifdef HWRENDER
	else /// \note do equivalent for hw mode?
	{
		if(scrolled > 0)
			V_DrawScaledPatch(320 - scrolled, 0, 0, pat);

		if(scrolled < 320)
			V_DrawScaledPatch(-scrolled, 0, 0, pat);
	}
#endif
}

void F_TitleScreenDrawer(void)
{
	// Draw that sky!
	F_SkyScroll();

	V_DrawScaledPatch(30, 14, 0, ttwing);

	if(finalecount < 57)
	{
		if(finalecount == 35)
			V_DrawScaledPatch(115, 15, 0, ttspop1);
		else if(finalecount == 36)
			V_DrawScaledPatch(114, 15, 0,ttspop2);
		else if(finalecount == 37)
			V_DrawScaledPatch(113, 15, 0,ttspop3);
		else if(finalecount == 38)
			V_DrawScaledPatch(112, 15, 0,ttspop4);
		else if(finalecount == 39)
			V_DrawScaledPatch(111, 15, 0,ttspop5);
		else if(finalecount == 40)
			V_DrawScaledPatch(110, 15, 0, ttspop6);
		else if(finalecount >= 41 && finalecount <= 44)
			V_DrawScaledPatch(109, 15, 0, ttspop7);
		else if(finalecount >= 45 && finalecount <= 48)
			V_DrawScaledPatch(108, 12, 0, ttsprep1);
		else if(finalecount >= 49 && finalecount <= 52)
			V_DrawScaledPatch(107, 9, 0, ttsprep2);
		else if(finalecount >= 53 && finalecount <= 56)
			V_DrawScaledPatch(106, 6, 0, ttswip1);
		V_DrawScaledPatch(93, 106, 0, ttsonic);
	}
	else
	{
		V_DrawScaledPatch(93, 106, 0,ttsonic);
		if(finalecount/5 & 1)
			V_DrawScaledPatch(100, 3, 0,ttswave1);
		else
			V_DrawScaledPatch(100,3, 0,ttswave2);
	}

	V_DrawScaledPatch(48, 142, 0,ttbanner);
}
