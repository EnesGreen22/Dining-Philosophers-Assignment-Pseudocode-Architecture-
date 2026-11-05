# Dining Philosophers Assignment (Pseudocode / Architecture)

## Architecture Overview
This solution implements the classic Dining Philosophers problem using POSIX Shared Memory and Semaphores, following the guidelines provided in class:
- **Master Process**: Creates the shared memory segment (`shm_open`, `ftruncate`, `mmap`) and 5 named semaphores initialized to 1 for the 5 forks (`sem_open`). It spawns 5 philosopher processes using `fork()` and `execvp()`. When execution finishes, it reaps children with `wait()` and cleans up all IPC objects (`munmap`, `close`, `shm_unlink`, `sem_close`, `sem_unlink`).
- **Philosopher Processes**: Each philosopher connects to the shared memory and the 5 fork semaphores.

## Deadlock Prevention
To break the circular wait condition:
- Philosophers 0, 1, 2, and 3 pick up their **left fork first**, then their **right fork**.
- Philosopher 4 (the asymmetric philosopher) picks up the **right fork first**, then the **left fork**.

This prevents the deadlock where every philosopher holds one fork and waits indefinitely for the second.

## Syscalls Used
- Process creation & execution: `fork()`, `execvp()`, `wait()`, `exit()`
- Shared Memory: `shm_open()`, `ftruncate()`, `mmap()`, `munmap()`, `close()`, `shm_unlink()`
- Semaphores: `sem_open()`, `sem_wait()`, `sem_post()`, `sem_close()`, `sem_unlink()`
