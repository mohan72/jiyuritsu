//jutils.h
// Copyright 2015, Mohanraj Dhanagopal
#ifndef JUTILS_H
#define JUTILS_H

//general purpose utility functions
bool endsWithLF(char* txt);
int lastCursorStop(char* txt, int maxwidth);
int lastDisplayChar(char* txt, int maxwidth);
void delchars(char* delLine, int startPos, int delLen);
bool fileExists(char* fName);
bool createFile(char* fName);
#endif
//end of jutils.h
