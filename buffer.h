// buffer.h - memory management -
// copyright 2015, Mohanraj Dhanagopal
#ifndef BUFFER_H
#define BUFFER_H

struct line {
	line* prev;
	line* next;
	char* text;

	line(line* cLine, char* txt, int maxlen, bool addbefore=false);
	~line();
	void setText(char* txt, int maxlen);
};

class linebuffer {
	char* filename;
	int linewidth;
	line* head;
	line* tail;
	int linecount;
	bool saved;
public:
	linebuffer(char* fname, int lwidth);
	~linebuffer();
	int lineCount() { return linecount; }
	void printlines();
	line* getHead() { return head; }
	line* getTail() { return tail; }
	void setParams(line* someLine);
	void wordWrap(line* cLine);
	void wordWrap() { wordWrap(head); }
	line* getLine(int lineNum);
	void lineWrap(line* cLine);
	int linesToLF(line* cLine);
	bool saveFile();
};

#endif
//end of buffer.h
