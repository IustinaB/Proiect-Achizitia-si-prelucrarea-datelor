#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "interfata.h"
#include <formatio.h>
#include <stdlib.h>
#include <analysis.h>
#include "toolbox.h"

int wavePanel = 0;
int freqPanel = 0;

int waveInfo[2]; //waveInfo[0] = sampleRate
				 //waveInfo[1] = number of elements
double sampleRate = 0.0;
int npoints = 0;
double *waveData = 0;
double *waveDatap=0; //vectorul initial partitionat
int valStart=0;
int valStop=0;

int dimFereastra=0;
char filtru[2];
int filterType=0; 
double alpha=0.0;
int flag=0; //pentru a sti daca e semnalul intreg sau sunt pe secunde
double *filt=0; //vectorul filtrat
double*filt1=0; //vectorul filtrat partitionat
int flag_deriv=1;
double*deriv=0;
double*deriv1=0;
double*anv=0;
double* anv1=0;
int flag_anv=0;

int numarpuncte;
double* semnalFiltrat=0;
double* semnalFF=0;

int nrFereastra; //fereastra curenta pentru afisarea pe numarul de puncte
int numarFerestre;
double rate = 0;
int tipFiltru; //chebyshev II trece jos pentru 2/3 din spectrul semnalului si un filtru Notch pentru frecventa de 1000 Hz
int tipFereastra;//blackman sau dreptunghiular
int tipAfisare;
int secunda;
double *waveDataPerNrPuncte;
double *wavepersec;
#define SAMPLE_RATE		0
#define NPOINTS			1

void AfisarePeSecunde(double *waveDatap, double*waveData);
void Mediere(int dim);
void FiltrareOrdinI(double alpha);
void SalvarePeSecunde(int panel, int valStart, int valStop);

void AfisareAnvelopa(int panel, double*vect, int dim);
void afispesecunde(int panel);
void afisSemnalIntreg(int panel);
void aplicareFiltru(int panel);
void aplicareFereastra(int panel);
void afisareSpectru(int panel);

int main (int argc, char *argv[])
{
	int error=0;
	nullChk(InitCVIRTE(0,argv,0));
	errChk(wavePanel=LoadPanel(0,"interfata.uir",PANEL));
	errChk(freqPanel=LoadPanel(0,"interfata.uir",FREQ_PANEL));
		   
	errChk(DisplayPanel(wavePanel));
	errChk(RunUserInterface());
	
Error:
	if(wavePanel>0)
		DiscardPanel(wavePanel);
	/*if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	/*if ((panelHandle = LoadPanel (0, "interfata.uir", PANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	*/
	free(anv1);
	free(anv);
	free(filt1);
	free(filt);
	free(waveDatap);
	free(waveData);
	
	return 0;
}

void AfisarePeSecunde(double *waveDatap, double *waveData)
{
	for(int i=0;i<npoints/6;i++)
	{
		waveDatap[i]=waveData[i+valStart*npoints/6];
	}
	
}

