app.out: ssu_mntr.o ssu_size.o ssu_tree.o ssu_recover.o ssu_daemon.o
	gcc -o app.out ssu_mntr.o ssu_size.o ssu_tree.o ssu_recover.o -lpthread
	gcc -o ssu_daemon.out ssu_daemon.o

ssu_mntr.o: ssu_mntr.h ssu_mntr.c 
	gcc -c -o ssu_mntr.o ssu_mntr.c

ssu_size.o: ssu_mntr.h ssu_size.c
	gcc -c -o ssu_size.o ssu_size.c

ssu_tree.o: ssu_mntr.h ssu_tree.c
	gcc -c -o ssu_tree.o ssu_tree.c

ssu_recover.o: ssu_mntr.h ssu_recover.c
	gcc -c -o ssu_recover.o ssu_recover.c

ssu_daemon.o: ssu_daemon.h ssu_daemon.c
	gcc -c -o ssu_daemon.o ssu_daemon.c
