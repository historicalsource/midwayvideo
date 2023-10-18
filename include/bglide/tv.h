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
** $Log: /Releases/Banshee/GLOP/3Dfx/Devel/H3/glide3/src/tv.h $
** 
** 1     9/02/98 12:36a Sapphire
** 
** 2     7/11/98 8:18a Andrew
** changed some comments
** 
** 1     5/12/98 9:34a Andrew
** First attempt at TV out interface
** 
*/

#ifndef _TV_H_
#define _TV_H_

// TVOut Queries
#define QUERYTVAVAIL            (0x300)           // Returns Status Determined at Boot
#define QUERYTVSENSE            (0x301)           // Performs a Sense on the available connectors

// Individual Queries on Capabilities
#define QUERYGETPICCAP          (0x321)          // Get Individual Picture Control Capability
#define QUERYGETFILTERCAP       (0x322)          // Get Individual Filter Control Capability
#define QUERYGETPOSCAP          (0x323)          // Get Individual Position Control Capability
#define QUERYGETSIZECAP         (0x324)          // Get Individual Size Control Capability
#define QUERYGETSPECIALCAP      (0x325)          // Get Individual Special Control Capability

// Get Current Settings
#define QUERYGETSTANDARD        (0x340)          // Get Current Standard Setting
#define QUERYGETPICCONTROL      (0x341)          // Get Individual Picture Control Setting
#define QUERYGETFILTERCONTROL   (0x342)          // Get Individual Filter Control Setting  
#define QUERYGETPOSCONTROL      (0x343)          // Get Position Control Setting
#define QUERYGETSIZECONTROL     (0x344)          // Get Size Control Setting
#define QUERYGETSPECIAL         (0x345)          // Get Individual Special Control Setting
#define QUERYGETCONSTATUS       (0x346)          // Get Connector Status

// Set Settings
#define QUERYSETSTANDARD        (0x360)         // Set Current Standard Setting
#define QUERYSETPICCONTROL      (0x361)         // Set Individual Picture Control Setting
#define QUERYSETFILTERCONTROL   (0x362)         // Set Individual Filter Control Setting  
#define QUERYSETPOSCONTROL      (0x363)         // Set Individual Position Control Setting
#define QUERYSETSIZECONTROL     (0x364)         // Set Individual Size Control Setting
#define QUERYSSETSPECIAL        (0x365)         // Set Individual Special Control Setting
#define QUERYSETCONSTATUS       (0x366)         // Set Connector Status

// Registery Updates
#define QUERYCOMMITREG          (0x380)         // This should be called after the user hits ok
#define QUERYREFRESH            (0x381)         // This should be called if the user hits cancel

/*
**  Call this function first to know how if a TV Encoder is 
**  available and what general functionality it provides 
**
**  INPUT:      qin.dwSubFunc = QUERYTVAVAIL
**  OUTPUT: TVSTATUS
*/

typedef struct tvcon {
   DWORD dwType;                       // Type of the Connector <Composite, S-Video, SCART>
   DWORD dwStatus;                     // Connector Status <Tv Present/Enabled/Disabled>
} TVCON, FAR * LPTVCON, * PTVCON;

// To be used in the Type Field of TVCON
#define TV_TYPE_SVIDEO (0x00000001L)
#define TV_TYPE_COMPOSITE (0x00000002L)
#define TV_TYPE_SCART (0x00000004L)

// To be used in the Status Field of TVCON
#define TV_PRESENT          (0x00000001L)       // Did sense determine TV on connector ?
#define TV_CONNECTOR_ENABLED (0x00000002L)       // Is Connector Currently Enabled ?

#define MAX_CONNECTORS (0x000000004L)

// This structure defines what we are capable of 
typedef struct qtvstatus {
   DWORD dwEncoder;                       // Did I2C Query find a TV Encoder
   DWORD dwStandard;                      // Bit Map of Standard's Supported
   DWORD dwPicControl;                    // Bit Map of Picture Controls
   DWORD dwFilterControl;                 // Bit Map of Filter Controls
   DWORD dwPosControl;                    // Bit Map of Position Controls
   DWORD dwSizeControl;                   // Bit Map of Size Controls
   DWORD dwSpecial;                       // Bit Map of Special Controls
   DWORD dwNumSimultaneous;               // How many outputs can you support at the same time?
   DWORD dwNumConnectors;                 // How many connectors does the Encoder support
   TVCON TVCon[MAX_CONNECTORS];           // TV Connector Status
   BYTE  szName[10];                      // Name of the Encoder <Ch7003, Ch7004, BT868, BT869>
   } QTVSTATUS, FAR * LPQTVSTATUS, * PTVSTATUS;

