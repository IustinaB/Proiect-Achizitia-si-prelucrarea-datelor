/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  ACQ_PANEL                        1       /* callback function: OnAcquisitionPanelCB */
#define  ACQ_PANEL_IDC_SWITCH_PANEL       2       /* control type: binary, callback function: OnSwitchPanelCB */
#define  ACQ_PANEL_TEXTMSG                3       /* control type: textMsg, callback function: (none) */

#define  WAVE_PANEL                       2       /* callback function: OnWavePanelCB */
#define  WAVE_PANEL_IDC_SWITCH_PANEL      2       /* control type: binary, callback function: OnSwitchPanelCB */
#define  WAVE_PANEL_TEXTMSG               3       /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnAcquisitionPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSwitchPanelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnWavePanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif