
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string>
#include <sys/shm.h>
#include "../logger/simpleLogger.h"


#define BackingFile "/my_shared_memory_file"  // Define the shared memory file name
#define AccessPerms 0644                      // Define the access permissions
#define ByteSize 1024                         // Define the size of the shared memory segment
#define SemaphoreName "/my_semaphore"          // Define the name of the semaphore
#define MemContents "Hello, Shared Memory!"   // Define the content to write to shared memory

void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1);
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <-r or -l>" << std::endl;
        return -1;
    }

    int value;
    if (strcmp(argv[1], "-r") == 0) {
        value = 0;
    } else if (strcmp(argv[1], "-l") == 0) {
        value = 1;
    } else {
        std::cerr << "Invalid argument. Please provide -r or -l." << std::endl;
        return -1;
    }

    const char* pipeName = "../fifoChannel";
    mkfifo(pipeName, 0666); /* read/write for user/group/others */
    int fd = open(pipeName, O_CREAT | O_WRONLY); /* open as write-only */
    if (fd < 0) return -1; /* can't go on */

    write(fd, &value, sizeof(int));
    close(fd); /* close pipe */
    unlink(pipeName); /* unlink from the implementing file */
    std::cout << "Value " << value << " sent to the pipe." << std::endl;

    sleep(3);

    /* shared memory */
    fd = shm_open(BackingFile, O_RDWR, AccessPerms);  /* empty to begin */
    if (fd < 0) report_and_exit("Can't get file descriptor...");

    /* get a pointer to memory */
    void* memptr = mmap(NULL,       /* let system pick where to put segment */
                        ByteSize,   /* how many bytes */
                        PROT_READ | PROT_WRITE, /* access protections */
                        MAP_SHARED, /* mapping visible to other processes */
                        fd,         /* file descriptor */
                        0);         /* offset: start at 1st byte */
    if ((void*) -1 == memptr) report_and_exit("Can't access segment...");

    /* create a semaphore for mutual exclusion */
    sem_t* semptr = sem_open(SemaphoreName, /* name */
                            O_CREAT,       /* create the semaphore */
                            AccessPerms,   /* protection perms */
                            0);            /* initial value */
    if (semptr == (void*) -1) report_and_exit("sem_open");

    /* use semaphore as a mutex (lock) by waiting for writer to increment it */
    if (!sem_wait(semptr)) { /* wait until semaphore != 0 */
        for (int i = 0; i < strlen(MemContents); i++)
            write(STDOUT_FILENO, static_cast<char*>(memptr) + i, 1); /* one byte at a time */
        sem_post(semptr);
    }

    /* cleanup */
    munmap(memptr, ByteSize);
    close(fd);
    sem_close(semptr);
    unlink(BackingFile);
    return 0;
}