int CVICALLBACK OnLoad (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	double maxVal=0.0;
	double minVal=0.0;
	int maxIndex=0;
	int minIndex=0;
	double mean=0.0;
	double median=0.0;
	double stddev=0.0;
	int interv=20;
	double axis[100];
	int hist[100];
	int contor=0;
	switch (event)
	{
		case EVENT_COMMIT:
			flag=0;
			flag_deriv=0;
			flag_anv=0;
			DeleteGraphPlot (panel,PANEL_GRAPH_FILTERED_DATA, -1, VAL_IMMEDIATE_DRAW);
			//executa script python pentru conversia unui fisierului .wav in .txt
			//LaunchExecutable("python main.py");
			
			//astept sa fie generate cele doua fisiere (modificati timpul daca este necesar)
			//Delay(4);
			
			//incarc informatiile privind rata de esantionare si numarul de valori
			FileToArray("wafeInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			sampleRate = waveInfo[SAMPLE_RATE];
			npoints = waveInfo[NPOINTS];
			
			//alocare memorie pentru numarul de puncte
			waveData = (double *) calloc(npoints, sizeof(double));
			waveDatap=(double *)calloc(npoints/6, sizeof(double));
		
			filt=(double *) malloc(npoints*sizeof(double));
			filt1=(double*)malloc(npoints/6*sizeof(double));
			//incarcare din fisierul .txt in memorie (vector)
			FileToArray("waveData.txt", waveData, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			//afisare pe graph
			DeleteGraphPlot (panel,PANEL_GRAPH_RAW_DATA, -1, VAL_DELAYED_DRAW);
			PlotY(panel, PANEL_GRAPH_RAW_DATA, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			//calculare maxim, minim, dispersie, medie, mediana, numar treceri prin 0, histograma
			MaxMin1D(waveData,npoints,&maxVal,&maxIndex,&minVal,&minIndex);
			//calculare valoare medie
			Mean(waveData,npoints,&mean);
			//calculare valoare mediana
			Median(waveData,npoints,&median);
			//calculare dispersie
			StdDev(waveData,npoints,&mean,&stddev);
			//histograma
			Histogram(waveData,npoints,minVal-1,maxVal+1,hist,axis,interv);
			DeleteGraphPlot (panel,PANEL_GRAPH_HISTOGRAMA, -1, VAL_DELAYED_DRAW);
            PlotXY (panel,PANEL_GRAPH_HISTOGRAMA, axis,  hist, interv, VAL_DOUBLE, VAL_SSIZE_T, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
			//numarul de treceri prin 0
			for(int i=0;i<npoints-1;i++)
				if(((waveData[i]>0) && (waveData[i+1]<0)) || ((waveData[i]<0) && (waveData[i+1]>0)) || (waveData[i]==0))
					contor++;
			
			SetCtrlVal(panel,PANEL_NUMERIC_MAXIM,maxVal);
			SetCtrlVal(panel,PANEL_NUMERIC_MINIM,minVal);
			SetCtrlVal(panel,PANEL_NUMERIC_INDEX_MAXIM,maxIndex);
			SetCtrlVal(panel,PANEL_NUMERIC_INDEX_MINIM,minIndex);
			SetCtrlVal(panel,PANEL_NUMERIC_MEDIAN,median);
			SetCtrlVal(panel,PANEL_NUMERIC_MEAN,mean);
			SetCtrlVal(panel,PANEL_NUMERIC_DISPERSIE,stddev);
			SetCtrlVal(panel,PANEL_NUMERIC_TRECERI,contor);
			
			
			break;
	}
	return 0;
}

int CVICALLBACK OnWavePanel (int panel, int event, void *callbackData,
							 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK OnPrev (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_COMMIT:
			flag=1;
			GetCtrlVal(panel,PANEL_NUMERIC_START,&valStart);
			GetCtrlVal(panel,PANEL_NUMERIC_STOP,&valStop);
			
			if(valStart>0)
			{
				SetCtrlVal(panel,PANEL_NUMERIC_START,valStart-1);
				SetCtrlVal(panel,PANEL_NUMERIC_STOP,valStop-1);
			}
			GetCtrlVal(panel,PANEL_NUMERIC_START,&valStart);
			GetCtrlVal(panel,PANEL_NUMERIC_STOP,&valStop);
			AfisarePeSecunde(waveDatap,waveData);
			DeleteGraphPlot (panel,PANEL_GRAPH_FILTERED_DATA, -1, VAL_DELAYED_DRAW);
			DeleteGraphPlot (panel,PANEL_GRAPH_RAW_DATA, -1, VAL_DELAYED_DRAW);
			PlotY(panel,PANEL_GRAPH_RAW_DATA,waveDatap,npoints/6,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_RED);
			
			if(filterType==1)
			{
				DeleteGraphPlot (panel,PANEL_GRAPH_FILTERED_DATA, -1, VAL_DELAYED_DRAW);
				AfisarePeSecunde(filt1,filt);
				PlotY(panel,PANEL_GRAPH_FILTERED_DATA,filt1,npoints/6,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_BLUE);
				
			}
			else
				if(filterType==2)
				{
					DeleteGraphPlot (panel,PANEL_GRAPH_FILTERED_DATA, -1, VAL_DELAYED_DRAW);
					AfisarePeSecunde(filt1,filt);
					PlotY(panel,PANEL_GRAPH_FILTERED_DATA,filt1,npoints/6,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_GREEN);
					
				}
			if(flag_deriv==1)
			{
				AfisarePeSecunde(deriv1,deriv);
				PlotY(panel,PANEL_GRAPH_FILTERED_DATA,deriv1,npoints/6,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_MAGENTA);
			}
			if(flag_anv==1)
			{
				AfisarePeSecunde(anv1,anv);
				PlotY(panel,PANEL_GRAPH_RAW_DATA,anv1,npoints/6,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_DK_GREEN);
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnNext (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			flag=1;
			GetCtrlVal(panel,PANEL_NUMERIC_START,&valStart);
			GetCtrlVal(panel,PANEL_NUMERIC_STOP,&valStop);
			if(valStop<6)
			{
			SetCtrlVal(panel,PANEL_NUMERIC_START,valStart+1);
			SetCtrlVal(panel,PANEL_NUMERIC_STOP,valStop+1);
			}
			GetCtrlVal(panel,PANEL_NUMERIC_START,&valStart);
			GetCtrlVal(panel,PANEL_NUMERIC_STOP,&valStop);
			AfisarePeSecunde(waveDatap,waveData);
			DeleteGraphPlot (panel,PANEL_GRAPH_FILTERED_DATA, -1, VAL_DELAYED_DRAW);
			DeleteGraphPlot (panel,PANEL_GRAPH_RAW_DATA, -1, VAL_DELAYED_DRAW);
			PlotY(panel,PANEL_GRAPH_RAW_DATA,waveDatap,npoints/6,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_RED);
			
			
			if(filterType==1)
			{
				
				AfisarePeSecunde(filt1,filt);
				DeleteGraphPlot (panel,PANEL_GRAPH_FILTERED_DATA, -1, VAL_DELAYED_DRAW);
				PlotY(panel,PANEL_GRAPH_FILTERED_DATA,filt1,npoints/6,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_BLUE);
			}
			
			else
				if(filterType==2)
				{
				AfisarePeSecunde(filt1,filt);
				DeleteGraphPlot (panel,PANEL_GRAPH_FILTERED_DATA, -1, VAL_DELAYED_DRAW);
				PlotY(panel,PANEL_GRAPH_FILTERED_DATA,filt1,npoints/6,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_GREEN);
				}
			
			if(flag_deriv==1)
			{
				AfisarePeSecunde(deriv1,deriv);
				PlotY(panel,PANEL_GRAPH_FILTERED_DATA,deriv1,npoints/6,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_MAGENTA);
			}
			if(flag_anv==1)
			{
				AfisarePeSecunde(anv1,anv);
				PlotY(panel,PANEL_GRAPH_RAW_DATA,anv1,npoints/6,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_DK_GREEN);
			}
			break;
			
	}
	return 0;
}
void FiltrareOrdinI(double alpha)
{
	filt[0]=waveData[0];
	
	for(int i=1;i<npoints;i++)
		filt[i]=(1-alpha)*filt[i-1]+alpha*waveData[i];
	
}

void Mediere(int dim)
{
	double sum=0;
	for(int i=0;i<npoints;i++)
				{
					if(i<dim-1)
					{
						filt[i]=waveData[i];
						sum+=waveData[i];
					}
					else
						if(i==dim-1)
							filt[i]=sum/dim;
					else
					{
						sum=sum-waveData[i-dim]+waveData[i];
						filt[i]=sum/dim;
					}
				}
}

int CVICALLBACK OnAplica (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_COMMIT:
			flag_deriv=0;
			DeleteGraphPlot (panel,PANEL_GRAPH_FILTERED_DATA, -1, VAL_IMMEDIATE_DRAW);
			if(filterType==1)
			{
				
				//filtrare prin mediere
				GetCtrlVal(panel,PANEL_NUMERIC_DIMENSIUNE,&dimFereastra);
				Mediere(dimFereastra);
				
				if(flag==0)
					{
						PlotY(panel, PANEL_GRAPH_FILTERED_DATA, filt, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
					}
					else
					{
						AfisarePeSecunde(filt1,filt);
						PlotY(panel,PANEL_GRAPH_FILTERED_DATA,filt1,npoints/6,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_BLUE);
						
					}
			}
			else
				if(filterType==2)
				{
					//filtrare cu element de Ordin I
					GetCtrlVal(panel,PANEL_NUMERIC_ALPHA,&alpha);
					FiltrareOrdinI(alpha);
					
					if(flag==0)
					{
						PlotY(panel, PANEL_GRAPH_FILTERED_DATA, filt, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
					}
					else
					{
						AfisarePeSecunde(filt1,filt);
						PlotY(panel,PANEL_GRAPH_FILTERED_DATA,filt1,npoints/6,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_GREEN);
						
					}
				}
			
			break;
	}
		
	
	return 0;
}



int CVICALLBACK OnFilterType (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlAttribute(panel,PANEL_RING_FILTER_TYPE,ATTR_CTRL_INDEX,&filterType);
			if(filterType==1) //filtrare prin mediere
			{
				SetCtrlAttribute(panel,PANEL_NUMERIC_ALPHA,ATTR_DIMMED,1);
				SetCtrlAttribute(panel,PANEL_NUMERIC_DIMENSIUNE,ATTR_DIMMED,0);
			}
			else
				if(filterType==2) //cu element de ordin I
				{
					SetCtrlAttribute(panel,PANEL_NUMERIC_ALPHA,ATTR_DIMMED,0);
					SetCtrlAttribute(panel,PANEL_NUMERIC_DIMENSIUNE,ATTR_DIMMED,1);
				}
			break;
	}
	return 0;
	
}



int CVICALLBACK OnSave (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	int bitmapID;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlDisplayBitmap(panel,PANEL_GRAPH_RAW_DATA,1,&bitmapID);
			SaveBitmapToPNGFile(bitmapID,"RAW_DATA.png");
			DiscardBitmap(bitmapID);
			GetCtrlDisplayBitmap(panel,PANEL_GRAPH_FILTERED_DATA,1,&bitmapID);
			SaveBitmapToPNGFile(bitmapID,"FILTERED_DATA.png");
			DiscardBitmap(bitmapID);
			break;
	}
	return 0;
}




int CVICALLBACK OnDeriv (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			flag_deriv=1;
			deriv=(double*)malloc(npoints*sizeof(double));
			deriv1=(double*)malloc(npoints/6*sizeof(double));
			Difference(waveData,npoints,1.0,0.0,0.0,deriv);
			if(flag==0)
			{
				PlotY(panel,PANEL_GRAPH_FILTERED_DATA,deriv,npoints,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_MAGENTA);
			}
				else
				if(flag==1)
				{
					AfisarePeSecunde(deriv1,deriv);
					PlotY(panel,PANEL_GRAPH_FILTERED_DATA,deriv1,npoints/6,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_MAGENTA);
				}
			
			break;
	}
	return 0;
}

int CVICALLBACK OnAnvelopa (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			flag_anv=1;
			//LaunchExecutable("python envelope.py");
			
			//astept sa fie generate cele doua fisiere 
			//Delay(2);
			anv=(double*)malloc(npoints*sizeof(double));
			anv1=(double*)malloc(npoints/6*sizeof(double));
			FileToArray("anvelopa.txt", anv, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			if(flag==0)
			{
				PlotY(panel,PANEL_GRAPH_RAW_DATA,anv,npoints,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_DK_GREEN);
			}
			else if(flag==1)
			{
				AfisarePeSecunde(anv1,anv);
				PlotY(panel,PANEL_GRAPH_RAW_DATA,anv1,npoints/6,VAL_DOUBLE,VAL_THIN_LINE,VAL_EMPTY_SQUARE,VAL_SOLID,VAL_CONNECTED_POINTS,VAL_DK_GREEN);
			}
			break;
		
	}
	return 0;
}



int CVICALLBACK OnFrequencyPanel (int panel, int event, void *callbackData,
								  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:

			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK OnSwitchPanelCB (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(panel==wavePanel)
			{
				SetCtrlVal(freqPanel, FREQ_PANEL_SWITCH_PANEL,1);
				DisplayPanel(freqPanel);
				HidePanel(panel);
				//SetCtrlAttribute(freqPanel,FREQ_PANEL_TIMER, ATTR_ENABLED,0);
			}
			else
			{
				SetCtrlVal(wavePanel,PANEL_SWITCH_PANEL,0);
				DisplayPanel(wavePanel);
				HidePanel(panel);
				//SetCtrlAttribute(freqPanel,FREQ_PANEL_TIMER, ATTR_ENABLED,0);
			}
			break;
	}
	
	
	return 0;
}



int CVICALLBACK OnWindowType (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			aplicareFereastra(panel);
			
			
			break;
	}
	return 0;
}

int CVICALLBACK OnSecundaFreq (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			afispesecunde(panel);
			break;
	}
	return 0;
}

void aplicareFiltru(int panel)
{
	GetCtrlAttribute(panel,FREQ_PANEL_RING_FILTER_TYPE,ATTR_CTRL_INDEX,&tipFiltru);
	
	//filtru
	if(tipAfisare==1) //afisare pe nr de puncte
	{
		semnalFiltrat=(double *) malloc(numarpuncte*numarFerestre*sizeof(double));
		if(tipFiltru==1)
		{
			
			InvCh_LPF(waveDataPerNrPuncte,numarpuncte*numarFerestre,sampleRate,sampleRate/3.0,40,7,semnalFiltrat);
						
			DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_SF, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, FREQ_PANEL_GRAPH_SF,semnalFiltrat+nrFereastra*numarpuncte,numarpuncte, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);		
			aplicareFereastra(panel);
		}
		else
			if(tipFiltru==2) //Notch
			{
				double Numerator[3], Denominator[3];
				Numerator[0]=0.958216646376661040918065737059805542231;
				Numerator[1]=-1.897014980249072779727725901466328650713;
				Numerator[2]= 0.958216646376661040918065737059805542231;
					
				Denominator[0]=1;
				Denominator[1]=-1.897014980249072779727725901466328650713;
				Denominator[2]= 0.916433292753322081836131474119611084461;
					
					
				semnalFiltrat[0]=waveData[0]; 
				semnalFiltrat[1]=waveData[1];// 
				for(int i=2;i<numarpuncte;i++) //dimensiunea bufferului =1024 sau se inlocuieste cu numarul de esantioane
				{
					semnalFiltrat[i]=Numerator[0]*waveData[i+numarpuncte*nrFereastra]+Numerator[1]*waveData[(i-1)+numarpuncte*nrFereastra]+Numerator[2]*waveData[(i-2)+nrFereastra*numarpuncte] -Denominator[1]*semnalFiltrat[i-1]-Denominator[2]*semnalFiltrat[i-2];
				}
				DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_SF, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, FREQ_PANEL_GRAPH_SF,semnalFiltrat,numarpuncte, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);		
				aplicareFereastra(panel);
			}
	}
	else
		if(tipAfisare==2) //semnal intreg
		{
			semnalFiltrat=(double *) malloc(npoints*sizeof(double));
			if(tipFiltru==1)
		{
			
			InvCh_LPF(waveData,npoints,sampleRate,sampleRate/3.0,40,20,semnalFiltrat);
			DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_SF, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, FREQ_PANEL_GRAPH_SF,semnalFiltrat,npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);		
			aplicareFereastra(panel);	
		}
		else
			if(tipFiltru==2) //Notch
			{
				double Numerator[3], Denominator[3];
				Numerator[0]=0.958216646376661040918065737059805542231;
				Numerator[1]=-1.897014980249072779727725901466328650713;
				Numerator[2]= 0.958216646376661040918065737059805542231;
					
				Denominator[0]=1;
				Denominator[1]=-1.897014980249072779727725901466328650713;
				Denominator[2]= 0.916433292753322081836131474119611084461;
					
					
					
				semnalFiltrat[0]=waveData[0]; 
				semnalFiltrat[1]=waveData[1];// 
				for(int i=2;i<npoints;i++) //dimensiunea bufferului =1024 sau se inlocuieste
				{
					semnalFiltrat[i]=Numerator[0]*waveData[i]+Numerator[1]*waveData[i-1]+Numerator[2]*waveData[i-2] -Denominator[1]*semnalFiltrat[i-1]-Denominator[2]*semnalFiltrat[i-2];
				}
				DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_SF, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, FREQ_PANEL_GRAPH_SF,semnalFiltrat,npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);		
				aplicareFereastra(panel);
			}
		}
		else
			if(tipAfisare==3)
			{
				semnalFiltrat=(double *) malloc(npoints/6*sizeof(double));
				if(tipFiltru==1)
			{
				InvCh_LPF(wavepersec,npoints/6,sampleRate,sampleRate/3,40,7,semnalFiltrat);
				DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_SF, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, FREQ_PANEL_GRAPH_SF,semnalFiltrat,npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);		
				aplicareFereastra(panel);
			}
			else
				if(tipFiltru==2) //Notch
				{
					double Numerator[3], Denominator[3];
					Numerator[0]=0.958216646376661040918065737059805542231;
					Numerator[1]=-1.897014980249072779727725901466328650713;
					Numerator[2]= 0.958216646376661040918065737059805542231;
					
					Denominator[0]=1;
					Denominator[1]=-1.897014980249072779727725901466328650713;
					Denominator[2]= 0.916433292753322081836131474119611084461;
					
					
					semnalFiltrat[0]=waveData[0]; 
					semnalFiltrat[1]=waveData[1]; 
					for(int i=2;i<npoints/6;i++) 
					{
						semnalFiltrat[i]=Numerator[0]*waveData[i+secunda*npoints/6]+Numerator[1]*waveData[(i-1)+secunda*npoints/6]+Numerator[2]*waveData[(i-2)+secunda*npoints/6] -Denominator[1]*semnalFiltrat[i-1]-Denominator[2]*semnalFiltrat[i-2];
					}
					DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_SF, -1, VAL_IMMEDIATE_DRAW);
					PlotY(panel, FREQ_PANEL_GRAPH_SF,semnalFiltrat,npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);		
					aplicareFereastra(panel);
				}		
			}
			
}

