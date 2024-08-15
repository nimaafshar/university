//
// Created by Nima on 8/4/2020.
//
#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>


#define SHM_A_KEY 0x98765 //some arbitrary keys for shared memory
#define SHM_B_KEY 0x56789
#define SEM_KEY 0x18920 //some arbitrary key for semaphore
#define BLOCK_COUNT 5
#define BUFFER_SIZE 8200

#define BLOCK_AVAILABLE 2
#define BLOCK_WROTE 0
#define BLOCK_WAITING 1
#define MAX_TRIES 10

struct a_seg {
    int server_ready;
    int status[BLOCK_COUNT];
    char buffer_arr[BLOCK_COUNT][BUFFER_SIZE];
};
struct b_seg {
    char buffer_arr[BLOCK_COUNT][BUFFER_SIZE];
};

void uppercase_string(char *string, int size) {
    for (int i = 0; i < size; i++) {
        string[i] = toupper(string[i]);
    }
}

void initialize(struct a_seg *shm_a) {
    int semid;
    struct sembuf sem_buf;
    struct semid_ds buf;
    int retval;
    semid = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0666);
//    printf("errno is %d and semid is %d\n", errno, semid);

    /* Got the semaphore */
    if (semid >= 0) {
        printf("Semaphore:First Process\n");
        sem_buf.sem_op = 1;
        sem_buf.sem_flg = 0;
        sem_buf.sem_num = 0;
        retval = semop(semid, &sem_buf, 1);
        if (retval == -1) {
            perror("Semaphore Operation: ");
            return;
        }
    } else if (errno == EEXIST) { // Already other process got it
        int ready = 0;
        printf("Semaphore:Second Process\n");
        semid = semget(SEM_KEY, 1, 0);
        if (semid < 0) {
            perror("Semaphore GET: ");
            return;
        }

//        /* Waiting for the resource */
//        sem_buf.sem_num = 0;
//        sem_buf.sem_op = 0;
//        sem_buf.sem_flg = SEM_UNDO;
//        retval = semop(semid, &sem_buf, 1);
//        if (retval == -1) {
//            perror("Semaphore Locked: ");
//            return;
//        }
    }
    sem_buf.sem_num = 0;
    sem_buf.sem_op = -1; /* Allocating the resources */
    sem_buf.sem_flg = SEM_UNDO;
    retval = semop(semid, &sem_buf, 1);

    if (retval == -1) {
        perror("Semaphore Locked: ");
        return;
    }

    shm_a->server_ready = 1;
    for (int i = 0; i < BLOCK_COUNT; i++) {
        shm_a->status[i] = BLOCK_AVAILABLE;
    }

    sem_buf.sem_op = 1; /* Releasing the resource */
    retval = semop(semid, &sem_buf, 1);

    if (retval == -1) {
        perror("Semaphore Locked\n");
        return;
    }
}

void remove_semaphore() {
    int semid;
    int retval;
    semid = semget(SEM_KEY, 1, 0);
    if (semid < 0) {
        perror("Remove Semaphore: Semaphore GET: ");
        return;
    }
    retval = semctl(semid, 0, IPC_RMID);
    if (retval == -1) {
        perror("Remove Semaphore: Semaphore CTL: ");
        return;
    }
    return;
}

int process_blocks(struct a_seg *shm_a, struct b_seg *shm_b) {
    /**
     * returns number of processed blocks
     * in case of failure returns -1
     */
    int blocks_processed = 0;
    int semid;
    struct sembuf sem_buf;
    struct semid_ds buf;
    int retval;
    semid = semget(SEM_KEY, 1, 0);
    if (semid < 0) {
        perror("Remove Semaphore: Semaphore GET: ");
        return -1;
    }
//    printf("errno is %d and semid is %d\n", errno, semid);

    sem_buf.sem_num = 0;
    sem_buf.sem_op = -1; /* Allocating the resources */
    sem_buf.sem_flg = SEM_UNDO;
    retval = semop(semid, &sem_buf, 1);

    if (retval == -1) {
        perror("Semaphore Locked: ");
        return -1;
    }

    char temp[BUFFER_SIZE];
    for (int i = 0; i < BLOCK_COUNT; i++) {
        if (shm_a->status[i] == BLOCK_WROTE) {
            strcpy(temp, shm_a->buffer_arr[i]);
            uppercase_string(temp, strlen(temp));
            strcpy(shm_b->buffer_arr[i], temp);
            shm_a->status[i] = BLOCK_WAITING;
            printf("server: block %d processed\n", i);
            blocks_processed++;
        }
    }

    sem_buf.sem_op = 1; /* Releasing the resource */
    retval = semop(semid, &sem_buf, 1);

    if (retval == -1) {
        perror("Semaphore Locked\n");
        return -1;
    }

    return blocks_processed;

}

