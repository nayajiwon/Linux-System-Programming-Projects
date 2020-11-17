ssu: ssu_crontab.o ssu_crond.o ssu_rsync.o 
	gcc -o ssu_crontab.out ssu_crontab.o
	gcc -o ssu_crond.out ssu_crond.o -lpthread
	gcc -o ssu_rsync.out ssu_rsync.o

ssu_crontab.o: ssu_crontab.h ssu_crontab.c
	gcc -c -o ssu_crontab.o ssu_crontab.c

ssu_crond.o: ssu_crond.h ssu_crond.c
	gcc -c -o ssu_crond.o ssu_crond.c -lpthread

ssu_rsync.o: ssu_rsync.h ssu_rsync.c
	gcc -c -o ssu_rsync.o ssu_rsync.c