void aplicareFereastra(int panel)
{
	GetCtrlAttribute(panel,FREQ_PANEL_RING_WINDOW_TYPE,ATTR_CTRL_INDEX,&tipFereastra);
	double *fereastra;
	if(tipAfisare==1)//pe numarul de puncte
	{
		semnalFF=(double *) malloc(numarpuncte*sizeof(double));
		fereastra=(double *) malloc(numarpuncte*sizeof(double));
			for (int i = 0; i < numarpuncte; i++)
			{
				fereastra[i] = 1.0;
			}
		
			if(tipFereastra==1)//blackman
			{
				BkmanWin(fereastra,numarpuncte);
				DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_FEREASTRA, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, FREQ_PANEL_GRAPH_FEREASTRA, fereastra, numarpuncte, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			}
			else
				if(tipFereastra==2) //dreptunghiular
				{
					DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_FEREASTRA, -1, VAL_IMMEDIATE_DRAW);
					PlotY(panel, FREQ_PANEL_GRAPH_FEREASTRA, fereastra, numarpuncte, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				
				}
		
		if(tipFiltru!=0)
		{
			Mul1D(semnalFiltrat,fereastra,numarpuncte,semnalFF);
			DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_WINDOWING, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, FREQ_PANEL_GRAPH_WINDOWING, semnalFF, numarpuncte, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			afisareSpectru(panel);
		}		
	}
	else
		if(tipAfisare==2)//tot semnalul
		{
			semnalFF=(double *) malloc(npoints*sizeof(double));
			fereastra=(double *) malloc(npoints*sizeof(double));
			for (int i = 0; i < npoints; i++)
			{
				fereastra[i] = 1.0;
			}
		
			if(tipFereastra==1)//blackman
			{
				BkmanWin(fereastra,npoints);
				DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_FEREASTRA, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, FREQ_PANEL_GRAPH_FEREASTRA, fereastra, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			}
			else
				if(tipFereastra==2) //dreptunghiular
				{
					DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_FEREASTRA, -1, VAL_IMMEDIATE_DRAW);
					PlotY(panel, FREQ_PANEL_GRAPH_FEREASTRA, fereastra, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				}
			if(tipFiltru!=0)
			{
			Mul1D(semnalFiltrat,fereastra,npoints,semnalFF);
			
			DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_WINDOWING, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, FREQ_PANEL_GRAPH_WINDOWING, semnalFF, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			}
		}
	else
		if(tipAfisare==3)//pe secunde
		{
			semnalFF=(double *) malloc(npoints/6*sizeof(double));
			fereastra=(double *) malloc(npoints/6*sizeof(double));
			for (int i = 0; i < npoints/6; i++)
			{
				fereastra[i] = 1.0;
			}
		
			if(tipFereastra==1)//blackman
			{
				BkmanWin(fereastra,npoints/6);
				DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_FEREASTRA, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, FREQ_PANEL_GRAPH_FEREASTRA, fereastra, npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			}
			else
				if(tipFereastra==2) //dreptunghiular
				{
					DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_FEREASTRA, -1, VAL_IMMEDIATE_DRAW);
					PlotY(panel, FREQ_PANEL_GRAPH_FEREASTRA, fereastra, npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				}
			if(tipFiltru!=0)
			{
				Mul1D(semnalFiltrat,fereastra,npoints/6,semnalFF);
		
				DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_WINDOWING, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, FREQ_PANEL_GRAPH_WINDOWING, semnalFF, npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			}
		}
	
}
void afisareSpectru(int panel)
{
	if(tipAfisare==1)//pe numarul de puncte
	{
			//spectru semnal filtrat si ferestruit
			double *convertedSpectrumsff=(double*)calloc(numarpuncte/2,sizeof(double));
			double *autoSpectrumsff=(double*)calloc(numarpuncte/2,sizeof(double));
			double dff=0.0;
			double powerPeak1=0.0;
			double freqPeak1=0.0;
			char unit[32]="V";
			WindowConst winConst;
			
			ScaledWindowEx(semnalFF,numarpuncte,RECTANGLE_,0,&winConst);
			AutoPowerSpectrum(semnalFF,numarpuncte,1.0/sampleRate,autoSpectrumsff,&dff);
			//calculeaza puterea si frecventa corespunzatoare varfului din spectrul semnalului 
			PowerFrequencyEstimate(autoSpectrumsff,numarpuncte/2,-1.0,winConst,dff,7,&freqPeak1,&powerPeak1);
			SetCtrlVal( panel, FREQ_PANEL_NUMERIC_FREQ2, freqPeak1);
			SetCtrlVal( panel, FREQ_PANEL_NUMERIC_POWER2, powerPeak1);
	    	//Functia converteste spectrul de intrare (care este puterea, amplitudinea sau amplificarea) 
			//în formate alternative (linear, logarithmic, dB) ce permit o reprezentare grafica mai convenabila.
		 	SpectrumUnitConversion(autoSpectrumsff, numarpuncte/2,0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, dff, winConst, convertedSpectrumsff, unit);
			DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_SPECTRU_SF, -1, VAL_IMMEDIATE_DRAW);
			PlotWaveform(panel, FREQ_PANEL_GRAPH_SPECTRU_SF, convertedSpectrumsff, numarpuncte/4 , VAL_DOUBLE, 1.0, 0.0, 0.0, dff, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
		
	}
	else
		if(tipAfisare==2)//tot semnalul
		{
			//spectru semnal filtrat si ferestruit
			double *convertedSpectrumsff=(double*)calloc(npoints/2,sizeof(double));
			double *autoSpectrumsff=(double*)calloc(npoints/2,sizeof(double));
			double dff=0.0;
			double powerPeak1=0.0;
			double freqPeak1=0.0;
			char unit[32]="V";
			WindowConst winConst;
			
			ScaledWindowEx(semnalFF,npoints,RECTANGLE_,0,&winConst);
			AutoPowerSpectrum(semnalFF,npoints,1.0/sampleRate,autoSpectrumsff,&dff);
			//calculeaza puterea si frecventa corespunzatoare varfului din spectrul semnalului 
			PowerFrequencyEstimate(autoSpectrumsff,npoints/2,-1.0,winConst,dff,7,&freqPeak1,&powerPeak1);
			SetCtrlVal( panel, FREQ_PANEL_NUMERIC_FREQ2, freqPeak1);
			SetCtrlVal( panel, FREQ_PANEL_NUMERIC_POWER2, powerPeak1);
			//Functia converteste spectrul de intrare (care este puterea, amplitudinea sau amplificarea) 
			//în formate alternative (linear, logarithmic, dB) ce permit o reprezentare grafica mai convenabila.
			SpectrumUnitConversion(autoSpectrumsff, npoints/2,0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, dff, winConst, convertedSpectrumsff, unit);
			DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_SPECTRU_SF, -1, VAL_IMMEDIATE_DRAW);
			PlotWaveform(panel, FREQ_PANEL_GRAPH_SPECTRU_SF, convertedSpectrumsff, npoints/4 , VAL_DOUBLE, 1.0, 0.0, 0.0, dff, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
	
		}
	else
		if(tipAfisare==3)//pe secunde
		{
			//spectru semnal filtrat si ferestruit
			double *convertedSpectrumsff=(double*)calloc(npoints/12,sizeof(double));
			double *autoSpectrumsff=(double*)calloc(npoints/12,sizeof(double));
			double dff=0.0;
			double powerPeak1=0.0;
			double freqPeak1=0.0;
			char unit[32]="V";
			WindowConst winConst;
			
			ScaledWindowEx(semnalFF,npoints/6,RECTANGLE_,0,&winConst);
			AutoPowerSpectrum(semnalFF,npoints/6,1.0/sampleRate,autoSpectrumsff,&dff);
			//calculeaza puterea si frecventa corespunzatoare varfului din spectrul semnalului 
			PowerFrequencyEstimate(autoSpectrumsff,npoints/12,-1.0,winConst,dff,7,&freqPeak1,&powerPeak1);
			SetCtrlVal( panel, FREQ_PANEL_NUMERIC_FREQ2, freqPeak1);
			SetCtrlVal( panel, FREQ_PANEL_NUMERIC_POWER2, powerPeak1);
			//Functia converteste spectrul de intrare (care este puterea, amplitudinea sau amplificarea) 
			//în formate alternative (linear, logarithmic, dB) ce permit o reprezentare grafica mai convenabila.
			SpectrumUnitConversion(autoSpectrumsff, npoints/12,0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, dff, winConst, convertedSpectrumsff, unit);
			DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_SPECTRU_SF, -1, VAL_IMMEDIATE_DRAW);
			PlotWaveform(panel, FREQ_PANEL_GRAPH_SPECTRU_SF, convertedSpectrumsff, npoints/24 , VAL_DOUBLE, 1.0, 0.0, 0.0, dff, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
	
		}
	
}
void afispesecunde(int panel)
{
	double *convertedSpectrumsi= (double*)malloc((int)(npoints/12) * sizeof(double));
	double* autoSpectrumsi = (double*)malloc((int)(npoints/12) * sizeof(double));
	double powerPeak=0.0;
	double freqPeak=0.0;
	double df=0.0;
	char unit[32]="V";
	WindowConst winConst;
	GetCtrlAttribute(panel,FREQ_PANEL_RING_SECUNDE,ATTR_CTRL_INDEX,&secunda);
	wavepersec=(double*)malloc((int)(npoints/6) * sizeof(double));
	for(int i=0;i<npoints/6;i++)
	{
		wavepersec[i]=waveData[npoints/6*secunda+i];
	}
	DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_INITIAL, -1, VAL_IMMEDIATE_DRAW);
	PlotY(panel, FREQ_PANEL_GRAPH_INITIAL, wavepersec, npoints/6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
	ScaledWindowEx(wavepersec,npoints/6,RECTANGLE_,0,&winConst);
	AutoPowerSpectrum(wavepersec,npoints/6,1.0/sampleRate,autoSpectrumsi,&df);
	//calculeaza puterea si frecventa corespunzatoare varfului din spectrul semnalului 
	PowerFrequencyEstimate(autoSpectrumsi,npoints/12,-1.0,winConst,df,7,&freqPeak,&powerPeak);
	SetCtrlVal( panel, FREQ_PANEL_NUMERIC_FREQ1, freqPeak);
	SetCtrlVal( panel, FREQ_PANEL_NUMERIC_POWER1, powerPeak);
	//Functia converteste spectrul de intrare (care este puterea, amplitudinea sau amplificarea) 
	//în formate alternative (linear, logarithmic, dB) ce permit o reprezentare grafica mai convenabila.
	SpectrumUnitConversion(autoSpectrumsi, npoints/12,0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df, winConst, convertedSpectrumsi, unit);
	DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_SPECTRU_SI, -1, VAL_IMMEDIATE_DRAW);
	PlotWaveform(panel, FREQ_PANEL_GRAPH_SPECTRU_SI, convertedSpectrumsi, npoints/24 , VAL_DOUBLE, 1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
	aplicareFiltru(panel);
	aplicareFereastra(panel);
	afisareSpectru(panel);
			
}
void afisSemnalIntreg(int panel)
{
	double *convertedSpectrumsi= (double*)malloc((int)(npoints/2) * sizeof(double));
	double* autoSpectrumsi = (double*)malloc((int)(npoints/2) * sizeof(double));
	double powerPeak=0.0;
	double freqPeak=0.0;
	double df=0.0;
	char unit[32]="V";
	WindowConst winConst;
	
		
	DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_INITIAL, -1, VAL_IMMEDIATE_DRAW);
	PlotY(panel, FREQ_PANEL_GRAPH_INITIAL, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
	ScaledWindowEx(waveData,npoints,RECTANGLE_,0,&winConst);
	AutoPowerSpectrum(waveData,npoints,1.0/sampleRate,autoSpectrumsi,&df);
	//calculeaza puterea si frecventa corespunzatoare varfului din spectrul semnalului 
	PowerFrequencyEstimate(autoSpectrumsi,npoints/2,-1.0,winConst,df,7,&freqPeak,&powerPeak);
	SetCtrlVal( panel, FREQ_PANEL_NUMERIC_FREQ1, freqPeak);
	SetCtrlVal( panel, FREQ_PANEL_NUMERIC_POWER1, powerPeak);
	//Functia converteste spectrul de intrare (care este puterea, amplitudinea sau amplificarea) 
	//în formate alternative (linear, logarithmic, dB) ce permit o reprezentare grafica mai convenabila.
	SpectrumUnitConversion(autoSpectrumsi, npoints/2,0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df, winConst, convertedSpectrumsi, unit);
	DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_SPECTRU_SI, -1, VAL_IMMEDIATE_DRAW);
	PlotWaveform(panel, FREQ_PANEL_GRAPH_SPECTRU_SI, convertedSpectrumsi, npoints/4 , VAL_DOUBLE, 1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
	aplicareFiltru(panel);
	aplicareFereastra(panel);
	afisareSpectru(panel);
			
			
		
}