// To be used in TVStatus.dwEncoder
#define TV_ENCODER_PRESENT (0x00000001L)

// Define the TV Standards
// I have decided to define my own since Microsoft does not 
// have a standard for PAL_Nc or PAL_N for Argentina
// To be used in TVStatus.dwStandard, TVGetStandard.dwStandard, TVSetStandard.dwStandard
#define TV_STANDARD_NTSC             (0x80000000L) /* NTSC Default: RS170A timing - older standard */
#define TV_STANDARD_NTSCRS170A       (0x80000001L) /* NTSC RS170A timing */
#define TV_STANDARD_NTSCM            (0x80000002L) /* NTSC-M, CCIR 601 timing */
#define TV_STANDARD_NTSCN            (0x80000004L) /* NTSC-N, */
#define TV_STANDARD_NTSC443          (0x80000008L) /* NTSC-4.43, has a 4.43Mhz color burst */
#define TV_STANDARD_NTSCJAPAN        (0x80000010L) /* NTSC-M, with a 0 IRE setup (blank to black distance) */

#define TV_STANDARD_PAL              (0x40000000L) /* PAL Default: B/G, H, I, D */
// Check on if I can group these together ?? How is each set ??
#define TV_STANDARD_PALBDGHI         (0x40000100L) /* PAL-B/G, H, I, D */ 
#define TV_STANDARD_PALM             (0x40000200L) /* PAL-M */
#define TV_STANDARD_PALN             (0x40000400L) /* PAL-N (not for Argentina) */
#define TV_STANDARD_PALN_ARGENTINA   (0x40000800L) /* PAL-N for Argentina */

#define TV_STANDARD_SECAM            (0x20000000L) /* SECAM Default: D/K/K1/L */
#define TV_STANDARD_SECAMDKK1L       (0x20010000L) /* SECAM D, K, K1, L */
#define TV_STANDARD_SECAMBG          (0x20020000L) /* SECAM B, G */

#define TV_STANDARD_MAJORSTANDARD(x) ((x) & 0xF0000000)
#define TV_STANDARD_MAJOR_NTSC       (0x80000000)
#define TV_STANDARD_MAJOR_PAL        (0x40000000)
#define TV_STANDARD_MAJOR_SECAM      (0x20000000)

// To be used in TVStatus.dwPicControl <Note: Flag Set implies capability present>
#define TV_BRIGHTNESS (0x00000001)
#define TV_CONTRAST   (0x00000002)
#define TV_GAMMA      (0x00000004)
#define TV_HUE        (0x00000008)
#define TV_SATURATION (0x00000010)
#define TV_SHARPNESS  (0x00000020)

// To be used in TVStatus.dwFilter <Note: Flag Set implies capability present>
#define TV_FLICKER (0x00000001)
#define TV_CHROMA  (0x00000002)
#define TV_LUMA    (0x00000004)

// To be used in TVStatus.dwPosControl <Note: Flag Set implies capability present>
#define TV_HORIZONTAL (0x00000001)
#define TV_VERTICAL   (0x00000002)

// To be used in TVStatus.dwSizeControl <Note: Flag Set implies capability present>
#define TV_UNDERSCAN          (0x00000001)
#define TV_OVERSCAN           (0x00000002)
#define TV_ADJUST_UNDERSCAN   (0x00000004)
#define TV_ADJUST_OVERSCAN    (0x00000008)

// To be used in TVStatus.dwSpecial <Note: Flag Set implies capability present>
#define TV_CLOSED_CAPTION           (0x00000001)
#define TV_MACROVISION              (0x00000002)

/*
**  Call this function to request the Driver to perform
**  a sense on every connector
**
**  INPUT:      qin.dwSubFunc = QUERYTVSENSE
**  OUTPUT: TVCONSTATUS
*/

