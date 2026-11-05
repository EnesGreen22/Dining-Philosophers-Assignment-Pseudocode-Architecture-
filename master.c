// MASTER PROCESS
// Creates shared memory and semaphores, then starts 5 philosophers

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>

void main() {
    // 1. make shared memory for the table
    int shm_fd = shm_open("/my_shm", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, 5 * sizeof(int));
    int *table = mmap(NULL, 5 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // 2. create 5 semaphores for 5 forks (value 1 means fork is free)
    sem_t *fork_sem[5];
    char sem_names[5][20] = {"/fork_0", "/fork_1", "/fork_2", "/fork_3", "/fork_4"};
    for (int i = 0; i < 5; i++) {
        fork_sem[i] = sem_open(sem_names[i], O_CREAT, 0666, 1);
    }

    // 3. start 5 philosopher processes
    for (int i = 0; i < 5; i++) {
        int pid = fork();
        if (pid == 0) {
            // child process runs philosopher with its own id
            char id_str[4];
            sprintf(id_str, "%d", i);
            char *args[] = {"./philosopher", id_str, NULL};
            execvp(args[0], args);
        }
    }

    // 4. wait for all children to finish
    for (int i = 0; i < 5; i++) {
        wait(NULL);
    }

    // 5. clean up memory and semaphores
    munmap(table, 5 * sizeof(int));
    close(shm_fd);
    shm_unlink("/my_shm");

    for (int i = 0; i < 5; i++) {
        sem_close(fork_sem[i]);
        sem_unlink(sem_names[i]);
    }

    exit(0);
}
