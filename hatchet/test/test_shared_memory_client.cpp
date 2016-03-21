
#include <iostream>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <errno.h>
#include <time.h>

int main() 
{
    key_t key = 999955;
    size_t buf_size = 33554400;
    int shm_id = shmget(key, buf_size, 0666);
    if (shm_id < 0) {
        std::cout << "Get shared memory failed." << std::endl;
        std::cout << "error is: " << strerror(errno) << std::endl;
        return 1;
    }
    std::cout << "Successfully get shared memory. ShmID is: " << shm_id << std::endl;

    char* shm_addr = (char *)shmat(shm_id, 0, 0); 
    if (shm_addr == 0) {
        std::cout << "Get address of shared memory failed." << std::endl;
        return 1;
    }

    char *buf = new char[buf_size];

    time_t b,e;
    time(&b);
    for (int i=0; i<50000; i++) {
        memcpy(buf, shm_addr, buf_size);
        if (buf[25] != 25) {
            std::cout << "Value is not right." << std::endl;
            break;
        }
    }

    time(&e);
    
    std::cout << "Time elapsed for copying: " << difftime(e,b) << std::endl;

    return 0;
}