typedef struct tvconstatus {
   DWORD dwNumConnectors;                 // How many connectors does the Encoder support
   TVCON TVCon[MAX_CONNECTORS];           // TV Connector Status
   } TVCONSTATUS, FAR * LPTVCONSTATUS, * PTVCONSTATUS;

/****************************************************************************
**
** Get Capabilities
**
****************************************************************************/

typedef struct qind {
   DWORD dwSubFunc;                 // Sub Function
   DWORD dwCap;                     // Capability of Interest <one capability at a time>
} QIND, FAR * LPQIND, * PQIND;

/*
**  Call this function to request the Driver to
**  report individual Picture Capability
**
**  INPUT:      qind.dwSubFunc = QUERYGETPICCAP
**              qind.dwCap = TV_BRIGHTNESS |
**                           TV_CONTRAST |
**                           TV_GAMMA |
**                           TV_HUE |      
**                           TV_SATURATION |
**                           TV_SHARPNESS 
**  OUTPUT: TVCAPDATA      
*/

typedef struct tvcapdata {
   DWORD dwCap;                     // Capability of Interest <one capability at a time>
   DWORD dwNumSteps;                // Number of Steps Supported by Hardware
   } TVCAPDATA, FAR * LPTVCAPDATA, * PTVCAPDATA;                    

/*
**  Call this function to request the Driver to
**  report all Position Capability
**
**  INPUT:      qin.dwSubFunc = QUERYGETPOSITIONCAP
**  OUTPUT:       TVPOSCAP
**  Note this is a good match for Chrontel but unknown now to do this on Brooktree
*/
typedef struct tvposcap {
   DWORD  dwMaxLeft;               // Maximum Left Position in Hardware Units
   DWORD  dwMaxRight;              // Maximum Right Position in Hardware Units
   DWORD  dwHorGranularity;        // Size of Movement in Pixels
   DWORD  dwMaxTop;                // Maximum Top Position in Hardware Units
   DWORD  dwMaxBottom;             // Maximum Bottom Position in Hardware Units
   DWORD  dwVGAGranularity;        // Granularity in VGA Lines
   } TVPOSCAP, FAR * LPTVPOSCAP, * PTVPOSCAP;  

/*
**  Call this function to request the Driver to
**  report all Size Capability
**
**  INPUT:      qin.dwSubFunc = QUERYGETSIZECAP
**  OUTPUT:       TVSIZECAP
**  Note this is a good match for BrookTree but unknown now to do this on Chrontel
*/
typedef struct tvsizecap {
   DWORD  dwMaxHorInput;               // Maximum Input Horizontal Size in Active Pixels
   DWORD  dwMaxVerInput;               // Maximum Input Vertical Size in Active Lines
   DWORD  dwMaxHorOutput;              // Maximum Output Horizontal Size in Active Pixels
   DWORD  dwMaxVerOutput;              // Maximum Output Vertical Size in Active Pixels
   DWORD  dwMinHorInput;               // Minimum Input Horizontal Size in Active Pixels
   DWORD  dwMinVerInput;               // Minimum Input Vertical Size in Active Lines
   DWORD  dwMinHorOutput;              // Minimum Output Horizontal Size in Active Pixels
   DWORD  dwMinVerOutput;              // Minimum Output Vertical Size in Active Pixels
   DWORD  dwHorStepSize;               // In Percentage * 1000
   DWORD  dwVerStepSize;               // In Percentage * 1000
   } TVSIZECAP, FAR * LPTVSIZECAP, * PTVSIZECAP;  

/*
**  Call this function to request the Driver to
**  report on a Special Capability
**
**  INPUT:      qind.dwSubFunc = QUERYGETSPECIALCAP
**              qind.dwCap = TV_CLOSED_CAPTION |
**                           TV_MACROVISION
**  OUTPUT:       TVSPECCAP <TBD>
*/

/****************************************************************************
**
** Get Current Settings
**
****************************************************************************/

/*
**  Call this function to request the Driver to
**  report current Standard Setting
**
**  INPUT:      qin.dwSubFunc = QUERYGETSTANDARD
**  OUTPUT: TVGETSTANDARD      
*/
typedef struct tvgetstandard {
   DWORD dwStandard;                   // Standard Currently in Use
} TVGETSTANDARD, FAR * LPTVGETSTANDARD, * PTVGETSTANDARD;

