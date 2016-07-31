//jiyuritsu.cpp - main cpp file for the JIYURITSU text editor
// Copyright 2015, Mohanraj Dhanagopal

#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <stdlib.h>

#include "cmd.h"
#include "jutils.h"
#include "buffer.h"
#include "twin.h"

//Global variables
int WINWIDTH;
int WINHEIGHT;
//WINDOW* textWin;
char* applicationName = (char*)"JIYURITSU (Freeform)";
char* fileToOpen;
bool fileNameSupplied = false;

void displayHelp();

int main(int argc, char *argv[]) {
	if( argc == 2 ) {
		//printf("The argument supplied is %s\n", argv[1]);
		fileToOpen = new char[strlen(argv[1])+1];
		strcpy(fileToOpen, argv[1]);
		fileNameSupplied = true;
	}
	
	//Initiate the NCurses System
	initscr();
	keypad(stdscr, TRUE);
	curs_set(1);
	noecho();
	
	//Set the Screen Parameters
	getmaxyx(stdscr, WINHEIGHT, WINWIDTH);
	
	//App Name boiler plate!
	mvprintw(0, 0, "%s", applicationName);
	
	//Top Bar below menu
	attron(A_REVERSE);
	for (int i = 0; i < WINWIDTH; i++) {
		mvaddch(1, i, ' ');
	}
	//Bottom Status Bar
	for (int i = 0; i < WINWIDTH; i++) {
		mvaddch(WINHEIGHT-1, i, ' ');
	}
	
	mvprintw(WINHEIGHT-1, 1, "^X:QUIT");
	mvprintw(WINHEIGHT-1, 10, "^S:SAVE");
	mvprintw(WINHEIGHT-1, 20, "F1:HELP");
	mvprintw(WINHEIGHT-1, WINWIDTH-4, "INS");
	attroff(A_REVERSE);
	refresh();
	
	//displayHelp();
	
	WINDOW* textWin = newwin(WINHEIGHT-3, WINWIDTH, 2, 0);
	wrefresh(textWin);
	
	twin* tEditorWin;
	if (fileNameSupplied == true) {	//name was supplied
		if (fileExists(fileToOpen)) {	//file exists, so open it
			tEditorWin = new twin(textWin, fileToOpen, WINHEIGHT-3, WINWIDTH);
		} else {	//doesn't exist, so create and open
			if (createFile(fileToOpen) == true) {
				tEditorWin = new twin(textWin, fileToOpen, WINHEIGHT-3, WINWIDTH);
			} else {
				printf("Error creating file. Please check access.\n");
				exit(1);
			}
		}
	} else {
		fileToOpen = new char[strlen((char*)"unnamed.txt")+1];
		strcpy(fileToOpen, (char*)"unnamed.txt");
		fileNameSupplied = true;
		if (createFile(fileToOpen) == true) {
			tEditorWin = new twin(textWin, fileToOpen, WINHEIGHT-3, WINWIDTH);
		} else {
			printf("Error creating file. Please check access.\n");
			exit(1);
		}
	}
	tEditorWin->process();
	
	//getch();
	delete tEditorWin;
	endwin();
	return 0;
}

void displayHelp() {
	curs_set(0);
	//noecho();
	WINDOW* helpWin = newwin(10, 40, (WINHEIGHT/2)-5, (WINWIDTH/2)-20);
	box(helpWin, 0 , 0);
	//Print Help
	mvwprintw(helpWin, 2, 3, "^N : NEW");
	mvwprintw(helpWin, 3, 3, "^O : OPEN");
	mvwprintw(helpWin, 4, 3, "^S : SAVE");
	mvwprintw(helpWin, 5, 3, "^A : SAVE-AS");
	mvwprintw(helpWin, 6, 3, "^X : QUIT");
	mvwprintw(helpWin, 2, 20, "^Y : DELETE LINE");
	mvwprintw(helpWin, 3, 20, "^W : DELETE WORD");
	//mvwprintw(helpWin, 3, 15, "^S : SAVE");
	//mvwprintw(helpWin, 4, 15, "^A : SAVE-AS");
	//mvwprintw(helpWin, 5, 15, "^X : QUIT");
	
	mvwprintw(helpWin, 8, 6, "Press Any Key to Continue...");
	wrefresh(helpWin);
	
	getch();
	wborder(helpWin,' ',' ',' ',' ',' ',' ',' ',' ');
	wrefresh(helpWin);
	delwin(helpWin);
	curs_set(1);
	//noecho();
}

// end of jiyuritsu.cpp
