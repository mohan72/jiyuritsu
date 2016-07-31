//jutils.h
// Copyright 2015, Mohanraj Dhanagopal
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "jutils.h"

void delchars(char* delLine, int startPos, int delLen) {
	if (delLine == NULL || delLen == 0 || startPos > strlen(delLine)-1) { return; }
	if ((startPos+delLen) > strlen(delLine)) { return; }	//Not enough characters to delete
	
	int destPos = startPos;
	int srcPos = startPos + delLen;
	int copyLen = strlen(delLine) - srcPos;
	char* temp = new char[copyLen+1];
	strncpy(temp, delLine+srcPos, copyLen);
	temp[copyLen] = 0;
	strncpy(delLine+destPos, temp, copyLen);
	delLine[destPos+copyLen] = 0;
}

bool endsWithLF(char* txt) {
	if (txt == NULL) { return false; }
	
	bool ret = false;
	if (txt[strlen(txt)-1] == '\n')
		ret = true;
		
	return ret;
}

int lastCursorStop(char* txt, int maxwidth) {	//stop = character location 0=first
	int ret = 0;
	int len = strlen(txt);
	if (endsWithLF(txt)) {	//Line ends with LF, can't go past
		ret = len-1;
	} else {	//Line ends with space or char, can go past if screenwidth available
		if (len <= (maxwidth-1))
			ret = len;
		else
			ret = len-1;
	}
	
	return ret;
}

int lastDisplayChar(char* txt, int maxwidth) {
	for (int i = (maxwidth-1); i >= 0; i--) {
		if (txt[i] == '\n')
			return i;
		if (txt[i] == ' ') {
			return i;
		}
	}
	return maxwidth-1;
}

bool fileExists(char* fName) {
	if( access(fName, F_OK ) != -1 ) {
		// file exists
		return true;
	} else {
		// file doesn't exist
		return false;
	}
	return false;
}

bool createFile(char* fName) {
	FILE* fp = fopen(fName,"wt");
	if (fp!=NULL) {
		fputs ("\n", fp);
		fclose(fp);
		return true;
	} else {
		return false;
	}
	return false;
}

//end of jutils.h
