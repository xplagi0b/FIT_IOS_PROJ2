#include "utils.h"

int *parse(int argc, char *argv[]) {
    if(argc != 5) return NULL;
    int *nums = malloc(sizeof(int) * 4);
    if(nums == NULL) return NULL;

    for(int i = 1; i < argc; i++) {
        nums[i-1] = strtol(argv[i], NULL, 10);
    }

    if((nums[0] < min_elf_count) || (nums[0] > max_elf_count) ||
        (nums[1] < min_rd_count) || (nums[1] > max_rd_count) ||
        (nums[2] < min_elf_time) || (nums[2] > max_elf_time) ||
        (nums[3] < min_rd_time) || (nums[3]) > max_rd_time) {
            return NULL;
        }
    
    return nums;
}

void initSem(int rd) {
    INIT_SEM(gate_sem, gate_sem_id, 1);
    INIT_SEM(message_sem, message_sem_id, 1);
    INIT_SEM(rd_go_sem, rd_go_sem_id, 0);
    INIT_SEM(santa_do, santa_do_id, 0);
    INIT_SEM(santa_help, santa_help_id, 0);
    INIT_SEM(end_sem, end_sem_id, 0);
    INIT_SEM(elf_workshop, elf_workshop_id, 3);
    INIT_SEM(santa_helped, santa_helped_id, 0);

    INIT_SHARED_INTEGER(message_counter, message_counter_id, 1);
    INIT_SHARED_INTEGER(end, end_id, 0);
    INIT_SHARED_INTEGER(elf_counter, elf_counter_id, 0);
    INIT_SHARED_INTEGER(rd_counter, rd_counter_id, rd);
    INIT_SHARED_INTEGER(christmas, christmas_id, 0);
}

void deleteSem() {
    DESTROY_SEM(gate_sem, gate_sem_id);
    DESTROY_SEM(message_sem, message_sem_id);
    DESTROY_SEM(rd_go_sem, rd_go_sem_id);
    DESTROY_SEM(santa_do, santa_do_id);
    DESTROY_SEM(santa_help, santa_help_id);
    DESTROY_SEM(end_sem, end_sem_id);
    DESTROY_SEM(elf_workshop, elf_workshop_id);
    DESTROY_SEM(santa_helped, santa_helped_id);

    DESTROY_INT(message_counter_id);
    DESTROY_INT(elf_counter_id);
    DESTROY_INT(rd_counter_id);
    DESTROY_INT(christmas_id);
    DESTROY_INT(end_id);
}

void print_message(int id, int SaElRd, char *msg) {
    sem_wait(message_sem);
    if(SaElRd == 0) fprintf(file, "%d: Santa: ", *message_counter);
    else fprintf(file, "%d: %s %d: ", *message_counter, (SaElRd == 1 ? "Elf" : "RD"), id);
    fprintf(file, msg);
    (*message_counter)++;
    fprintf(file, "\n");
    fflush(file);
    sem_post(message_sem);
}

void print_help() {
    printf("usage: ./proj2 NE NR TE TR\n");
    printf("0 < NE < 1000\n");
    printf("0 < NR < 20\n");
    printf("0 <= TE <= 1000\n");
    printf("0 <= TR <= 1000\n");
}

void santa() {
    for(;;) {
        print_message(0, 0, "going to sleep");
        sem_wait(santa_do);
        sem_wait(gate_sem);
        if((*christmas) == 1) {
            break;
        }
        print_message(0, 0, "helping elves");
        sem_post(santa_help);
        sem_post(gate_sem);
        sem_wait(santa_helped);
    }
    print_message(0, 0, "closing workshop");
    sem_post(rd_go_sem);
    sem_wait(end_sem);
    print_message(0, 0, "Christmas started");
    (*end) = 1;
    sem_post(gate_sem);
    sem_post(santa_help);
}

void elf(int elf_id, int elfTime) {
    print_message(elf_id, 1, "started");
    for(;;) {
        if(elfTime != 0) {
            usleep((rand() % elfTime) * 1000);
        }
        print_message(elf_id, 1, "need help");
        sem_wait(elf_workshop);
        sem_wait(gate_sem);
        if((*end) == 1) {
            sem_post(gate_sem);
            break;
        }
        (*elf_counter)++;
        if((*elf_counter) == 3) {
            sem_post(santa_do);
        }
        sem_post(gate_sem);
        sem_wait(santa_help);
        if((*end) == 1) {
            break;
        }
        (*elf_counter)--;
        print_message(elf_id, 1, "get help");
        if((*elf_counter) == 0) {
            sem_post(santa_helped);
            sem_post(elf_workshop);
            sem_post(elf_workshop);
            sem_post(elf_workshop);
        } else {
            sem_post(santa_help);
        }
    }
    sem_wait(rd_go_sem);
    print_message(elf_id, 1, "taking holidays");
    sem_post(rd_go_sem);
    sem_post(elf_workshop);
    sem_post(santa_help);
}

void rd(int rd_id, int rdTime, int rds) {
    print_message(rd_id, 2, "rstarted");
    if(rdTime != 0) {
        usleep(((rand() % (rdTime / 2)) + (rdTime / 2)) * 1000);
    }
    print_message(rd_id, 2, "return home");
    sem_wait(gate_sem);
    (*rd_counter)--;
    if((*rd_counter) == 0) {
        (*christmas) = 1;
        sem_post(santa_do);
    }
    sem_post(gate_sem);

    sem_wait(rd_go_sem);
    print_message(rd_id, 2, "get hitched");
    (*rd_counter)++;
    if((*rd_counter) == rds) {
        sem_post(end_sem);
    }
    sem_post(rd_go_sem);
}

int main(int argc, char *argv[]) {
    int *in = parse(argc, argv);
    if(in == NULL) { print_help(); return 1; }
    file = fopen("proj2.out", "w");
    if(file == NULL) { fprintf(stderr, "Chyba pri otevirani souboru proj2.out\n"); return 1; }
    initSem(in[1]);
    srand(time(NULL));

    // Vytvor Santu
    pid_t id = fork();
    if(id == 0) {
        santa();
        free(in);
        fclose(file);
        return 0;
    }

    for(int i = 0; i < in[0]; i++) {
        id = fork();
        if(id == 0) {
            elf(i + 1, in[2]);
            free(in);
            fclose(file);
            return 0;
        }
    }

    for(int i = 0; i < in[1]; i++) {
        id = fork();
        if(id == 0) {
            rd(i + 1, in[3], in[1]);
            free(in);
            fclose(file);
            return 0;
        }
    }

    while(wait(NULL)) {
        if(errno == 10)
            break;
    }
    deleteSem();

    fclose(file);
    free(in);
    return 0;
}