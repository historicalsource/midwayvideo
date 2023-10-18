/*
** Copyright (c) 1995, 3Dfx Interactive, Inc.
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of 3Dfx Interactive, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of 3Dfx Interactive, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished  -
** rights reserved under the Copyright Laws of the United States.
**
** $Revision: 1 $ 
** $Date: 9/02/98 12:36a $
** $Log: /Releases/Banshee/GLOP/3Dfx/Devel/H3/glide3/src/qmodes.h $
** 
** 1     9/02/98 12:36a Sapphire
** 
** 4     7/13/98 5:26p Andrew
** Changed to support a gamma table
** 
** 3     7/11/98 8:16a Andrew
** Added gamma correction
** 
** 2     5/12/98 9:35a Andrew
** Added some minor changes for valid modes
** 
** 1     4/22/98 2:47p Andrew
** Query Modes Protocol Information
**
** 
*/

#ifndef _QMODES_H_
#define _QMODES_H_

/* The QUERYMODES ESC Code */
#define QUERYESCMODE (0x8001)

#define TDFXACK   (0x3DF0)
#define TDFXERR   (0xFFFF)

// General Queries
#define QUERYVERSION (0x0000)

// Mode Queries
#define QUERYNUMMODES (0x0001)
#define QUERYMODES (0x0002)
#define QUERYDEVNODE (0x0003)

// Virtual Desktop Queries
#define QUERYMAXFREEMEM (0x100)
#define QUERYSETVIRTUALSIZE (0x101)
#define QUERYGETSTARTADDR (0x102)
#define QUERYSETSTARTADDR (0x103)

// Gamma Queries
#define QUERYGETGAMMA (0x200)
#define QUERYSETGAMMA (0x201)

// TVOut Queries
#include "tv.h"

/*
**  Protocol:
**
**  Call will be of the form 
**   ExtEscape(hdc, QUERYESCMODE, sizeof(QIN), &Qin, sizeof(Output), &Output);
**   Escape(hdc, QUERYESCMODE, sizeof(QIN), &Qin, &Output);
**  
*/

 
/*
**  Standard Input Structure 
**
*/
typedef struct qin {
   DWORD dwSubFunc;        // Subfunction
} QIN, FAR * LPQIN, * PQIN;

/*
**  
**
**  INPUT:      qin.dwSubFunc = QUERYVERSION
**  OUTPUT: Driver Major & Minor Version
*/
typedef struct qversion {
   DWORD dwMajor;
   DWORD dwMinor;
   } QVERSION, FAR * LPQVERSION, * PQVERSION;

#define QUERYMODE_MAJOR 0x00000000
#define QUERYMODE_MINOR 0x00009999

/*
**  Call this function first to know how many modes to allow
**  the following structure for 
**
**  INPUT:      qin.dwSubFunc = QUERYNUMMODES
**  OUTPUT: Number of modes
*/
typedef struct qnummode {
   DWORD dwNum;
   } QNUMMODE, FAR * LPQNUMMODE, * PQNUMODE;

/*
**  This is the mode information.  You will need to allocate
**  # modes * QMODE structures that the Driver will fill in.
**
**  INPUT:      qin.dwSubFunc = QUERYMODES
**  OUTPUT: #Modes * QMODE
*/
typedef struct qmode {
   DWORD dwX;
   DWORD dwY;
   DWORD dwBpp;
   DWORD dwRef;
   DWORD dwValid;       // This will be a field of flags
   } QMODE, FAR * LPQMODE, * PQMODE;

#define QUERY_MODE_VALID (0x000000001L)
#define QUERY_TV_MODE (0x000000002L)

/*
**  This is Devnode that the driver is using
**
**  INPUT:      qin.dwSubFunc = QUERYDEVNODE
**  OUTPUT: dwDevNode
*/
typedef struct qdevnode {
   DWORD dwDevNode;                       // Monitor Device Node
   DWORD dwValidDefGamma;                 // 1 ==> bGamma is valid; 0 ==> Invalid
   BYTE bGamma;                           // Monitor Default Gamma <To Convert to float (bGamma + 100)/100.00>
   } QDEVNODE, FAR * LPQDEVNODE, * PQDEVNODE;

