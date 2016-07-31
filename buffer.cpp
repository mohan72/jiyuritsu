// buffer.cpp - memory management for text editor
// copyright 2015, Mohanraj Dhanagopal

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "jutils.h"
#include "buffer.h"

//txt should be smaller than SCREENWIDTH
line::line(line* cLine, char* txt, int maxlen, bool addbefore) {
	text = new char[maxlen+2];
	memset(text, 0, maxlen+2);
	strcpy(text, txt);

	if (cLine) {
		if (addbefore) {
			next = cLine;
			prev = cLine->prev;
			cLine->prev = this;
			if (prev != NULL)
				prev->next = this;
		} else {
			prev = cLine;
			next = cLine->next;
			cLine->next = this;
			if (next != NULL)
				next->prev = this;
		}
	} else {
		prev = NULL;
		next = NULL;
	}
}

line::~line() {
	delete text;
	
	if (prev)
		prev->next = next;
	if (next)
		next->prev = prev;
}

void line::setText(char* txt, int maxlen) {
	delete text;
	text = new char[maxlen+2];
	memset(text, 0, maxlen+2);
	strcpy(text, txt);
}

linebuffer::linebuffer(char* fname, int lwidth) {
	filename = new char[strlen(fname)+1];
	memset(filename, 0, strlen(fname)+1);
	strcpy(filename, fname);
	
	linewidth = lwidth;
	head = NULL;
	tail = NULL;
	linecount = 0;
	
	line* cLine = NULL;
	
	FILE* bufferinput = fopen(filename, "rt");
	char readbuffer[linewidth+1];
	*readbuffer = 0;
	if (bufferinput) {
		do {
			fgets(readbuffer, linewidth+1, bufferinput);
			if (!*readbuffer) {
				tail = cLine;
				break;
			}

			cLine = new line(cLine, readbuffer, linewidth);
			linecount++;
			if (linecount == 1)
				head = cLine;
			*readbuffer = 0;
		} while(!feof(bufferinput));
	} else {
		printf("Error opening file: '%s'\n", filename);
		exit(1);
	}
	fclose(bufferinput);
}

linebuffer::~linebuffer() {
	delete filename;
	if (head == NULL) { return; }
	
	//delete all lines
	line* cLine = head;
	while (cLine->next != NULL) {
		delete cLine->next;
	}
	delete cLine;
	
	head = NULL;
	tail = NULL;
	linecount = 0;
}

void linebuffer::printlines() {
	line* cLine = head;
	if (cLine != NULL) {
		while (cLine->next != NULL) {
			if (endsWithLF(cLine->text))
				printf("%s", cLine->text);
			else
				printf("%s\n", cLine->text);
			cLine = cLine->next;
		}
		if (endsWithLF(cLine->text))
			printf("%s", cLine->text);
		else
			printf("%s\n", cLine->text);
		cLine = cLine->next;
	} else {
		printf("No Lines to print!\n");
	}
}

void linebuffer::setParams(line* someLine) {
	if (someLine == NULL) {
		head = NULL;
		tail = NULL;
		linecount = 0;
	} else {
		//reach head
		head = someLine;
		while (head->prev != NULL)
			head = head->prev;
		
		tail = head;
		linecount = 1;
		while (tail->next != NULL) {
			tail = tail->next;
			linecount++;
		}
	}
}

