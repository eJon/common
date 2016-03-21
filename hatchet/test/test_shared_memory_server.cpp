
#include <iostream>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <errno.h>
#include <time.h>

int main() 
{
    key_t key = 999955;
    size_t buf_size = 33554400;
    int shm_id = shmget(key, buf_size, IPC_CREAT | 0666);
    if (shm_id < 0) {
        std::cout << "Create shared memory failed." << std::endl;
        std::cout << "error is: " << strerror(errno) << std::endl;
        return 1;
    }
    std::cout << "Successfully create shared memory. ShmID is: " << shm_id << std::endl;

    char* shm_addr = (char *)shmat(shm_id, 0, 0); 
    if (shm_addr == 0) {
        std::cout << "Get address of shared memory failed." << std::endl;
        return 1;
    }

    char *buf = new char[buf_size];

    memset(buf, 25, buf_size);

    time_t b,e;
    time(&b);
    for (int i=0; i<50000; i++) {
        buf[i%buf_size] = (char)25;
        memcpy(shm_addr, buf, buf_size);
    }
    time(&e);
    
    std::cout << "Time elapsed for copying: " << difftime(e,b) << std::endl;
    shmctl(key, IPC_RMID, 0);

    return 0;
}

