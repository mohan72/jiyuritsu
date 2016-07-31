// twin.cpp
// Copyright 2015, Mohanraj Dhanagopal

#include <ncurses.h>
#include <string.h>

#include "cmd.h"
#include "jutils.h"
#include "twin.h"
#include "buffer.h"

twin::twin(WINDOW* eWin, char* fName, int h, int w) {
	editwin = eWin;
	filename = new char[strlen(fName)+1];
	strcpy(filename, fName);
	winwidth = w;
	winheight = h;
	topline = 0;
	curx = 0;
	cury = 0;
	ins = true;
	msgDisplayed = false;
	
	buffer = new linebuffer(filename, winwidth);
	buffer->wordWrap();
	curline = buffer->getHead();
	displayFileName();
	displaylines(topline);
	displayCursorLocation();
}

twin::~twin() {
	delete buffer;
}

void twin::process() {
	bool done = false;
	while (!done) {
		int key = getch();
		displayKey(key);
		eraseMsg();
		
		switch (key) {
			case CTRL_X:	//EXIT
				done = true;
				break;
			case INS:		//INS
				changeInsState();
				break;
			case RT_ARROW:	//Right Arrow Key
				moveRtByChar();
				break;
			case LT_ARROW:	//Left Arrow Key
				moveLtByChar();
				break;
			case DN_ARROW:	//Down Arrow Key
				moveDnByChar();
				break;
			case UP_ARROW:	//Up Arrow Key
				moveUpByChar();
				break;
			case HOME:		//Home Key
				moveHome();
				break;
			case END:		//End Key
				moveEnd();
				break;
			case CTRL_RT_ARROW:	//Move to start of next word
				moveRtByWord();
				break;
			case CTRL_LT_ARROW:	//Move to start of prev word
				moveLtByWord();
				break;
			case DEL:
				delChar();
				break;
			case BKSP:
				delPrevChar();
				break;
			case CTRL_W:	//Delete word
				delWord();
				break;
			case CTRL_Y:	//Delete line
				delLine();
				break;
			case ENTER:
				addEnter();
				break;
			case F10:	//Save requested
				saveBuffer();
				break;
			default:
				if (key >= 32 && key <= 126)	//128 - 255 are extended characters
					addChar(key);
				break;
		}
		displayUnderlyingChar();
		displayCursorLocation();
	}
}

void twin::delWord() {
	if (curline == NULL) { return; }
	if (curx >= lastCursorStop(curline->text, winwidth)) {
		//moveRtByChar();
		return;
	}
	
	bool spaceFound = false;
	for (int i = curx; i < lastCursorStop(curline->text, winwidth); i++) {
		if (curline->text[i] == ' ') {
			spaceFound = true;
		}
		
		if (spaceFound == true && curline->text[i] != ' ' && curline->text[i] != '\n') {
			int nextWord = i;
			int delLen = i - curx;
			delchars(curline->text, curx, delLen);
			displaylines(topline);
			return;
		}
	}
	if (curx == 0) {
		delLine();	//last word
	} else {
		int delLen = strlen(curline->text) - curx;
		delchars(curline->text, curx, delLen);
		displaylines(topline);
	}
	return;
}

void twin::delLine() {
	if (curline == NULL) { return; }
	if (curline == buffer->getHead() && curline == buffer->getTail()) {	//Last Line
		curline->setText((char*)"", winwidth);
		curx = 0;
	} else {	//at lease two lines exist
		if (curline->next != NULL) {
			line* ln = curline->next;
			delete curline;
			buffer->setParams(ln);
			curline = ln;
			curx = 0;
		}
		else {
			line* ln = curline->prev;
			delete curline;
			buffer->setParams(ln);
			curline = ln;
			curx = 0;
			if (cury > 0) {
				cury--;
			} else {
				topline--;
			}
		}
	}
	displaylines(topline);
	wmove(editwin, cury, curx);
	wrefresh(editwin);
}

void twin::saveBuffer() {
	bool saved = buffer->saveFile();
	if (!saved) {
		displayMsg((char*)"Error saving file...");
	} else {
		displayMsg((char*)"Buffer saved successfully.");
	}
}

