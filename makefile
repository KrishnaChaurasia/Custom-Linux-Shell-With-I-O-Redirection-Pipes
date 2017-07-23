all: mypwd mymkdir myrm mymv myps myls mytail mychdir myshell run

mypwd:
	gcc mypwd.c -o mypwd.out

mymkdir:
	gcc mymkdir.c -o mymkdir.out

myrm:
	gcc myrm.c -o myrm.out

mymv:
	gcc mymv.c -o mymv.out

myps:
	gcc myps.c -o myps.out

myls:
	gcc myls.c -o myls.out

mytail:
	gcc mytail.c -o mytail.out

mychdir:
	gcc mychdir.c -o mychdir.out

myshell:
	gcc myshell.c -o myshell.out

run:
	./myshell.out
