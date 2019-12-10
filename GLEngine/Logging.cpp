
/******************************************************************************
 ******* - DEBUG LOGGING - ****************************************************	
 *****************************************************************************/

#include <tchar.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "Logging.h"

#define Report(message) { MessageBox(NULL, message,"Message", MB_OK | MB_ICONEXCLAMATION); }

void _StartLogging()
{
	FILE* pFile = NULL;

	if(!fopen_s(&pFile, "..\\debuglog.htm", "w"))
	{
		//	Write Log header
		fprintf(pFile, "<html><head><title>Debug Log for GLEngine</title></head><body>\n");
		fprintf(pFile, "<font face=\"Arial\" size=\"3\" color=\"#000000\"><b>Debug Log for GLEngine</b><br></font><br>\n");

		//	Close the file
		fclose(pFile);
	} else
	{
		Report("Error opening log file");
	}
}

void _StopLogging()
{
	FILE* pFile = NULL;

	LogHeader("Log end");

	if(!fopen_s(&pFile, "..\\debuglog.htm", "a+"))
	{
		//	Write tail
		fprintf(pFile, "</body></html>");

		//	Close the file
		fclose(pFile);
	} else	{
		Report("Error appending to log file");
	}
}

void _LogHeader(const char *Message, ...)
{
	FILE* pFile = NULL;
	va_list ArgList;

	//	Init the arglist
	va_start(ArgList, Message);

	//	Open File
	if (!fopen_s(&pFile, "..\\debuglog.htm", "a+"))
	{
		//	Set font and black font color
		fprintf(pFile, "<font face=\"Arial\" size=\"2\" color=\"#000000\"><b><br>");

		//	Print the error
		vfprintf(pFile, Message, ArgList);

		//	Close tags
		fprintf(pFile, "<br><br></b></font>\n");

		//	Close the file
		fclose(pFile);

		//	Output to console
		printf("\n");
		vprintf(Message, ArgList);
		printf("\n");

	} else {
		Report("Error appending to log file");
	}

	//	End the arglist
	va_end(ArgList);
}

void _LogError(const char *Message, ...)
{
	FILE* pFile = NULL;
	va_list ArgList;

	//	Init the arglist
	va_start(ArgList, Message);

	//	Open File
	if(!fopen_s(&pFile, "..\\debuglog.htm", "a+"))
	{
		//	Set font and red font color
		fprintf(pFile, "<font face=\"Arial\" size=\"2\" color=\"#FF0000\">");

		//	Print the error
		vfprintf(pFile, Message, ArgList);

		//	Close tags
		fprintf(pFile, "</font><br>\n");

		//	Close the file
		fclose(pFile);

		vprintf(Message, ArgList);
		printf("\n");

	} else {
		Report("Error appending to log file");
	}

	//	End the arglist
	va_end(ArgList);
}

void _LogWarning(const char *Message, ...)
{
	FILE* pFile = NULL;
	va_list ArgList;

	//	Init the arglist
	va_start(ArgList, Message);

	//	Open File
	if (!fopen_s(&pFile, "..\\debuglog.htm", "a+"))
	{
		//	Set font and blue font color
		fprintf(pFile, "<font face=\"Arial\" size=\"1\" color=\"#0000FF\">");

		//	Print the error
		vfprintf(pFile, Message, ArgList);

		//	Close tags
		fprintf(pFile, "</font><br>\n");

		//	Close the file
		fclose(pFile);

		//	Output to console
		vprintf(Message, ArgList);
		printf("\n");

	} else {
		Report("Error appending to log file");
	}

	//	End the arglist
	va_end(ArgList);
}

void _LogMessage(const char *Message, ...)
{
	FILE* pFile = NULL;
	va_list ArgList;

	//	Init the arglist
	va_start(ArgList, Message);

	//	Open File
	if(!fopen_s(&pFile, "..\\debuglog.htm", "a+"))
	{
		//	Set font and black font color
		fprintf(pFile, "<font face=\"Arial\" size=\"2\" color=\"#000000\">");

		//	Print the error
		vfprintf(pFile, Message, ArgList);

		//	Close tags
		fprintf(pFile, "</font><br>\n");

		//	Close the file
		fclose(pFile);

		//	Output to console
		vprintf(Message, ArgList);
		printf("\n");

	} else {
		Report("Error appending to log file");
	}

	//	End the arglist
	va_end(ArgList);

}

void _LogStats(const char *Message, ...)
{
	FILE* pFile = NULL;
	va_list ArgList;

	//	Init the arglist
	va_start(ArgList, Message);

	//	Open File
	if(!fopen_s(&pFile, "..\\debuglog.htm", "a+"))
	{
		//	Set font and black font color
		fprintf(pFile, "<font face=\"Courier\" size=\"2\" color=\"#000000\">");

		//	Print the error
		vfprintf(pFile, Message, ArgList);

		//	Close tags
		fprintf(pFile, "</font><br>\n");

		//	Close the file
		fclose(pFile);

		//	Output to console
		vprintf(Message, ArgList);

	} else {
		Report("Error appending to log file");
	}

	//	End the arglist
	va_end(ArgList);
}

void _LogNothing()
{
	FILE* pFile = NULL;

	//	Open File
	if(!fopen_s(&pFile, "..\\debuglog.htm", "a+"))
	{
		fprintf(pFile, "<br>\n");

		//	Close the file
		fclose(pFile);
	} else {
		Report("Error appending to log file");
	}
}

void DumpMatrices(const float* pMatrices, int NrMatrices)
{
	float* pCurrMat = (float*)pMatrices;
	for(int i=0; i<NrMatrices; i++, pCurrMat+=16) {
		LogMessage("<b>Matrix %i</b>", i);
		LogMessage("--<b>x1:</b>%.5f <b>y2:</b>%.5f <b>z3:</b>%.5f <b>w4:</b>%.5f",
			pCurrMat[0], pCurrMat[1], pCurrMat[2], pCurrMat[3]);
		LogMessage("--<b>x2:</b>%.5f <b>y2:</b>%.5f <b>z2:</b>%.5f <b>w2:</b>%.5f",
			pCurrMat[4], pCurrMat[5], pCurrMat[6], pCurrMat[7]);
		LogMessage("--<b>x3:</b>%.5f <b>y3:</b>%.5f <b>z3:</b>%.5f <b>w3:</b>%.5f",
			pCurrMat[8], pCurrMat[9], pCurrMat[10], pCurrMat[11]);
		LogMessage("--<b>x4:</b>%.5f <b>y4:</b>%.5f <b>z4:</b>%.5f <b>w4:</b>%.5f",
			pCurrMat[12], pCurrMat[13], pCurrMat[14], pCurrMat[15]);
	}
}
