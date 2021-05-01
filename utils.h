#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <time.h>
#include <sys/shm.h>
#include <unistd.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef proj2Utils
#define proj2Utils

#define INIT_SEM(sem, sem_id, value)\
    sem_id = shmget(IPC_PRIVATE, sizeof(sem_t), 01777);\
    sem = shmat(sem_id, NULL, 0);\
    sem_init(sem, 1, value);

#define INIT_SHARED_INTEGER(integer, integer_id, value)\
    integer_id = shmget(IPC_PRIVATE, sizeof(int), 01777);\
    integer = shmat(integer_id, NULL, 0);\
    *integer = value;

#define DESTROY_SEM(sem, sem_id)\
    sem_destroy(sem);\
    shmctl(sem_id, IPC_RMID, NULL);

#define DESTROY_INT(int_id)\
    shmctl(int_id, IPC_RMID, NULL);


#define max_elf_count 999
#define min_elf_count 1

#define max_rd_count 19
#define min_rd_count 1

#define max_elf_time 1000
#define min_elf_time 0

#define max_rd_time 1000
#define min_rd_time 0

FILE *file;

int gate_sem_id;
sem_t *gate_sem;

int message_sem_id;
sem_t *message_sem;

int rd_go_sem_id;
sem_t *rd_go_sem;

int santa_do_id;
sem_t *santa_do;

int santa_help_id;
sem_t *santa_help;

int santa_helped_id;
sem_t *santa_helped;

int elf_workshop_id;
sem_t *elf_workshop;

int end_sem_id;
sem_t *end_sem;

int christmas_id;
int *christmas;

int end_id;
int *end;

int message_counter_id;
int *message_counter;

int elf_counter_id;
int *elf_counter;

int rd_counter_id;
int *rd_counter;

#endif