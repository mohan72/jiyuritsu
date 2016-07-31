# Makefile for JIYURITSU
# Copyright 2015, Mohanraj Dhanagopal

jiyuritsu: jiyuritsu.o twin.o buffer.o jutils.o
	g++ -o jiyuritsu jiyuritsu.o twin.o buffer.o jutils.o -lncurses


jiyuritsu.o: jiyuritsu.cpp twin.h buffer.h
	g++ -c jiyuritsu.cpp -lncurses

twin.o: twin.cpp buffer.h
	g++ -c twin.cpp -lncurses

buffer.o: buffer.cpp buffer.h jutils.h
	g++ -c buffer.cpp
	
jutils.o : jutils.cpp jutils.h
	g++ -c jutils.cpp

clean:
	@rm -f jiyuritsu
	@rm -f *.o
	@rm -f *.out

# end of Makefile
