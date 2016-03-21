
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>

#define NLOOPS       1000
#define SIZE         (1<<20)     /* size of shared memory area */

int main(int argc, char * argv[])
{
    int     fd;
    void    *area;

    if (argc > 1 && strcmp(argv[1], "-f") == 0) {
        if ( (fd = open("/home/zhangkailin01/tmp/mytmp", O_CREAT | O_RDWR | O_TRUNC | 0666)) < 0) {
            perror("open error");
            return 1;
        }
        if (lseek(fd, SIZE, SEEK_SET) == -1) {
            perror("lsee error");
            return 1;
        }
        if (write(fd, "", 1) != 1) {
            perror("write error");
            return 1;
        }
    }
    else if (argc > 1) {
        std::cout << "Usage: prog -f" << std::endl;
        std::cout << "       prog" << std::endl; 
        return 1;
    }
    else if ((fd = open("/dev/zero", O_RDWR)) < 0) {
      perror("open error");
      return 1;
    }
    if ((area = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
		     fd, 0)) == MAP_FAILED) {
      perror("mmap error");
      return 1;
    }

    close(fd);      /* can close /dev/zero now that it's mapped */
    
    char *malloced_buf = (char*)malloc(SIZE);
    if (!malloced_buf) {
      perror("malloc error");
      return 1;
    }

    std::cout << "SIZE is: " << SIZE << std::endl;
    time_t start;
    time(&start);
    unsigned char *my_buf = (unsigned char *)area;
    for (int i=0; i< SIZE; i++) {
      my_buf[i] = i%256;
      memcpy(malloced_buf, my_buf, SIZE);
    }
    time_t end;
    time(&end);

    double total_time = difftime(end, start);
    std::cout << "Time spent is: " << total_time << std::endl;
    std::cout << "Time spend per Byte per second: " << (double)SIZE/total_time*SIZE << std::endl;
}

