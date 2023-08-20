/** Compilation: g++ -o fh fh.c -lrt -lpthread **/
#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstring>
#include "../logger/simpleLogger.h"


#define BackingFile "/my_shared_memory_file"
#define AccessPerms 0644
#define ByteSize 1024
#define SemaphoreName "/my_semaphore"

void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1);
}

int main() {
    int counter = 24 ; 
    LOG_TRACE << "step into main function"  << counter ;
    const char* NamePipe = "../fifoChannel";
    int fd = open(NamePipe, O_RDONLY);
    if (fd < 0){

        LOG_ERROR <<"FAiled in file descriptor ";
        return -1; /* no point in continuing */
    }
    int value;
    ssize_t count = read(fd, &value, sizeof(int));
    if (count != sizeof(int)) {
        std::cerr << "Error reading from the pipe." << std::endl;
        LOG_ERROR <<"Error reading from the pipe ";
        close(fd);
        return -1;
    }
    
    close(fd); /* close pipe from read end */
    unlink(NamePipe); /* unlink from the underlying file */

    /*shared memory*/
    fd = shm_open(BackingFile, O_RDWR | O_CREAT, AccessPerms);
    
    if (fd < 0) { 
        // Checking if shared memory opened 
    report_and_exit("Can't open shared mem segment...");
    LOG_ERROR <<"Can't open shared mem segment...";}
    ftruncate(fd, ByteSize);

    void* memptr = mmap(NULL, ByteSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if ((void*) -1 == memptr) report_and_exit("Can't get segment...");
    LOG_TRACE << " Made a shared memory location ";
    std::cerr << "shared mem address: " << memptr << " [0.." << ByteSize - 1 << "]" << std::endl;
    std::cerr << "backing file:       /dev/shm" << BackingFile << std::endl;
	
    sem_t* semptr = sem_open(SemaphoreName, O_CREAT, AccessPerms, 0);
    if (semptr == (void*) -1) report_and_exit("sem_open");

if (value == 0) {
    // Open the file for reading
    LOG_TRACE <<"READING FROM THE FILE  ";
    FILE* file = fopen("../file.txt", "r");
    if (file == nullptr) {
        report_and_exit("fopen");
    }

    // Read the content of the file
    size_t read_size = fread(memptr, 1, ByteSize, file);
    if (read_size > 0) {
        // Null-terminate the content if needed
        if (read_size < ByteSize) {
            static_cast<char*>(memptr)[read_size] = '\0';
        }
    }

    sleep(12);
    // Close the file and post the semaphore
    fclose(file);
    sem_post(semptr);
}
     else if (value == 1) {
        // Execute "ls" command and capture its output
        FILE* ls_output = popen("ls ../","r");
        LOG_TRACE<<"Listed the file  in the directory ";
        if (ls_output == nullptr) report_and_exit("popen");

        // Read the output and write to shared memory
        char buffer[ByteSize];
        size_t read_size = fread(buffer, sizeof(char), ByteSize, ls_output);
        if (read_size > 0) {
            strncpy(static_cast<char*>(memptr), buffer, ByteSize);
        }

        sleep(12);

        // Close the popen stream and the semaphore
        pclose(ls_output);
        LOG_TRACE<<"Close the popen stream and the semaphore";
        sem_post(semptr);
    }

    // Cleanup
    munmap(memptr, ByteSize);
    close(fd);
    sem_close(semptr);
    shm_unlink(BackingFile);
    LOG_INFO<<"ENded the log file ";
    return 0;
}
