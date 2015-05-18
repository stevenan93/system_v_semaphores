/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2015.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/
//Edited By Steven An for Final Exam Take Home Part 1
//Dr Nguyen, CSC 060 Spring 2015
#include "curr_time.h"                      /* Declaration of currTime() */
#include "tlpi_hdr.h"
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "semun.h"


int
main(int argc, char *argv[])
{
    int j, dummy;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s sleep-time...\n", argv[0]);
    setbuf(stdout, NULL);                
    key_t semkey;
    if((semkey = ftok(".", 'a')) == (key_t)-1) //makes sure unique semaphore id key
    {
	perror("IPC error: ftok");
	exit(1);
    }	
    int semid = semget(semkey, 1, IPC_CREAT | S_IRUSR | S_IWUSR); //creates one semaphore
    if(semid == -1)
    {
	errExit("semid");
    }
    printf("Semaphore ID: %d\n", semid);
    union semun arg;
    struct sembuf sop;
    arg.val = 0; //init semaphore to 0
    if(semctl(semid, 0, SETVAL, arg) == -1) //execute semop
	errExit("semctl");

    for (j = 1; j < argc; j++) {
        switch (fork()) {
        case -1:
            errExit("fork %d", j);

        case 0: /* Child */
            sleep(getInt(argv[j], GN_NONNEG, "sleep-time"));           
            printf("%s  Child %d unlocking (PID=%ld)\n",
                    currTime("%T"), j, (long) getpid());
            /* Child increments by one */
	    sop.sem_num = 0;
	    sop.sem_op = 1;
	    sop.sem_flg = 0;
	    if(semop(semid, &sop, 1) == -1)
		errExit("semop");
	    _exit(EXIT_SUCCESS);
        default: /* Parent loops to create next child */
            break;
        }
    }  
	sop.sem_num = 0;
	sop.sem_op =  0 - (argc - 1); //parent will be locked here
	sop.sem_flg = 0;
	if(semop(semid, &sop, 1) == -1)
		errExit("semop");
       
	printf("%s: all obstacles removed, parents can do things now\n", currTime("%T"));
    /* Parent can now carry on to do other things... */
        if(semctl(semid, 0, IPC_RMID, dummy) == -1) 
		errExit("semctl");
        printf("semid: %d successfully removed\n", semid);
    exit(EXIT_SUCCESS);
}
