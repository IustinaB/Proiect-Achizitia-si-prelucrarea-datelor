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

#define  FREQ_PANEL                       1       /* callback function: OnFrequencyPanel */
#define  FREQ_PANEL_SWITCH_PANEL          2       /* control type: binary, callback function: OnSwitchPanelCB */
#define  FREQ_PANEL_NR_PUNCTE             3       /* control type: ring, callback function: (none) */
#define  FREQ_PANEL_GRAPH_SPECTRU_SF      4       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_GRAPH_WINDOWING       5       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_GRAPH_SF              6       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_RING_FILTER_TYPE      7       /* control type: ring, callback function: OnFilterTypeFreq */
#define  FREQ_PANEL_RING_TIP_AFISARE      8       /* control type: ring, callback function: OnShow */
#define  FREQ_PANEL_RING_WINDOW_TYPE      9       /* control type: ring, callback function: OnWindowType */
#define  FREQ_PANEL_GRAPH_FEREASTRA       10      /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_GRAPH_SPECTRU_SI      11      /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_GRAPH_INITIAL         12      /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_RING_SECUNDE          13      /* control type: ring, callback function: OnSecundaFreq */
#define  FREQ_PANEL_NUMERIC_FREQ1         14      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_NUMERIC_FREQ2         15      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_NUMERIC_POWER2        16      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_NUMERIC_POWER1        17      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_COMMANDBUTTON_SAVE    18      /* control type: command, callback function: OnSave2 */
#define  FREQ_PANEL_TIMER                 19      /* control type: timer, callback function: OnTimer */

#define  PANEL                            2       /* callback function: OnWavePanel */
#define  PANEL_GRAPH_RAW_DATA             2       /* control type: graph, callback function: (none) */
#define  PANEL_GRAPH_FILTERED_DATA        3       /* control type: graph, callback function: (none) */
#define  PANEL_COMMANDBUTTON_PREV         4       /* control type: command, callback function: OnPrev */
#define  PANEL_COMMANDBUTTON_NEXT         5       /* control type: command, callback function: OnNext */
#define  PANEL_COMMANDBUTTON_3            6       /* control type: command, callback function: OnLoad */
#define  PANEL_RING_FILTER_TYPE           7       /* control type: ring, callback function: OnFilterType */
#define  PANEL_COMMANDBUTTON_APLICA       8       /* control type: command, callback function: OnAplica */
#define  PANEL_NUMERIC_MAXIM              9       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_MEAN               10      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_MINIM              11      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG                    12      /* control type: textMsg, callback function: (none) */
#define  PANEL_NUMERIC_INDEX_MAXIM        13      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_INDEX_MINIM        14      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_MEDIAN             15      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_DISPERSIE          16      /* control type: numeric, callback function: (none) */
#define  PANEL_GRAPH_HISTOGRAMA           17      /* control type: graph, callback function: (none) */
#define  PANEL_NUMERIC_TRECERI            18      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_START              19      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_STOP               20      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_ALPHA              21      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_DIMENSIUNE         22      /* control type: numeric, callback function: (none) */
#define  PANEL_COMMANDBUTTON_SAVE         23      /* control type: command, callback function: OnSave */
#define  PANEL_COMMANDBUTTON_DERIV        24      /* control type: command, callback function: OnDeriv */
#define  PANEL_COMMANDBUTTON_ENVELOP      25      /* control type: command, callback function: OnAnvelopa */
#define  PANEL_SWITCH_PANEL               26      /* control type: binary, callback function: OnSwitchPanelCB */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnAnvelopa(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnAplica(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDeriv(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFilterType(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFilterTypeFreq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFrequencyPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoad(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnNext(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPrev(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSave(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSave2(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSecundaFreq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnShow(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSwitchPanelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnWavePanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnWindowType(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif