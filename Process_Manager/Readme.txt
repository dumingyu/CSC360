First :

PMan.c:A bunch of methods to run a program in the background and accepting user commands.

Makefile; for make command.

Readme.txt: instructions of how to utilize files in the current directory

Next:

1. run make to compile PMan.c 

2. run ./PMan

3. PMan: > should be displayed.

4. available commands:
	a. bgkill <pid>: terminate the process with pid

	b. bgstop <pid>: pause the process

	c. bgstart <pid>: re-start the process

	d. bg [command]: execute a program and initiate a process
			 for example: bg ./inf

	e. bglist <pid>:list all of the processes currently running in the background
	
	f. pstat <pid>: list the comm(filename of executable), state, utime, stime, rss(resident set size), voluntary_ctxt_swithes and nonvoluntary_ctxt_swithes of a process
			with given pid
	
5. To exit the program, simply Ctrl+Z or Ctrl+Shift+Z(depends on the setting of computer)