int CVICALLBACK OnTimer (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	
	double* convertedSpectrumsi = (double*)malloc((int)(npoints) * sizeof(double));
	double* autoSpectrumsi = (double*)malloc((int)(npoints) * sizeof(double));
	double powerPeak=0.0;
	double freqPeak=0.0;
	double df=0.0;
	char unit[32]="V";
	
	WindowConst winConst;
	switch (event)
	{
		case EVENT_TIMER_TICK:
		
			GetCtrlVal(panel, FREQ_PANEL_NR_PUNCTE, &numarpuncte);
			double *wave=(double*)calloc(numarpuncte,sizeof(double));
			for(int i=0;i<numarpuncte;i++)
			{
				wave[i]=waveData[i+numarpuncte*nrFereastra];
			}
			DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_INITIAL, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, FREQ_PANEL_GRAPH_INITIAL, wave, numarpuncte, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			//spectru semnal initial
			
			numarFerestre = (int)(((double)npoints) / numarpuncte);
			if (((double)npoints) / numarpuncte - numarFerestre > 0.0)
			{
				numarFerestre++;
			}
			waveDataPerNrPuncte=(double*)calloc(numarpuncte*numarFerestre,sizeof(double));
			
			for(int i=0;i<numarpuncte*numarFerestre;i++)//formam wave data astfel incat sa avem un nr complet de puncte si pentru ultima fereastra(completam ultima fereastra cu 0)
			{
				if(i<npoints)
				{
					waveDataPerNrPuncte[i]=waveData[i];
				}
				else
				{
					waveDataPerNrPuncte[i]=0;
				}
			}
			
			
			convertedSpectrumsi=(double*)calloc(numarpuncte/2,sizeof(double));
			autoSpectrumsi=(double*)calloc(numarpuncte/2,sizeof(double));
			
			ScaledWindowEx(waveDataPerNrPuncte+nrFereastra*numarpuncte,numarpuncte,RECTANGLE_,0,&winConst);
			AutoPowerSpectrum(waveDataPerNrPuncte+nrFereastra*numarpuncte,numarpuncte,1.0/sampleRate,autoSpectrumsi,&df);
			//calculeaza puterea si frecventa corespunzatoare varfului din spectrul semnalului 
			PowerFrequencyEstimate(autoSpectrumsi,numarpuncte/2,-1.0,winConst,df,7,&freqPeak,&powerPeak);
			SetCtrlVal( panel, FREQ_PANEL_NUMERIC_FREQ1, freqPeak);
			SetCtrlVal( panel, FREQ_PANEL_NUMERIC_POWER1, powerPeak);
	    	//Functia converteste spectrul de intrare (care este puterea, amplitudinea sau amplificarea) 
			//în formate alternative (linear, logarithmic, dB) ce permit o reprezentare grafica mai convenabila.
		 	SpectrumUnitConversion(autoSpectrumsi, numarpuncte/2,0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df, winConst, convertedSpectrumsi, unit);
			DeleteGraphPlot (panel,FREQ_PANEL_GRAPH_SPECTRU_SI, -1, VAL_IMMEDIATE_DRAW);
			PlotWaveform(panel, FREQ_PANEL_GRAPH_SPECTRU_SI, convertedSpectrumsi, numarpuncte/4 , VAL_DOUBLE, 1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			aplicareFiltru(panel);
			aplicareFereastra(panel);
			
			
			
			
			if(nrFereastra==numarFerestre)
			{
				nrFereastra=0;
			}
			else
				nrFereastra++;
			break;
	}
	return 0;
}

int CVICALLBACK OnFilterTypeFreq (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			aplicareFiltru(panel);
			break;
	}
	return 0;
}

