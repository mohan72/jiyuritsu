//twin.h
// Copyright 2015, Mohanraj Dhanagopal
#ifndef TWIN_H
#define TWIN_H

#include <ncurses.h>

#include "buffer.h"

class twin {
	WINDOW* editwin;
	char* filename;
	int winwidth;
	int winheight;
	linebuffer* buffer;
	line* curline;
	int topline;
	int curx, cury;
	bool ins;
	bool msgDisplayed;
public:
	twin(WINDOW* eWin, char* fName, int h, int w);
	~twin();
	void displaylines(int tLineNum);
	void process();
	
	void saveBuffer();
	void addEnter();
	void addChar(int charInt);
	void delLine();
	void delPrevChar();
	void delChar();
	void delWord();
	void moveLtByWord();
	void moveRtByWord();
	void moveEnd();
	void moveHome();
	void moveUpByChar();
	void moveDnByChar();
	void moveLtByChar();
	void moveRtByChar();
	void changeInsState();
	void displayKey(int k);
	void displayMsg(char* msg);
	void eraseMsg();
	void displayFileName();
	void displayCursorLocation();
	void displayUnderlyingChar();
};

#endif
