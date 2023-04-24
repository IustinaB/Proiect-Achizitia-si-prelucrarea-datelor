//==============================================================================
//
// Title:		SwitchPanels
// Purpose:		A short description of the application.
//
// Created on:	11/5/2020 at 4:51:51 PM by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "SwitchPanels.h"
#include "toolbox.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

int wavePanel = 0;
int acqPanel = 0;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
	int error = 0;
	
	/* initialize and load resources */
	nullChk (InitCVIRTE (0, argv, 0));
	errChk (wavePanel = LoadPanel (0, "SwitchPanels.uir", WAVE_PANEL));
	errChk (acqPanel = LoadPanel (0, "SwitchPanels.uir", ACQ_PANEL));
	
	/* display the panel and run the user interface */
	errChk (DisplayPanel (wavePanel));
	errChk (RunUserInterface ());

Error:
	/* clean up */
	if (wavePanel > 0)
		DiscardPanel (wavePanel);
	return 0;
}

//==============================================================================
// UI callback function prototypes

/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK OnWavePanelCB (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
		QuitUserInterface (0);
	return 0;
}

int CVICALLBACK OnSwitchPanelCB (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(panel == wavePanel)
			{
				SetCtrlVal(acqPanel, ACQ_PANEL_IDC_SWITCH_PANEL, 1);
				DisplayPanel(acqPanel);
				HidePanel(panel);
			}
			else
			{
				SetCtrlVal(wavePanel, ACQ_PANEL_IDC_SWITCH_PANEL, 0);
				DisplayPanel(wavePanel);
				HidePanel(panel);
			}
			break;
	}
	return 0;
}