int CVICALLBACK OnShow (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
	
			GetCtrlAttribute(panel,FREQ_PANEL_RING_TIP_AFISARE,ATTR_CTRL_INDEX,&tipAfisare);
			if(tipAfisare==1)
			{
				nrFereastra=0;
				SetCtrlAttribute(panel,FREQ_PANEL_TIMER, ATTR_ENABLED,1);
				SetCtrlAttribute(panel,FREQ_PANEL_NR_PUNCTE,ATTR_DIMMED,0);
				SetCtrlAttribute(panel,FREQ_PANEL_RING_SECUNDE,ATTR_DIMMED,1);
			}
			else
				if(tipAfisare==2)
				{
					SetCtrlAttribute(panel,FREQ_PANEL_TIMER, ATTR_ENABLED,0);
					SetCtrlAttribute(panel,FREQ_PANEL_NR_PUNCTE,ATTR_DIMMED,1);
					SetCtrlAttribute(panel,FREQ_PANEL_RING_SECUNDE,ATTR_DIMMED,1);
					afisSemnalIntreg(panel);
				}
			else
				if(tipAfisare==3)
				{
					SetCtrlAttribute(panel,FREQ_PANEL_TIMER, ATTR_ENABLED,0);
					SetCtrlAttribute(panel,FREQ_PANEL_NR_PUNCTE,ATTR_DIMMED,1);
					SetCtrlAttribute(panel,FREQ_PANEL_RING_SECUNDE,ATTR_DIMMED,0);
					afispesecunde(panel);
				}
			break;
	}
	return 0;
}