#define QUERY_MONITOR_GAMMA_VALID (0x00000001L)

/*
**  This is maximum free memory available for Virtual Desktop Usage in
**  this mode
**
**  INPUT:      qin.dwSubFunc = QUERYMAXFREEMEM
**  OUTPUT: Maximum Free Memory
*/
typedef struct qmaxfree {
   DWORD dwSubFunc;           // Should be set to QUERYSETVIRTUALSIZE
   DWORD dwMaxFree;           // Free Memory Available for Virtual Desktop
   DWORD dwMaxX;              // Maximum Free X ==> min(dwMaxFree/Y, MaxHardwareX) <not simultaneous with MaxY>
   DWORD dwMaxY;              // Maximum Free Y ==> min(dwMaxFree/(X*BPP), MaxHardwareY) <not simultaneous with MaxX>
   } QMAXFREE, FAR * LPQMAXFREE, * PQMAXFREE;

/*
**  This is used to set the Virtual Desktop Size
**
**  INPUT:      qsetvsize.dwSubFunc = QUERYSETVIRTUALSIZE
**  OUTPUT: None <Returns error on Failure>
*/
typedef struct qsetvsize {
   DWORD dwSubFunc;           // Should be set to QUERYSETVIRTUALSIZE
   DWORD dwX;                 // X Size in Pixels  (X * BPP * Y <= MaxMemSize)
   DWORD dwY;                 // Y Size in Lines
   } QSETVSIZE, FAR * LPQSETVSIZE, * PQSETVSIZE;

/*
**  This is used to get the Virtual Desktop Start Address
**
**  INPUT:      qin.dwSubFunc = QUERYGETSTARTADDR
**  OUTPUT: QGetStartAddr
*/
typedef struct qgetstartaddr {
   DWORD dwX;                 // X Location in Pixels  
   DWORD dwY;                 // Y Location in Lines
   } QGETSTARTADDR, FAR * LPQGETSTARTADDR, * PQGETSTARTADDR;

/*
**  This is used to move the Virtual Desktop Start Address
**
**  INPUT:      qsetstartaddr.dwSubFunc = QUERYSETSTARTADDR
**  OUTPUT: None <Returns Error on Failure>
*/
typedef struct qsetstartaddr {
   DWORD dwSubFunc;           // Should be set to QUERYSETSTARTADDR
   DWORD dwX;                 // X Location in Pixels  (Note: Start Addr = X * BPP + Y * Display Pitch)
   DWORD dwY;                 // Y Location in Lines
   } QSETSTARTADDR, FAR * LPQSETSTARTADDR, * PQSETSTARTADDR;

/*
**  This is used to get the gamma values currently in use
**
**  INPUT:      qin.dwSubFunc = QUERYGETGAMMA
**  OUTPUT: QGETGAMMA
*/
typedef struct qgetgamma {
   DWORD dwRed;                 // Red Gamma * 100
   DWORD dwGreen;               // Green Gamma * 100
   DWORD dwBlue;                // Blue Gamma * 100
   DWORD GammaTable[256];       // Gamma Table Defined as 0x00BBGGRR
   } QGETGAMMA, FAR * LPQGETGAMMA, * PQGETGAMMA;

/*
**  This is used to set the Gamma Value
**
**  INPUT:      qSetGamma.dwSubFunc = QUERYSETSTARTADDR
**  OUTPUT: None
*/
typedef struct qsetgamma {
   DWORD dwSubFunc;             // Should be set to QUERYSETGAMMA
   DWORD dwRed;                 // Red Gamma * 100
   DWORD dwGreen;               // Green Gamma * 100
   DWORD dwBlue;                // Blue Gamma * 100
   DWORD GammaTable[256];       // Gamma Table Defined as 0x00BBGGRR
   } QSETGAMMA, FAR * LPQSETGAMMA, * PQSETGAMMA;

int QueryMode(LPQIN lpQIN, LPVOID lpOutput);

#endif