int check_clients(struct a_seg *shm_a) {
    /**
     * checks if all clients have read their response
     * if true:
     * returns 1
     * else
     * returns 0
     * in case of errors returns -1
     */
    int semid, result = 1;
    struct sembuf sem_buf;
    struct semid_ds buf;
    int retval;
    semid = semget(SEM_KEY, 1, 0);
    if (semid < 0) {
        perror("check clients: Semaphore GET: FAILED");
        return -1;
    }
//    printf("errno is %d and semid is %d\n", errno, semid);

    sem_buf.sem_num = 0;
    sem_buf.sem_op = -1; /* Allocating the resources */
    sem_buf.sem_flg = SEM_UNDO;
    retval = semop(semid, &sem_buf, 1);

    if (retval == -1) {
        perror("Semaphore Locked: ");
        return -1;
    }

    for (int i = 0; i < BLOCK_COUNT; i++) {
        if (shm_a->status[i] != BLOCK_AVAILABLE) {
            result = 0;
            break;
        }
    }

    sem_buf.sem_op = 1; /* Releasing the resource */
    retval = semop(semid, &sem_buf, 1);

    if (retval == -1) {
        perror("Semaphore Locked\n");
        return -1;
    }
    return result;
}

int main() {
    int shm_a_id, shm_b_id, semid;
    struct a_seg *shm_a;
    struct b_seg *shm_b;
    printf("server: starting...\n");
    shm_a_id = shmget(SHM_A_KEY, sizeof(struct a_seg), 0644 | IPC_CREAT);
    if (shm_a_id == -1) {
        perror("cannot get Shared memory a");
        return 1;
    }

    shm_a = shmat(shm_a_id, NULL, 0);

    if (shm_a == (void *) -1) {
        perror("Shared memory a attach: FAILED");
        return 1;
    }
    shm_b_id = shmget(SHM_B_KEY, sizeof(struct b_seg), 0644 | IPC_CREAT);
    if (shm_b_id == -1) {
        perror("cannot get Shared memory a");
        return 1;
    }

    shm_b = shmat(shm_b_id, NULL, 0);

    if (shm_b == (void *) -1) {
        perror("Shared memory a attach: FAILED");
        return 1;
    }

    initialize(shm_a);

    printf("server_ready:%d\n", shm_a->server_ready);
    printf("BLOCK STATUS:");
    for (int i = 0; i < BLOCK_COUNT; i++) {
        printf("%d ", shm_a->status[i]);
    }
    printf("\n");

    printf("server: cycle started\n");
    char temp[BUFFER_SIZE];
    int tries = 0, retval;
    while (tries < MAX_TRIES) {
        sleep(1);
//        printf("BLOCK STATUS:");
//        for (int i = 0; i < BLOCK_COUNT; i++) {
//            printf("%d ", shm_a->status[i]);
//        }
//        printf("\n");
        retval = process_blocks(shm_a, shm_b);
        if (retval == -1)
            break;
        if (retval == 0) { //server did nothing in this cycle
            tries++;
//            printf("server did nothing in this cycle\n");
        } else {
            tries = 0;
        }
    }
    if (tries == MAX_TRIES) {
        printf("server did nothing %d times in a row.shutting down...\n", tries);
        //wait till all clients read their message
        tries = 0;
        while (check_clients(shm_a) != 1 && tries < MAX_TRIES) {
            printf("some clients still didn't read their messages\n");
            sleep(1);
            tries++;
        }
    }

    //detaching a and b
    if (shmdt(shm_a) == -1) {
        perror("cannot detach a");
        return 1;
    }
    if (shmdt(shm_b) == -1) {
        perror("cannot detach b");
        return 1;
    }


    //deleting shared memory a and b
    if (shmctl(shm_a_id, IPC_RMID, 0) == -1) {
        perror("cannot delete shared memory a");
        return 1;
    }
    if (shmctl(shm_b_id, IPC_RMID, 0) == -1) {
        perror("cannot delete shared memory b");
        return 1;
    }

    //destroying semaphore
    remove_semaphore();
}