void linebuffer::wordWrap(line* cLine) {
	if (head == NULL) { return; }
	
	line* readLine = cLine;
	line* writeLine = cLine;
	
	char* remline = new char[linewidth*3];
	memset(remline, 0, linewidth*3);
	char* temp1 = new char[linewidth*3];
	memset(temp1, 0, linewidth*3);
	char* temp2 = new char[linewidth*3];
	memset(temp2, 0, linewidth*3);
	
	bool finished = false;
	while (!finished) {
		if (strlen(remline) == 0) {	//nothing remaining in the buffer
			if (readLine == NULL) {	//reading past last line as well
				finished = true;
			} else { //lines available to read
				strcpy(remline, readLine->text);
				readLine = readLine->next;
			}
		} else {	//there is text in remline, see if it is ready for processing
			if (strlen(remline) >= linewidth) {	//enough to be processed
				int wwlen = lastDisplayChar(remline, linewidth);
				
				memset(temp1, 0, linewidth*3);
				strncpy(temp1, remline, wwlen+1);
				temp1[wwlen+1] = 0;
				line* ln = new line(writeLine, temp1, linewidth, true);
				tail = ln;
				memset(temp2, 0, linewidth*3);
				strncpy(temp2, remline+wwlen+1, strlen(remline)-(wwlen+1));
				temp2[strlen(remline)-(wwlen+1)] = 0;
				
				memset(remline, 0, linewidth*3);
				strcpy(remline, temp2);
			} else {	//short text, read more
				if (endsWithLF(remline)) {	//short and has a LF
					line* ln = new line(writeLine, remline, linewidth, true);
					tail = ln;
					memset(remline, 0, linewidth*3);
				} else if (readLine == NULL) {	//reading past last line as well
					//process the available text & exit
					line* ln = new line(writeLine, remline, linewidth, true);
					tail = ln;
					finished = true;
				} else { //lines available to read
					strcat(remline, readLine->text);
					readLine = readLine->next;
				}
			}
		}
	}
	delete remline;
	delete temp1;
	delete temp2;
	
	//delete everything past the tail
	while (tail->next != NULL)
		delete tail->next;
		
	setParams(tail);
}

line* linebuffer::getLine(int lineNum) {
	if (lineNum < 0 || head == NULL) { return NULL; }
	
	int x = 0;
	line* cLine = head;
	while (cLine != NULL) {
		if (x == lineNum)
			return cLine;
		cLine = cLine->next;
		x++;
	}
	return NULL;
}

void linebuffer::lineWrap(line* cLine) {
	line* startLine;
	
	if (cLine->prev != NULL && !endsWithLF(cLine->prev->text)) {	//wrap starting prev line
		startLine = cLine->prev;
	} else {
		startLine = cLine;
	}
	
	int lineLen = linesToLF(startLine);
	if (lineLen == -1) {
		wordWrap(startLine);
		return;
	}
	
	char* readLine = new char[((linewidth+2)*(lineLen+1))+1];
	memset(readLine, 0, ((linewidth+2)*(lineLen+1))+1);
	char* insertBuffer = new char[linewidth+1];
	char* tempLine = new char[((linewidth+2)*(lineLen+1))+1];
	
	line* sLine = startLine;
	for (int x = 0; x <= lineLen; x++) {
		strcat(readLine, sLine->text);
		sLine = sLine->next;
	}
	
	while (strlen(readLine) != 0) {	//process the line
		//get the display length
		int wwlen = lastDisplayChar(readLine, linewidth);
		memset(insertBuffer, 0, linewidth+1);
		strncpy(insertBuffer, readLine, wwlen+1);
		insertBuffer[wwlen+1] = 0;
		line* ln = new line(startLine, insertBuffer, linewidth, true);
		
		memset(tempLine, 0, ((linewidth+2)*(lineLen+1))+1);
		strncpy(tempLine, readLine+wwlen+1, strlen(readLine)-(wwlen+1));
		tempLine[strlen(readLine)-(wwlen+1)] = 0;
		
		memset(readLine, 0, ((linewidth+2)*(lineLen+1))+1);
		strcpy(readLine, tempLine);
		
	}
	
	delete insertBuffer;
	delete tempLine;
	
	line* delLine = startLine->prev;
	for (int x = 0; x <= lineLen; x++) {
		if (delLine->next != NULL)
			delete delLine->next;
	}
	
	setParams(delLine);
}

int linebuffer::linesToLF(line* cLine) {	//Returns -1 if ENTER not found
	if (cLine == NULL) { return -1; }

	//bool enterFound = false;
	int linesToEnter = 0;
	line* curLine = cLine;

	do {
		if (endsWithLF(curLine->text)) {	//ENTER found
			return linesToEnter;
		}
		linesToEnter++;
		curLine = curLine->next;
	} while (curLine != NULL);
	return -1;
}

bool linebuffer::saveFile() {
	if (head == NULL) { return false; }
	bool fileSaved = false;
	
	FILE* fp =fopen(filename, "wt");
	if (fp == NULL)
		return fileSaved;

	//write the file
	line* wLine = head;
	do {
		//write wLine->text;
		fprintf(fp, "%s", wLine->text);
		wLine = wLine->next;
	} while(wLine != NULL);
	
	fclose (fp);
	saved = true;
	fileSaved = true;
	return fileSaved;
}

// end of buffer.cpp
