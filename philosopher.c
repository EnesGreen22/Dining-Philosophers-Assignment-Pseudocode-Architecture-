// PHILOSOPHER PROCESS
// Gets its id from args and loops: think -> take forks -> eat -> put forks

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>

// helper functions for simulation
void think() {
    usleep(50000);
}

void eat() {
    usleep(50000);
}

void main(int argc, char *argv[]) {
    int id = atoi(argv[1]); // my id (0 to 4)
    int left_fork = id;
    int right_fork = (id + 1) % 5;

    // 1. open the shared memory
    int shm_fd = shm_open("/my_shm", O_RDWR, 0666);
    int *table = mmap(NULL, 5 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // 2. open the fork semaphores
    sem_t *fork_sem[5];
    char sem_names[5][20] = {"/fork_0", "/fork_1", "/fork_2", "/fork_3", "/fork_4"};
    for (int i = 0; i < 5; i++) {
        fork_sem[i] = sem_open(sem_names[i], 0);
    }

    // 3. main loop
    int running = 10;
    while (running > 0) {
        think();

        // take forks: philosopher 4 takes right first to prevent deadlock
        if (id == 4) {
            sem_wait(fork_sem[right_fork]); // grab right fork first
            sem_wait(fork_sem[left_fork]);  // grab left fork
        } else {
            sem_wait(fork_sem[left_fork]);  // grab left fork first
            sem_wait(fork_sem[right_fork]); // grab right fork
        }

        eat();

        // put forks back on the table
        sem_post(fork_sem[left_fork]);
        sem_post(fork_sem[right_fork]);

        running--;
    }

    // 4. close connections and leave
    munmap(table, 5 * sizeof(int));
    close(shm_fd);
    for (int i = 0; i < 5; i++) {
        sem_close(fork_sem[i]);
    }

    exit(0);
}