void twin::addEnter() {
	if (curline == NULL) { return; }
	
	char* temp = new char[winwidth+1];
	memset(temp, 0, winwidth+1);
	strncpy(temp, curline->text+curx, strlen(curline->text)-curx+1);
	//temp[strlen(curline->text)-curx+1] = '\n';
	line* ln = new line(curline, temp, winwidth);
	
	memset(curline->text+curx, 0, winwidth-curx+1);
	curline->text[curx] = '\n';
	
	delete temp;
	
	curline = curline->next;
	curx = 0;
	
	if (cury == (winheight-1)) {	//Last line of window
		topline++;
	} else {
		cury++;
	}
	buffer->setParams(ln);
	buffer->lineWrap(curline);
	curline = buffer->getLine(topline+cury);
	displaylines(topline);
	wmove(editwin, cury, curx);
	wrefresh(editwin);
}

void twin::addChar(int charInt) {
	if (charInt < 32 && charInt > 126) { return; }
	if (curline == NULL) { return; }
	
	if (curline->text[curx] == 0) {	//dealing with a space-ended line past the space
		//End of line here
		moveRtByChar();
	}
	// else {
	int len = strlen(curline->text);
	for (int x = len; x > curx; x--) {
		curline->text[x] = curline->text[x-1];
	}
	curline->text[curx] = charInt;
	curx++;
	//}
	
	int prevLen = -1;
	if (curline->prev != NULL)
		prevLen = strlen(curline->prev->text);
	
	buffer->lineWrap(curline);
	//buffer->lineWrap(curline);
	curline = buffer->getLine(topline+cury);
	int newPrevLen = -1;
	if (curline->prev != NULL)
		newPrevLen = strlen(curline->prev->text);
	if (prevLen != -1 && prevLen != newPrevLen) {	//Part moved to prev line
		int lenAdded = newPrevLen - prevLen;
		if (curx < lenAdded) {	//cursor moved to previous line
			curx = prevLen + curx;
			curline = curline->prev;
			if (cury > 0) {
				cury--;
			} else {
				topline--;
			}
			//displaylines(topline);
		} else {	//part moved but not the cursor
			curx -= newPrevLen - prevLen;
		}
	} else {	//last line remained the same
		if (lastCursorStop(curline->text, winwidth) < curx) {
			curx = curx - (lastCursorStop(curline->text, winwidth));
			if (curline->next != NULL) {
				curline = curline->next;
				if (curx > lastCursorStop(curline->text, winwidth))	//hack
					curx = lastCursorStop(curline->text, winwidth);
			} else {
				//add a line and then move
				line* nLine = new line(curline, (char*)"", winwidth);
				buffer->setParams(curline);
				curline = curline->next;
				curx = 0;	//hack
			}
			
			if (cury < winheight-1)
				cury++;
			else
				topline++;
			
		}
	}
	displaylines(topline);
}

void twin::displaylines(int tLineNum) {
	werase(editwin);
	line* dLine = buffer->getHead();
	if (dLine == NULL) { return; }
	if (tLineNum > (buffer->lineCount())-1) { return; }
	
	dLine = buffer->getLine(tLineNum);
	int linesDisplayed = 0;
	do {
		mvwprintw(editwin, linesDisplayed, 0, "%s", dLine->text);
		dLine = dLine->next;
		linesDisplayed++;
	} while(dLine != NULL && linesDisplayed < winheight);
	wmove(editwin, cury, curx);
	wrefresh(editwin);
}

void twin::delPrevChar() {
	if (curline == NULL) { return; }
	if (curx == 0 && curline->prev == NULL) { return; }
	
	if (curx == 0 && curline->prev->text[strlen(curline->prev->text)-1] == ' ') {
		moveLtByChar();
	}
	
	moveLtByChar();
	delChar();
}

