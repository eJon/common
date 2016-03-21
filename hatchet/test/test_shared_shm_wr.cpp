
#include <iostream>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <errno.h>
#include <time.h>

int main() 
{
    key_t key = 999955;
    size_t buf_size = (1<<20);
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

    time_t start, end;
    time(&start);
    for (size_t i=0; i<buf_size; i++) {
        shm_addr[i] = i%256;
        memcpy(buf, shm_addr, buf_size);
    }

    time(&end);

    double total_time = difftime(end, start);
    std::cout << "Time spent is: " << total_time << std::endl;
    std::cout << "Time spend per Byte per second: " << (double)buf_size/total_time*buf_size << std::endl;
    
    return 0;
}