/*
**  Call this function to request the Driver to
**  report current individual Picture Capability
**
**  INPUT:      qind.dwSubFunc = QUERYGETPICCONTROL
**              qind.dwCap = TV_BRIGHTNESS |
**                           TV_CONTRAST |
**                           TV_GAMMA |
**                           TV_HUE |      
**                           TV_SATURATION |
**                           TV_SHARPNESS 
**  OUTPUT: TVCURCAP      
*/

typedef struct tvcurcap {
   DWORD dwCap;                 // Capability Requested
   DWORD dwStep;                // Current Hardware Step we are on
   } TVCURCAP, FAR * LPTVCURCAP, * PTVCURCAP;                    

/*
**  Call this function to request the Driver to
**  report current individual Filter Capability
**
**  INPUT:      qind.dwSubFunc = QUERYGETFILTERCONTROL
**              qind.dwCap = TV_FLICKER |
**                           TV_CHROMA |
**                           TV_LUMA
**  OUTPUT: TVCURCAP
*/

/*
**  Call this function to request the Driver to
**  report all current Position Capability
**
**  INPUT:      qin.dwSubFunc = QUERYGETPOSITIONCONTROL
**  OUTPUT:       TVCURPOS
**  Note this is a good match for Chrontel but unknown now to do this on Brooktree
*/
typedef struct tvcurpos {
   DWORD  dwCurLeft;               // Current Left Position in Hardware Units
   DWORD  dwCurRight;              // Current Right Position in Hardware Units
   DWORD  dwCurTop;                // Current Top Position in Hardware Units
   DWORD  dwCurBottom;             // Current Bottom Position in Hardware Units
   } TVCURPOS, FAR * LPTVCURPOS, * PTVCURPOS;  

/*
**  Call this function to request the Driver to
**  report current Size Capability
**
**  INPUT:      qin.dwSubFunc = QUERYGETSIZECONTROL
**  OUTPUT:       TVCURSIZE
**  Note this is a good match for BrookTree but unknown now to do this on Chrontel
*/
typedef struct tvcursize {
   DWORD  dwCurHorInput;               // Current Input Horizontal Size in Active Pixels
   DWORD  dwCurVerInput;               // Current Input Vertical Size in Active Lines
   DWORD  dwCurHorOutput;              // Current Output Horizontal Size in Active Pixels
   DWORD  dwCurVerOutput;              // Current Output Vertical Size in Active Pixels
   } TVCURSIZE, FAR * LPTVCURSIZE, * PTVCURSIZE;  

/*
**  Call this function to request the Driver to
**  report on current a Special Capability
**
**  INPUT:      qind.dwSubFunc = QUERYGETSPECIAL
**              qind.dwCap = TV_CLOSED_CAPTION |
**                           TV_MACROVISION
**  OUTPUT:       TVCURSPEC <TBD>
*/

/*
**  Call this function to request the Driver to return
**  in memory data on every connector
**
**  INPUT:      qin.dwSubFunc = QUERYGETCONSTATUS
**  OUTPUT: TVCONSTATUS
*/


/****************************************************************************
**
** Set Functions
**
****************************************************************************/

/*
**  Call this function to request the Driver to
**  change the current Standard Setting
**
**  INPUT:      TvSetStandard.dwSubFunc = QUERYSETSTANDARD
**              TvSetStandard.dwStandard = TV_STANDARD_NTSCRS170A |      
**                                         TV_STANDARD_NTSCM |           
**                                         TV_STANDARD_NTSCN |           
**                                         TV_STANDARD_NTSC443 |         
**                                         TV_STANDARD_NTSCJAPAN |        
**                                         TV_STANDARD_PALBDGHI |         
**                                         TV_STANDARD_PALM  |           
**                                         TV_STANDARD_PALN  |           
**                                         TV_STANDARD_PALN_ARGENTINA |  
**
**  OUTPUT: None
*/
typedef struct tvsetstandard {
   DWORD dwSubFunc;                    // Sub Function
   DWORD dwStandard;                   // Standard Currently in Use
} TVSETSTANDARD, FAR * LPTVSETSTANDARD, * PTVSETSTANDARD;

