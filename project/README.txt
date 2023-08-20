this steps to produce the cmake files of the program :
cmake . 
make 
to ensure the communication in the project we should exceute the requester first to open the named pipe to ipc with file_handler 
to open the requester app ./request/request
to open the file_handler ./file_handler/fh

Description of the project some of the libraries is implemented by examples used in the session like the simple logger example 
the requester app makes an named pipe named fifo channel to ipc with file_handler app the request app requiers an argument to start the app  
either 1 - -l to list the files in the project directory 
       2 - -r to read from a file.txt created by the user of this project to read from it
the requester also reads from a shared memory that is required to be opened to both threads share the files list and the file read by the fh (file-handler app )

The File Handelr App  : 
it receives data from the requester to either get the data from a specific file in our case it file.txt or list files in the directory 


to ensure the communication between two threads there synchronization using semaphores 
 
There is a failed attempts to apply the gtest in the project 
Gtest will be implemented in the next version 