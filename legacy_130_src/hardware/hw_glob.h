// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: hw_glob.h,v 1.8 2000/04/27 17:48:47 hurdler Exp $
//
// Copyright (C) 1998-2000 by DooM Legacy Team.
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
// $Log: hw_glob.h,v $
// Revision 1.8  2000/04/27 17:48:47  hurdler
// colormap code in hardware mode is now the default
//
// Revision 1.7  2000/04/24 15:46:34  hurdler
// Support colormap for text
//
// Revision 1.6  2000/04/23 16:19:52  bpereira
// no message
//
// Revision 1.5  2000/04/22 21:08:23  hurdler
// I like it better like that
//
// Revision 1.4  2000/04/22 16:09:14  hurdler
// support skin color in hardware mode
//
// Revision 1.3  2000/03/29 19:39:49  bpereira
// no message
//
// Revision 1.2  2000/02/27 00:42:11  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:33  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      globals (shared data & code) for hw_ modules
//
//-----------------------------------------------------------------------------


#ifndef _HWR_GLOB_H_
#define _HWR_GLOB_H_

#include "hw_defs.h"
#include "hw_main.h"

#define crapmul (1.0f / 65536.0f)

// height of status bar scaled to current resolution
#define STAT_HEIGHT         (ST_HEIGHT*vid.height/BASEVIDHEIGHT)

// the original aspect ratio of Doom graphics isn't square
#define ORIGINAL_ASPECT     (320.0f/200.0f)

// -----------
// structures
// -----------

// a vertex of a Doom 'plane' polygon
typedef struct {
    float   x;
    float   y;
} polyvertex_t;

// a convex 'plane' polygon, clockwise order
typedef struct {
    int          numpts;
    polyvertex_t pts[0];
} poly_t;

// holds extra info for 3D render, for each subsector in subsectors[]
typedef struct {
    poly_t*     planepoly;  // the generated convex polygon
} extrasubsector_t;

// needed for sprite rendering
// equivalent of the software renderer's vissprites
typedef struct gr_vissprite_s
{
    // Doubly linked list
    struct gr_vissprite_s* prev;
    struct gr_vissprite_s* next;
    float               x1;
    float               x2;
    float               tz;
    float               ty;
    int                 patch;
    boolean             flip;
    unsigned char       translucency;       //alpha level 0-255
    unsigned char       sectorlight;        // ...
    //01/11/99: Hurdler: added for coronas correct lighting with all wads
    // actually an int, but maybe later an unsigned char
    int                 type; // light, rocket,...
    mobj_t              *mobj; //BP: i need the position without transform 
                               // (remove this when TANDL is done)
   //Hurdler: 25/04/2000: now support colormap in hardware mode
    byte                *colormap;
} gr_vissprite_t;


// --------
// hw_bsp.c
// --------
extern  extrasubsector_t*   extrasubsectors;
extern  int                 addsubsector;

void HWR_InitPolyPool (void);
void HWR_FreePolyPool (void);


// --------
// hw_cache.c
// --------
void HWR_InitTextureCache (void);
void HWR_FreeTextureCache (void);
void HWR_FreeExtraSubsectors (void);

void HWR_GetFlat (int flatlumpnum);
GlideTexture_t* HWR_GetTexture (int tex);
void HWR_GetPatch (GlidePatch_t* gpatch);
void HWR_GetMappedPatch(GlidePatch_t* gpatch, byte *colormap);
GlidePatch_t *HWR_GetPic (int lumpnum);


// --------
// hw_draw.c
// --------
extern  float   gr_patch_scalex;
extern  float   gr_patch_scaley;

void HWR_InitFog (void);
void HWR_FreeFog (void);
void HWR_FoggingOn (void);

extern  consvar_t cv_grrounddown;   //on/off


// ------------
// misc externs
// ------------

extern void V_SetPalette( byte* palette );
extern ULONG V_GetColor( int color );


#endif //_HW_GLOB_