int CVICALLBACK OnSave2 (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			int bitmapID=0;
			char*filename=0;
			filename=(char*)calloc(100,sizeof(char));
			//spectru semnal initial
			GetCtrlDisplayBitmap(panel,FREQ_PANEL_GRAPH_SPECTRU_SI,1,&bitmapID);
			if(tipAfisare==1)
			{
				sprintf(filename,"Spectru_initial_%d_%d.png",nrFereastra*numarpuncte,(nrFereastra+1)*numarpuncte-1);
			}
			else
				if(tipAfisare==2)
				{
					sprintf(filename,"Spectru_initial_intreg.png");
				}
			else
				if(tipAfisare==3)
				{
					sprintf(filename,"Spectru_initial_secunda_%d_%d.png",secunda,secunda+1);
				}
			SaveBitmapToJPEGFile(bitmapID,filename,JPEG_DCTFLOAT,50);
			DiscardBitmap(bitmapID);
			
			//fereastra simpla
			GetCtrlDisplayBitmap(panel,FREQ_PANEL_GRAPH_FEREASTRA,1,&bitmapID);
			if(tipAfisare==1)
			{
				if(tipFereastra==1)
				{
				sprintf(filename,"Fereastra_blackman_numarpuncte_%d_%d.png",nrFereastra*numarpuncte,(nrFereastra+1)*numarpuncte-1);
				}
				else
					if(tipFereastra==2)
					{
						sprintf(filename,"Fereastra_dreptunghiulara_%d_%d.png",nrFereastra*numarpuncte,(nrFereastra+1)*numarpuncte-1);
					}
			}
			else
				if(tipAfisare==2)
				{
					if(tipFereastra==1)
					{
						sprintf(filename,"Fereastra_blackman_intreg.png");
					}
					else
						if(tipFereastra==2)
						{
							sprintf(filename,"Fereastra_dreptunghiulara_intreg.png");
						}
				}
			else
				if(tipAfisare==3)
				{
					if(tipFereastra==1)
					{
						sprintf(filename,"Fereastra_blackman_secunda_%d_%d.png",secunda,secunda+1);
					}
					else
						if(tipFereastra==2)
						{
							sprintf(filename,"Fereastra_dreptunghiulara_secunda_%d_%d.png",secunda,secunda+1);
						}
					}
					
			SaveBitmapToJPEGFile(bitmapID,filename,JPEG_DCTFLOAT,50);
			DiscardBitmap(bitmapID);
			GetCtrlDisplayBitmap(panel,FREQ_PANEL_GRAPH_SF,1,&bitmapID);
			//semnal filtrat
			if(tipAfisare==1)
			{
				if(tipFiltru==1)
				{
					sprintf(filename,"Filtru_ChebyshevII_trece_jos_numarpuncte_%d_%d.png",nrFereastra*numarpuncte,(nrFereastra+1)*numarpuncte-1);
				}
				else
					if(tipFiltru==2)
					{
						sprintf(filename,"Filtru_Notch_%d_%d.png",nrFereastra*numarpuncte,(nrFereastra+1)*numarpuncte-1);
				}
			}
			
			else
				if(tipAfisare==2)
				{
					if(tipFiltru==1)
					{
						sprintf(filename,"Filtru_ChebyshevII_trece_jos_intreg.png");
					}
					else
						if(tipFiltru==2)
						{
							sprintf(filename,"Filtru_Notch_intreg.png");
						}
				}
			else 
				if(tipAfisare==3)
				{
					if(tipFiltru==1)
				{
					sprintf(filename,"Filtru_ChebyshevII_trece_jos_secunde_%d_%d.png",secunda,secunda+1);
				}
				else
					if(tipFiltru==2)
					{
						sprintf(filename,"Filtru_Notch_%d_%d.png",secunda,secunda+1);
				}
				}
			
			SaveBitmapToJPEGFile(bitmapID,filename,JPEG_DCTFLOAT,50);
			DiscardBitmap(bitmapID);
			
			GetCtrlDisplayBitmap(panel,FREQ_PANEL_GRAPH_WINDOWING,1,&bitmapID);
			//semnal filtrat si ferestruit
			
			if(tipAfisare==1)
			{
				if(tipFereastra==1)
				{
					if(tipFiltru==1)
					{
						sprintf(filename,"Fereastra_Blackman_filtru_chebyshevII_%d_%d.png",nrFereastra*numarpuncte,(nrFereastra+1)*numarpuncte-1);
					}
					else 
						if(tipFiltru==2)
						{
							sprintf(filename,"Fereastra_Blackman_filtru_Notch_%d_%d.png",nrFereastra*numarpuncte,(nrFereastra+1)*numarpuncte-1);
						}
				}
				else
					if(tipFereastra==2)
					{
						if(tipFiltru==1)
						{
							sprintf(filename,"Fereastra_Dreptunghiulara_filtru_chebyshevII_%d_%d.png",nrFereastra*numarpuncte,(nrFereastra+1)*numarpuncte-1);
						}
						else
							if(tipFiltru==2)
							{
								sprintf(filename,"Fereastra_Dreptunghiulara_filtru_Notch_%d_%d.png",nrFereastra*numarpuncte,(nrFereastra+1)*numarpuncte-1);
							}
					}
			}
			else
				if(tipAfisare==2)
				{
					if(tipFereastra==1)
					{
						if(tipFiltru==1)
						{
							sprintf(filename,"Fereastra_Blackman_Filtru_ChebyshevII_trece_jos_intreg.png");
						}
						else
							if(tipFiltru==2)
							{
								sprintf(filename,"Fereastra_Blackman_Filtru_Notch_intreg.png");
							}
					}
					else
						if(tipFereastra==2)
						{
							if(tipFiltru==1)
						{
							sprintf(filename,"Fereastra_Dreptunghiulara_Filtru_ChebyshevII_trece_jos_intreg.png");
						}
						else
							if(tipFiltru==2)
							{
								sprintf(filename,"Fereastra_Dreptunghiulara_Filtru_Notch_intreg.png");
							}
					}
				}
			else
				if(tipAfisare==3)
				{
					if(tipFereastra==1)
					{
						if(tipFiltru==1)
						{
							sprintf(filename,"Fereastra_Blackman_Filtru_ChebyshevII_%d_%d.png",secunda,secunda+1);
						}
						else
							if(tipFiltru==2)
							{
								sprintf(filename,"Fereastra_Blackman_Filtru_Notch_%d_%d.png",secunda,secunda+1);
							}
					}
					else
						if(tipFereastra==2)
						{
							if(tipFiltru==1)
						{
							sprintf(filename,"Fereastra_Dreptunghiulara_Filtru_ChebyshevII_%d_%d.png",secunda,secunda+1);
						}
						else
							if(tipFiltru==2)
							{
								sprintf(filename,"Fereastra_Dreptunghiulara_Filtru_Notch_%d_%dintreg.png",secunda,secunda+1);
							}
					}
				}
			SaveBitmapToJPEGFile(bitmapID,filename,JPEG_DCTFLOAT,50);
			DiscardBitmap(bitmapID);
			
			GetCtrlDisplayBitmap(panel,FREQ_PANEL_GRAPH_SPECTRU_SF,1,&bitmapID);
			//spectrul noului semnal
			if(tipAfisare==1)
			{
				if(tipFereastra==1)
				{
					if(tipFiltru==1)
					{
						sprintf(filename,"Spectru_Fereastra_Blackman_filtru_chebyshevII_%d_%d.png",nrFereastra*numarpuncte,(nrFereastra+1)*numarpuncte-1);
					}
					else 
						if(tipFiltru==2)
						{
							sprintf(filename,"Spectru_Fereastra_Blackman_filtru_Notch_%d_%d.png",nrFereastra*numarpuncte,(nrFereastra+1)*numarpuncte-1);
						}
				}
				else
					if(tipFereastra==2)
					{
						if(tipFiltru==1)
						{
							sprintf(filename,"Spectru_Fereastra_Dreptunghiulara_filtru_chebyshevII_%d_%d.png",nrFereastra*numarpuncte,(nrFereastra+1)*numarpuncte-1);
						}
						else
							if(tipFiltru==2)
							{
								sprintf(filename,"Spectru_Fereastra_Dreptunghiulara_filtru_Notch_%d_%d.png",nrFereastra*numarpuncte,(nrFereastra+1)*numarpuncte-1);
							}
					}
			}
			else
				if(tipAfisare==2)
				{
					if(tipFereastra==1)
					{
						if(tipFiltru==1)
						{
							sprintf(filename,"Spectru_Fereastra_Blackman_Filtru_ChebyshevII_trece_jos_intreg.png");
						}
						else
							if(tipFiltru==2)
							{
								sprintf(filename,"Spectru_Fereastra_Blackman_Filtru_Notch_intreg.png");
							}
					}
					else
						if(tipFereastra==2)
						{
							if(tipFiltru==1)
						{
							sprintf(filename,"Spectru_Fereastra_Dreptunghiulara_Filtru_ChebyshevII_trece_jos_intreg.png");
						}
						else
							if(tipFiltru==2)
							{
								sprintf(filename,"Spectru_Fereastra_Dreptunghiulara_Filtru_Notch_intreg.png");
							}
					}
				}
			else
				if(tipAfisare==3)
				{
					if(tipFereastra==1)
					{
						if(tipFiltru==1)
						{
							sprintf(filename,"Spectru_Fereastra_Blackman_Filtru_ChebyshevII_%d_%d.png",secunda,secunda+1);
						}
						else
							if(tipFiltru==2)
							{
								sprintf(filename,"Spectru_Fereastra_Blackman_Filtru_Notch_%d_%d.png",secunda,secunda+1);
							}
					}
					else
						if(tipFereastra==2)
						{
							if(tipFiltru==1)
						{
							sprintf(filename,"Spectru_Fereastra_Dreptunghiulara_Filtru_ChebyshevII_%d_%d.png",secunda,secunda+1);
						}
						else
							if(tipFiltru==2)
							{
								sprintf(filename,"Spectru_Fereastra_Dreptunghiulara_Filtru_Notch_%d_%dintreg.png",secunda,secunda+1);
							}
					}
				}
			SaveBitmapToJPEGFile(bitmapID,filename,JPEG_DCTFLOAT,50);
			DiscardBitmap(bitmapID);
			free(filename);
			break;
	}
	return 0;
}