void twin::delChar() {
	//do not delete if there are no chars
	if (curline == NULL) { return; }
	
	int pLen = -1;
	int pCLen = -1;
	int pInc = 0;
	int cLen = strlen(curline->text);
	int cCLen = 0;
	int nLen = -1;
	int nCLen = -1;
	if (curline->prev != NULL)
		pLen = strlen(curline->prev->text);
	if (curline->next != NULL)
		nLen = strlen(curline->next->text);
		
	if (curline->text[curx] != 0) {
		delchars(curline->text, curx, 1);
		buffer->lineWrap(curline);
		curline = buffer->getLine(topline + cury);
		if (curline == NULL) {
			curline = buffer->getLine(topline + cury - 1);
			pInc = strlen(curline->text) - pLen;
			curx = pLen + curx;
			if (cury > 0) {
				cury--;
			} else {
				topline--;
			}
		} else {
			if (curline->prev != NULL)
				pInc = strlen(curline->prev->text) - pLen;
			if (pInc > 0) {
				if (pInc >= curx) {
					curline = curline->prev;
					curx = pLen + curx;
					if (cury > 0 )
						cury--;
					else {
						topline--;
					}
				} else {
					curx -= pInc;
				}
			} else {
				//cCLen = strlen(curline->text) - cLen;
				if (curx > lastCursorStop(curline->text, winwidth)) {	//word has moved to next line
					curx = cLen - strlen(curline->text) - 1;
					curline = curline->next;
					if (cury < winwidth-1) {
						cury++;
					} else {
						topline++;
					}
				}
			}
		}
		displaylines(topline);
	} else {	//dealing with del at the end of a line
		if (curline->next != NULL) {
			moveRtByChar();
			delChar();
		}
	}
}

void twin::moveLtByWord() {
	if (curline == NULL) { return; }
	if (curx <= 0) {
		moveLtByChar();
		return;
	}
	
	int z = curx-1;
	if (curline->text[curx] != ' ' && curline->text[curx-1] == ' ') {
		z--;
	}
	
	for (int i = z; i >= 0; i--) {
		if (curline->text[i] == ' ' && curline->text[i+1] != ' ') {
			curx = i+1;
			wmove(editwin, cury, curx);
			wrefresh(editwin);
			return;
		}
		
		if (curline->text[i] != ' ' && i == 0) {
			curx = 0;
			wmove(editwin, cury, curx);
			wrefresh(editwin);
			return;
		}
	}
}

void twin::moveRtByWord() {
	if (curline == NULL) { return; }
	if (curx >= lastCursorStop(curline->text, winwidth)) {
		moveRtByChar();
		return;
	}
	
	bool spaceFound = false;
	for (int i = curx; i < lastCursorStop(curline->text, winwidth); i++) {
		if (curline->text[i] == ' ') {
			spaceFound = true;
		}
		
		if (spaceFound == true && curline->text[i] != ' ' && curline->text[i] != '\n') {
			curx = i;
			wmove(editwin, cury, curx);
			wrefresh(editwin);
			return;
		}
	}
	
	curx = lastCursorStop(curline->text, winwidth);
	wmove(editwin, cury, curx);
	wrefresh(editwin);
}

void twin::moveEnd() {
	if (curline == NULL) { return; }
	if (lastCursorStop(curline->text, winwidth) != curx) {
		curx = lastCursorStop(curline->text, winwidth);
	}
	wmove(editwin, cury, curx);
	wrefresh(editwin);
}

void twin::moveHome() {
	if (curline == NULL) { return; }
	if (curx != 0) {
		curx = 0;
	}
	wmove(editwin, cury, curx);
	wrefresh(editwin);
}

void twin::moveUpByChar() {
	if (curline == NULL) { return; }
	//Determine if lines left
	if (curline != buffer->getHead()) {
		curline = curline->prev;
		if (lastCursorStop(curline->text, winwidth) < curx) {	//move to end of line
			curx = lastCursorStop(curline->text, winwidth);
		}
		
		if (cury > 0) {	//not yet at the top of the screen
			cury--;
		} else {	//at the bottom of the screen with lines available to scroll
			topline--;
			displaylines(topline);
		}
		wmove(editwin, cury, curx);
		wrefresh(editwin);
	}
}

void twin::moveDnByChar() {
	if (curline == NULL) { return; }
	//Determine if lines left
	if (curline != buffer->getTail()) {
		curline = curline->next;
		if (lastCursorStop(curline->text, winwidth) < curx) {	//move to end of line
			curx = lastCursorStop(curline->text, winwidth);
		}

		if (cury < (winheight-1)) {	//not yet at the bottom of the screen
			cury++;
		} else {	//at the bottom of the screen with lines available to scroll
			topline++;
			displaylines(topline);
		}
		wmove(editwin, cury, curx);
		wrefresh(editwin);
	}
}

