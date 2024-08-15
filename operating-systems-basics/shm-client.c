/* Filename: shm_read_cntr.c */
#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<string.h>
#include<sys/sem.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/time.h>

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

int send(char *message, struct a_seg *shm_a) {
    /*
     * returns in which block it wrote the message
     * in case of failure it returns -1
     */
    int semid, block = -1;
    struct sembuf sem_buf;
    struct semid_ds buf;
    int retval;
    semid = semget(SEM_KEY, 1, 0);
    if (semid < 0) {
        perror("client:send message: Semaphore GET: FAILED");
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
        if (shm_a->status[i] == BLOCK_AVAILABLE) {
            shm_a->status[i] = BLOCK_WROTE;
            strcpy(shm_a->buffer_arr[i], message);
            block = i;
            break;
        }
    }
//    sleep(1);

    sem_buf.sem_op = 1; /* Releasing the resource */
    retval = semop(semid, &sem_buf, 1);

    if (retval == -1) {
        perror("Semaphore Locked\n");
        return -1;
    }
    return block;
}

int receive_message(char *message, struct a_seg *shm_a, struct b_seg *shm_b, int block) {
    /**
     * returns -1 in case of failure
     * otherwise returns 0
     */
    while (shm_a->status[block] != BLOCK_WAITING) {
        printf("waiting for server to respond\n");
        sleep(1);
    }
    /*
    * returns in which block it wrote the message
    * in case of failure it returns -1
    */
    int semid;
    struct sembuf sem_buf;
    struct semid_ds buf;
    int retval;
    semid = semget(SEM_KEY, 1, 0);
    if (semid < 0) {
        perror("client:receive_message: Semaphore GET: FAILED");
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

    //copy message from b to buffer
    strcpy(message, shm_b->buffer_arr[block]);

    //make block available in a
    shm_a->status[block] = BLOCK_AVAILABLE;

    sem_buf.sem_op = 1; /* Releasing the resource */
    retval = semop(semid, &sem_buf, 1);

    if (retval == -1) {
        perror("Semaphore Locked\n");
        return -1;
    }
    return 0;
}


int main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE];
    int shm_a_id, shm_b_id;
    struct a_seg *shm_a;
    struct b_seg *shm_b;
    if (argc != 2)
        strcpy(buffer, "default message");
    else {
        strcpy(buffer, argv[1]);

    }
    shm_a_id = shmget(SHM_A_KEY, sizeof(struct a_seg), 0644 | IPC_CREAT);


    if (shm_a_id == -1) {
        perror("Shared memory a:FAILED");
        return 1;
    }
    // Attach to the segment to get a pointer to it.
    shm_a = shmat(shm_a_id, NULL, 0);

    if (shm_a == (void *) -1) {
        perror("Shared memory a attach FAILED");
        return 1;
    }
    shm_b_id = shmget(SHM_B_KEY, sizeof(struct b_seg), 0644 | IPC_CREAT);

    if (shm_b_id == -1) {
        perror("Shared memory b:FAILED");
        return 1;
    }
    // Attach to the segment to get a pointer to it.
    shm_b = shmat(shm_b_id, NULL, 0);

    if (shm_b == (void *) -1) {
        perror("Shared memory b attach:FAILED");
        return 1;
    }


    while (shm_a->server_ready != 1) {
        printf("server is not ready yet\n");
        sleep(1);
    }
//    printf("server is now ready\n");

    struct timeval start, end;

    gettimeofday(&start, NULL);
    int block = send(buffer, shm_a);
    if (block == -1) {
        perror("error in sending message");
    } else {
//        printf("block number is %d\n", block);
//        printf("BLOCK STATUS:");
//        for (int i = 0; i < BLOCK_COUNT; i++) {
//            printf("%d ", shm_a->status[i]);
//        }
        if (receive_message(buffer, shm_a, shm_b, block) == 0) {
            gettimeofday(&end, NULL);
            printf("recieved message: %s\n", buffer);
            printf("client: took %f miliseconds\n",
                   (end.tv_sec - start.tv_sec) * 1000.0f + (end.tv_usec - start.tv_usec) / 1000.0f);
//            printf("BLOCK STATUS:");
//            for (int i = 0; i < BLOCK_COUNT; i++) {
//                printf("%d ", shm_a->status[i]);
//            }
        } else {
            perror("error in receiving message");
        }
    }

    //detach shared memory
    if (shmdt(shm_a) == -1) {
        perror("shared memory a detach:FAILED");
        return 1;
    }
    if (shmdt(shm_b) == -1) {
        perror("shared memory b detach:FAILED");
        return 1;
    }
    return 0;

}
