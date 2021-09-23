all: shell.c
	gcc -g shellOld.c -o mysh

#builds the zip
lab5.zip: 
	zip -r lab5.zip shellOld.c makefile

lab5.tar: 
	tar cf lab5.tar shellOld.c makefile README.txt