/*
**  Call this function to request the Driver to
**  set individual Picture Capability
**
**  INPUT:      TVSetCap.dwSubFunc = QUERYSETPICCONTROL
**              TVSetCap.dwCap = TV_BRIGHTNESS |
**                               TV_CONTRAST |
**                               TV_GAMMA |
**                               TV_HUE |      
**                               TV_SATURATION |
**                               TV_SHARPNESS 
**              TVSetCap.dwStep =
**  OUTPUT: None      
*/

typedef struct tvsetcap {
   DWORD dwSubFunc;             // SubFunction
   DWORD dwCap;                 // Capability Requested
   DWORD dwStep;                // Current Hardware Step we are on
   } TVSETCAP, FAR * LPTVSETCAP, * PTVSETCAP;                    

/*
**  Call this function to request the Driver to
**  set individual Filter Capability
**
**  INPUT:      TVSetCap.dwSubFunc = QUERYSETFILTERCONTROL
**              TVSetCap.dwCap = TV_FLICKER |
**                               TV_CHROMA |
**                               TV_LUMA
**              TVSetCap.dwStep = 
**  OUTPUT: None
*/

/*
**  Call this function to request the Driver to
**  move the Output Image 
**
**  INPUT:      TVSetPos.dwSubFunc = QUERYSETPOSCONTROL
**              TVSetPos.dwLeft = 
**              TVSetPos.dwRight = 
**              TVSetPos.dwTop = 
**              TVSetPos.dwBottom = 
**  OUTPUT:       None
**  Note this is a good match for Chrontel but unknown now to do this on Brooktree
*/
typedef struct tvsetpos {
   DWORD  dwSubFunc;            // SubFunction
   DWORD  dwLeft;               // Left Position in Hardware Units
   DWORD  dwRight;              // Right Position in Hardware Units
   DWORD  dwTop;                // Top Position in Hardware Units
   DWORD  dwBottom;             // Bottom Position in Hardware Units
   } TVSETPOS, FAR * LPTVSETPOS, * PTVSETPOS;  

/*
**  Call this function to request the Driver to
**  set the resize the input/output 
**
**  INPUT:      TVSetSize.dwSubFunc = QUERYSETSIZECONTROL
**              TvSetSize.dwHorOutput =
**              TvSetSize.dwVerOutput =
**  OUTPUT:     None
**  Note this is a good match for BrookTree but unknown now to do this on Chrontel
*/
typedef struct tvsetsize {
   DWORD  dwSubFunc;                   // SubFunction
   DWORD  dwHorOutput;                 // Output Horizontal Size in Active Pixels
   DWORD  dwVerOutput;                 // Output Vertical Size in Active Lines
   } TVSETSIZE, FAR * LPTVSETSIZE, * PTVSETSIZE;  

/*
**  Call this function to request the Driver to
**  set the a Special Capability
**
**  INPUT:      TBD
**  OUTPUT:     None
*/

/*
**  Call this function to request the Driver to enable/disable
**  connectors
**
**  INPUT:      TVSetConnector.dwSubFunc = QUERYSETCONSTATUS
**              Note i can range from 0 to < dwNumSimultaneous
**              TVSetConnector.TVCon[i].dwType = TV_TYPE_SVIDEO | TV_TYPE_COMPOSITE | TV_TYPE_SCART
**              TVSetConnector.TVCon[i].dwStatus = TV_CONNECTOR_ENABLED |  0x00
**
**  OUTPUT: None
*/

typedef struct tvsetconnector {
   DWORD dwSubFunc;                       // SubFunction
   TVCON TVCon[MAX_CONNECTORS];           // TV Connector Status
   } TVSETCONNECTOR, FAR * LPTVSETCONNECTOR, * PTVSETCONNECTOR;

/****************************************************************************
**
** Registry Functions
**
****************************************************************************/
/*
**  Call this function to request the Driver to save
**  in memory data to registry
**
**  INPUT:      qin.dwSubFunc = QUERYCOMMITREG
**  OUTPUT: None
*/

/*
**  Call this function to request the Driver to refresh
**  in memory data with registry data
**
**  INPUT:      qin.dwSubFunc = QUERYREFRESH
**  OUTPUT: None
*/

#endif
