#include<stdio.h>
#include "pbPlots.h"
#include "supportLib.h"
//gcc plot.c pbPlots.c supportLib.c -lm
void StartPlot(double *x,double *RandomFIFOPF, double *RandomFIFOIR, double *RandomFIFODW,wchar_t *t,char *fileName)
{
	ScatterPlotSeries *series = GetDefaultScatterPlotSeriesSettings();
	series->xs = x;
	series->xsLength = 10;
	series->ys = RandomFIFOPF;
	series->ysLength = 10;
	series->linearInterpolation = true;
	series->lineType = L"solid";
	series->lineTypeLength = wcslen(series->lineType);
	series->lineThickness = 2;
	series->color = CreateRGBColor(1, 0, 0);

	ScatterPlotSeries *series2 = GetDefaultScatterPlotSeriesSettings();
	series2->xs = x;
	series2->xsLength = 10;
	series2->ys = RandomFIFOIR;
	series2->ysLength = 10;
	series2->linearInterpolation = true;
	series2->lineType = L"solid";
	series2->lineTypeLength = wcslen(series2->lineType);
	series2->lineThickness = 2;
	series2->color = CreateRGBColor(0, 1, 0);
	
	ScatterPlotSeries *series3 = GetDefaultScatterPlotSeriesSettings();
	series3->xs = x;
	series3->xsLength = 10;
	series3->ys = RandomFIFODW;
	series3->ysLength = 10;
	series3->linearInterpolation = true;
	series3->lineType = L"solid";
	series3->lineTypeLength = wcslen(series3->lineType);
	series3->lineThickness = 2;
	series3->color = CreateRGBColor(0, 0, 1);

	ScatterPlotSettings *settings = GetDefaultScatterPlotSettings();
	settings->width = 1000;
	settings->height = 800;
	settings->autoBoundaries = true;
	settings->autoPadding = true;
	settings->title = t;
	settings->titleLength = wcslen(settings->title);
	settings->xLabel = L"Frame number(Red:Page fault/Green:Interrupt/Blue:Disk write)";
	settings->xLabelLength = wcslen(settings->xLabel);
	settings->yLabel = L"Page fault/Interrupt/Disk write";
	settings->yLabelLength = wcslen(settings->yLabel);
	ScatterPlotSeries *s [] = {series,series2, series3};
	settings->scatterPlotSeries = s;
	settings->scatterPlotSeriesLength = 3;
	
	_Bool success;
	StringReference *errorMessage;
	RGBABitmapImageReference *imageReference = CreateRGBABitmapImageReference();
	
	errorMessage = (StringReference *)malloc(sizeof(StringReference));
	success = DrawScatterPlotFromSettings(imageReference, settings, errorMessage);

	if(success){
		size_t length;
		double *pngdata = ConvertToPNG(&length, imageReference->image);
		WriteToFile(pngdata, length, fileName);
		DeleteImage(imageReference->image);
	}else{
		fprintf(stderr, "Error: ");
		for(int i = 0; i < errorMessage->stringLength; i++){
			fprintf(stderr, "%c", errorMessage->string[i]);
		}
		fprintf(stderr, "\n");
	}

	FreeAllocations();
}
int main(int argc,char* argv[])
{
	double RandomFIFOPF[10];uint8_t RandomFIFOPFCount = 0;
	double RandomFIFOIR[10];uint8_t RandomFIFOIRCount = 0;
	double RandomFIFODW[10];uint8_t RandomFIFODWCount = 0;
	double RandomOPTPF[10];uint8_t RandomOPTPFCount = 0;
	double RandomOPTIR[10];uint8_t RandomOPTIRCount = 0;
	double RandomOPTDW[10];uint8_t RandomOPTDWCount = 0;
	double RandomESCPF[10];uint8_t RandomESCPFCount = 0;
	double RandomESCIR[10];uint8_t RandomESCIRCount = 0;
	double RandomESCDW[10];uint8_t RandomESCDWCount = 0;
	double RandomMyPF[10];uint8_t RandomMyPFCount = 0;
	double RandomMyIR[10];uint8_t RandomMyIRCount = 0;
	double RandomMyDW[10];uint8_t RandomMyDWCount = 0;
	
	double LocalFIFOPF[10];uint8_t LocalFIFOPFCount = 0;
	double LocalFIFOIR[10];uint8_t LocalFIFOIRCount = 0;
	double LocalFIFODW[10];uint8_t LocalFIFODWCount = 0;
	double LocalOPTPF[10];uint8_t LocalOPTPFCount = 0;
	double LocalOPTIR[10];uint8_t LocalOPTIRCount = 0;
	double LocalOPTDW[10];uint8_t LocalOPTDWCount = 0;
	double LocalESCPF[10];uint8_t LocalESCPFCount = 0;
	double LocalESCIR[10];uint8_t LocalESCIRCount = 0;
	double LocalESCDW[10];uint8_t LocalESCDWCount = 0;
	double LocalMyPF[10];uint8_t LocalMyPFCount = 0;
	double LocalMyIR[10];uint8_t LocalMyIRCount = 0;
	double LocalMyDW[10];uint8_t LocalMyDWCount = 0;
	
	double MYREFFIFOPF[10];uint8_t MYREFFIFOPFCount = 0;
	double MYREFFIFOIR[10];uint8_t MYREFFIFOIRCount = 0;
	double MYREFFIFODW[10];uint8_t MYREFFIFODWCount = 0;
	double MYREFOPTPF[10];uint8_t MYREFOPTPFCount = 0;
	double MYREFOPTIR[10];uint8_t MYREFOPTIRCount = 0;
	double MYREFOPTDW[10];uint8_t MYREFOPTDWCount = 0;
	double MYREFESCPF[10];uint8_t MYREFESCPFCount = 0;
	double MYREFESCIR[10];uint8_t MYREFESCIRCount = 0;
	double MYREFESCDW[10];uint8_t MYREFESCDWCount = 0;
	double MYREFMyPF[10];uint8_t MYREFMyPFCount = 0;
	double MYREFMyIR[10];uint8_t MYREFMyIRCount = 0;
	double MYREFMyDW[10];uint8_t MYREFMyDWCount = 0;
	double xs[10] = {10,20,30,40,50,60,70,80,90,100};
	printf("Call the plot process.\n");
	uint16_t i;//Can't uint8_t, or it may cause infinite loop
	for(i=1;i<360+1;i++)
	{
		//printf("%s\n",argv[i]);
		int x = atoi(argv[i]);
		// printf("i=%d\n",i);
		if(i<31)
		{
			if(i%3==1)RandomFIFOPF[RandomFIFOPFCount++] = x;
			else if(i%3==2)RandomFIFOIR[RandomFIFOIRCount++] = x;
			else if(i%3==0)RandomFIFODW[RandomFIFODWCount++] = x;
		}
		else if(i>=31 && i<61)
		{
			if(i%3==1)RandomOPTPF[RandomOPTPFCount++] = x;
			else if(i%3==2)RandomOPTIR[RandomOPTIRCount++] = x;
			else if(i%3==0)RandomOPTDW[RandomOPTDWCount++] = x;
		}
		else if(i>=61 && i<91)
		{
			if(i%3==1)RandomESCPF[RandomESCPFCount++] = x;
			else if(i%3==2)RandomESCIR[RandomESCIRCount++] = x;
			else if(i%3==0)RandomESCDW[RandomESCDWCount++] = x;
		}
		else if(i>=91 && i<121)
		{
			if(i%3==1)RandomMyPF[RandomMyPFCount++] = x;
			else if(i%3==2)RandomMyIR[RandomMyIRCount++] = x;
			else if(i%3==0)RandomMyDW[RandomMyDWCount++] = x;
		}
		else if(i>=121 && i<151)
		{
			if(i%3==1)LocalFIFOPF[LocalFIFOPFCount++] = x;
			else if(i%3==2)LocalFIFOIR[LocalFIFOIRCount++] = x;
			else if(i%3==0)LocalFIFODW[LocalFIFODWCount++] = x;
		}
		else if(i>=151 && i<181)
		{
			if(i%3==1)LocalOPTPF[LocalOPTPFCount++] = x;
			else if(i%3==2)LocalOPTIR[LocalOPTIRCount++] = x;
			else if(i%3==0)LocalOPTDW[LocalOPTDWCount++] = x;
		}
		else if(i>=181 && i<211)
		{
			if(i%3==1)LocalESCPF[LocalESCPFCount++] = x;
			else if(i%3==2)LocalESCIR[LocalESCIRCount++] = x;
			else if(i%3==0)LocalESCDW[LocalESCDWCount++] = x;
		}
		else if(i>=211 && i<241)
		{
			if(i%3==1)LocalMyPF[LocalMyPFCount++] = x;
			else if(i%3==2)LocalMyIR[LocalMyIRCount++] = x;
			else if(i%3==0)LocalMyDW[LocalMyDWCount++] = x;
		}
		else if(i>=241 && i<271)
		{
			if(i%3==1)MYREFFIFOPF[MYREFFIFOPFCount++] = x;
			else if(i%3==2)MYREFFIFOIR[MYREFFIFOIRCount++] = x;
			else if(i%3==0)MYREFFIFODW[MYREFFIFODWCount++] = x;
		}
		else if(i>=271 && i<301)
		{
			if(i%3==1)MYREFOPTPF[MYREFOPTPFCount++] = x;
			else if(i%3==2)MYREFOPTIR[MYREFOPTIRCount++] = x;
			else if(i%3==0)MYREFOPTDW[MYREFOPTDWCount++] = x;
		}
		else if(i>=301 && i<331)
		{
			if(i%3==1)MYREFESCPF[MYREFESCPFCount++] = x;
			else if(i%3==2)MYREFESCIR[MYREFESCIRCount++] = x;
			else if(i%3==0)MYREFESCDW[MYREFESCDWCount++] = x;
		}
		else if(i>=331 && i<361)
		{
			if(i%3==1)MYREFMyPF[MYREFMyPFCount++] = x;
			else if(i%3==2)MYREFMyIR[MYREFMyIRCount++] = x;
			else if(i%3==0)MYREFMyDW[MYREFMyDWCount++] = x;
		}
	}
	wchar_t *title = L"FIFO Random";
	char *fileName = "FIFO Random.png";
	StartPlot(xs,RandomFIFOPF,RandomFIFOIR,RandomFIFODW,title,fileName);
	title = L"OPT Random";
	fileName = "OPT Random.png";
	StartPlot(xs,RandomOPTPF,RandomOPTIR,RandomOPTDW,title,fileName);
	
	title = L"ESC Random";
	fileName = "ESC Random.png";
	StartPlot(xs,RandomESCPF,RandomESCIR,RandomESCDW,title,fileName);
	
	title = L"My_Algo Random";
	fileName = "My_Algo Random.png";
	StartPlot(xs,RandomMyPF,RandomMyIR,RandomMyDW,title,fileName);
	
	title = L"FIFO Local";
	fileName = "FIFO Local.png";
	StartPlot(xs,LocalFIFOPF,LocalFIFOIR,LocalFIFODW,title,fileName);
	
	title = L"OPT Local";
	fileName = "OPT Local.png";
	StartPlot(xs,LocalOPTPF,LocalOPTIR,LocalOPTDW,title,fileName);
	
	title = L"ESC Local";
	fileName = "ESC Local.png";
	StartPlot(xs,LocalESCPF,LocalESCIR,LocalESCDW,title,fileName);
	
	title = L"My_Algo Local";
	fileName = "My_Algo Local.png";
	StartPlot(xs,LocalMyPF,LocalMyIR,LocalMyDW,title,fileName);
	
	title = L"FIFO My string";
	fileName = "FIFO My string.png";
	StartPlot(xs,MYREFFIFOPF,MYREFFIFOIR,MYREFFIFODW,title,fileName);
	
	title = L"OPT My string";
	fileName = "OPT My string.png";
	StartPlot(xs,MYREFOPTPF,MYREFOPTIR,MYREFOPTDW,title,fileName);
	
	title = L"ESC My string";
	fileName = "ESC My string.png";
	StartPlot(xs,MYREFESCPF,MYREFESCIR,MYREFESCDW,title,fileName);
	
	title = L"My_Algo My string";
	fileName = "My_Algo My string.png";
	StartPlot(xs,MYREFMyPF,MYREFMyIR,MYREFMyDW,title,fileName);
	// for(i=0;i<10;i++)printf("%lf ",RandomFIFOPF[i]);
	// printf("\n");

	
	// RGBABitmapImageReference *imageRef = CreateRGBABitmapImageReference();
	// StringReference *errorMessage;
	// errorMessage = CreateStringReference(strparam(L""));

	// DrawScatterPlot(imageRef,800,600,xs,10,RandomFIFOPF,10,errorMessage);
	// size_t length;
	// double *pngData = ConvertToPNG(&length, imageRef->image);
	// WriteToFile(pngData,length,"Random FIFO PageFault plot.png");
	
	// DrawScatterPlot(imageRef,800,600,xs,10,RandomFIFOIR,10,errorMessage);
	// double *pngData1 = ConvertToPNG(&length, imageRef->image);
	// WriteToFile(pngData1,length,"Random FIFO Interrupt plot.png");
	
	printf("Image saved.\n");
	return 0;
}