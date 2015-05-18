<<<<<<< HEAD
/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2015.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

#include "curr_time.h"                      /* Declaration of currTime() */
#include "tlpi_hdr.h"
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "semun.h"


int
main(int argc, char *argv[])
{
    int j, dummy, semid;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s sleep-time...\n", argv[0]);

    setbuf(stdout, NULL);                   /* Make stdout unbuffered, since we
                                               terminate child with _exit() */
											   
											   
    
    key_t semkey;
	if((semkey = ftok(".", 'a')) == (key_t)-1){
		perror("IPC error: ftok");
		exit(1);
	}
	union semun arg;
	semid = semget(semkey, 1, IPC_CREAT | S_IRUSR | S_IWUSR);
	if(semid == -1)
	{
		errExit("semid");
	}
    printf("Semaphore ID : %d\n", semid);
	arg.val = 0;
	if(semctl(semid, 0, SETVAL, arg) == -1)
		errExit("semctl");
	
    printf("%s  Parent started\n", currTime("%T"));

    for (j = 1; j < argc; j++) {
        switch (fork()) {
        case -1:
            errExit("fork %d", j);

        case 0: /* Child */

            /* Child does some work, and lets parent know it's done */

            sleep(getInt(argv[j], GN_NONNEG, "sleep-time"));
                                            /* Simulate processing */
            printf("%s  Child %d (PID=%ld) unlocking\n",
                    currTime("%T"), j, (long) getpid());
            /* Child now carries on to do other things... */
            struct sembuf sop;
			sop.sem_num = 0;
			sop.sem_op = 1;
			sop.sem_flg = 0;
			printf("%ld: about to semop at  %s\n", (long) getpid(), currTime("%T"));
			if(semop(semid, &sop, 1) == -1)
				errExit("semop");
			printf("%ld: semop completed at %s\n", (long) getpid(), currTime("%T"));

            _exit(EXIT_SUCCESS);

        default: /* Parent loops to create next child */
            break;
        }
    }

    /* Parent comes here; close write end of pipe so we can see EOF */
	
    /* Parent may do other work, then synchronizes with children */
    struct sembuf sop;
    printf("%s  Parent ready to go\n", currTime("%T"));
	sop.sem_num = 0;
	sop.sem_op = -3;
	sop.sem_flg = 0;
	printf("%ld: about to semop at  %s\n", (long) getpid(), currTime("%T"));
	if(semop(semid, &sop, 1) == -1)
		errExit("semop");
	printf("%ld: semop completed at %s\n", (long) getpid(), currTime("%T"));
    /* Parent can now carry on to do other things... */

    exit(EXIT_SUCCESS);
}
=======
/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2015.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

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

    setbuf(stdout, NULL);                   /* Make stdout unbuffered, since we
                                               terminate child with _exit() */
											   
											   
    
        key_t semkey;
	if((semkey = ftok(".", 'a')) == (key_t)-1){
		perror("IPC error: ftok");
		exit(1);
	}
	
	int semid = semget(semkey, 1, IPC_CREAT | S_IRUSR | S_IWUSR);
	if(semid == -1)
	{
		errExit("semid");
	}
        printf("Semaphore ID: %d\n", semid);
	union semun arg;
	struct sembuf sop;
	arg.val = 0; //init semaphore to 0
	if(semctl(semid, 0, SETVAL, arg) == -1)
		errExit("semctl");

    for (j = 1; j < argc; j++) {
        switch (fork()) {
        case -1:
            errExit("fork %d", j);

        case 0: /* Child */

            /* Child does some work, and lets parent know it's done */

            sleep(getInt(argv[j], GN_NONNEG, "sleep-time"));
                                            /* Simulate processing */
            printf("%s  Child %d (PID=%ld) unlocking\n",
                    currTime("%T"), j, (long) getpid());
            /* Child increments by one */
			sop.sem_num = 0;
			sop.sem_op = 1;
			sop.sem_flg = 0;
	//		printf("%ld: about to semop at  %s\n", (long) getpid(), currTime("%T"));
			if(semop(semid, &sop, 1) == -1)
				errExit("semop");
	//		printf("%ld: semop completed at %s\n", (long) getpid(), currTime("%T"));

            _exit(EXIT_SUCCESS);

        default: /* Parent loops to create next child */
            break;
        }
    }

    /* Parent comes here; close write end of pipe so we can see EOF */
	
       
	sop.sem_num = 0;
	sop.sem_op =  0 - (argc - 1); //parent will be locked here
	sop.sem_flg = 0;
	//printf("%ld: about to semop at  %s\n", (long) getpid(), currTime("%T"));
	if(semop(semid, &sop, 1) == -1)
		errExit("semop");
       
	printf("%ld: all obstacles removed, parents can do things now %s\n", (long) getpid(), currTime("%T"));
    /* Parent can now carry on to do other things... */
        if(semctl(semid, 0, IPC_RMID, dummy) == -1) 
		errExit("semctl");
        printf("semid: %d successfully removed\n", semid);
    exit(EXIT_SUCCESS);
}
>>>>>>> b51d760072faafed713ef82b5382a0e5353eae38
