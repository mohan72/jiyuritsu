# JIYURITSU (Freeform)
UNIX text editor for writers.

The tools an artist chooses to use define not only the process but also the results.
-Gwenn Seemel

For the past few months I have been toying with the idea of running a text mode UNIX console as my primary writing platform. After much experimentation I chose OpenBSD as my platform of choice; it just seemed like a solid system. I quickly realized that text editing in the command line is still quite a challenge, especially for writers. There is no dearth of text editors. In fact numerous choices are available and the challenge is selecting the right one. For writers unfortunately there is no solution that works well straight out of the box.

First I started with the VIM text editor. I instantly hit pretty steep road blocks. VIM needs a PhD in computing for even simple tasks. VIM insists that you read the manual and keep it handy before you write even a single line of text. I understand it was developed to replace the venerable 'vi' text editor, which was designed for TTY terminals, which is none of my problem. All I wanted to do was write a few sentences and have word-wrap turned on while I do that. It took me hours of searching online help to get wrap set at word boundaries. The learning curve seemed too steep and so I downloaded and installed the Nano text editor and almost instantly fell in love with its simplicity. My enthusiasm was short-lived. When I opened the files later in a grown-up word processor I uncovered a fatal flaw. Nano doesn't do word wrap in an elegant way. It will insert hard returns at the end of each line. I still haven't figured out a way to stop this behaviour despite trying out numerous configuration options.

Ultimately, I bulldozed through hefty manuals and managed to learn enough VIM to configure it to my liking. It is a supremely capable tool. No wonder it is akin to religion for many users. Once you understand the different modes of operation and the basic navigation commands, it is really a joy to use. If you are willing to put in the mandatory hours of hard work, look no further. If you are a newcomer to UNIX and are not sure about the choice of text editor, it is a different story. VIM is one of the most difficult text editors on earth for a beginner. For some people though, this difficulty is VIM's most attractive feature.

There is no single text editor for the UNIX text console that does soft-wrap at word boundaries out of the box. If there is one it is UNIX's best kept secret. If there is only an option to start VIM in an idiot mode!!

Soon, I started thinking about writing my own text editor. The spec was simple:

1. Word-wrap out of the box.
2. Navigate as I like.
3. No configuration options!

I started scouring the web and found many fantastic books and pages dedicated to the craft of text editor building. Dr. Dobbs had a section on Gap buffers and even included code to go with it. There was a whole book on text editor design. I bought the brilliant 'Type and Learn C++' mainly for the fact that it teaches development of a text editor as an example. My mind was not in relearning the nitty gritties of the C++ syntax and soon got tired of the code differences with the narrative. I wish one of the authors had developed a minimalist free or open source text editor for writers - not coders or system administrators.

Haiku is a form of minimalist Japanese poetry. To attain its minimalist nirvana, Haiku has a rigid three sentence structure and uses just 17 syllables in a 5-7-5 format. My text editor is also about simple things - a minimal set of essential editing features. At the same time I don't want it to be too restrictive. That is where jiyuritsu comes in. Jiyuritsu is freeform haiku without the 17-syllable restriction. I hope you find the 'jiyuritsu' text editor useful. If not, feel free to modify it and make it useful in your own way. At the moment it is less than 1500 lines of code and pretty easy to tinker with. There are only a handful of functions that do all the heavy lifting.

Any help to add features and to debug is welcome. Some of the key features missing:

1. Copy and Paste is completely missing. This makes the editor unsuitable for any serious work.
2. jiyuritsu uses unnamed.txt as an empty file at startup if no filename is provided. Changing this behaviour would help.

Writing. Alone?