void twin::moveLtByChar() {
	if (curline == NULL) { return; }
	//determine if space available
	if (curx > 0) {	//space available
		curx--;
	} else {
		if (curline == buffer->getHead()) { return; }	//start of the buffer reached
		
		curline = curline->prev;
		curx = lastCursorStop(curline->text, winwidth);
		if (cury == 0) {	//First line of window
			topline--;
			displaylines(topline);
		} else {	//top line reached
			cury--;
		}
	}
	wmove(editwin, cury, curx);
	wrefresh(editwin);
}

void twin::moveRtByChar() {
	if (curline == NULL) { return; }
	//determine if space available
	if (curx < lastCursorStop(curline->text, winwidth)) {
		curx++;
	} else {
		if (curline == buffer->getTail()) { return; }	//end of the buffer reached
		
		curline = curline->next;
		curx = 0;
		
		if (cury == (winheight-1)) {	//Last line of window
			topline++;
			displaylines(topline);
		} else {
			cury++;
		}
	}
	wmove(editwin, cury, curx);
	wrefresh(editwin);
}

void twin::changeInsState() {
	attron(A_REVERSE);
	if (ins) {
		ins = false;
		mvprintw(winheight+2, winwidth-4, "OVR");
		curs_set(2);
	} else {
		ins = true;
		mvprintw(winheight+2, winwidth-4, "INS");
		curs_set(1);
	}
	attroff(A_REVERSE);
	wrefresh(stdscr);
	wmove(editwin, cury, curx);
	wrefresh(editwin);
}

void twin::displayKey(int k) {
	attron(A_REVERSE);
	mvwprintw(stdscr, winheight+2, winwidth-15, "     ");
	mvwprintw(stdscr, winheight+2, winwidth-15, "%d", k);
	attroff(A_REVERSE);
	wrefresh(stdscr);
	wmove(editwin, cury, curx);
	wrefresh(editwin);
}

void twin::displayCursorLocation() {
	attron(A_REVERSE);
	mvwprintw(stdscr, winheight+2, winwidth-10, "      ");
	mvwprintw(stdscr, winheight+2, winwidth-10, "%d,%d", cury, curx);
	attroff(A_REVERSE);
	wrefresh(stdscr);
	wmove(editwin, cury, curx);
	wrefresh(editwin);
}

void twin::displayUnderlyingChar() {
	attron(A_REVERSE);
	mvwprintw(stdscr, winheight+2, winwidth-20, "     ");
	mvwprintw(stdscr, winheight+2, winwidth-20, "%d", curline->text[curx]);
	attroff(A_REVERSE);
	wrefresh(stdscr);
	wmove(editwin, cury, curx);
	wrefresh(editwin);
}

void twin::displayMsg(char* msg) {
	//attron(A_REVERSE);
	mvwprintw(stdscr, 0, 25, "                                                  ");
	mvwprintw(stdscr, 0, 25, "%s", msg);
	//attroff(A_REVERSE);
	wrefresh(stdscr);
	wmove(editwin, cury, curx);
	wrefresh(editwin);
	msgDisplayed = true;
}

void twin::eraseMsg() {
	if (msgDisplayed == false) { return; }
	//attron(A_REVERSE);
	mvwprintw(stdscr, 0, 25, "                                                  ");
	//mvwprintw(stdscr, 0, 25, "%s", msg);
	//attroff(A_REVERSE);
	wrefresh(stdscr);
	wmove(editwin, cury, curx);
	wrefresh(editwin);
	msgDisplayed = false;
}

void twin::displayFileName() {
	attron(A_REVERSE);
	mvwprintw(stdscr, 1, 1, "                                                  ");
	mvwprintw(stdscr, 1, 0, "%s", filename);
	attroff(A_REVERSE);
	wrefresh(stdscr);
	wmove(editwin, cury, curx);
	wrefresh(editwin);
}

//end of twin.cpp
