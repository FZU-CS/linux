#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FLAGS O_WRONLY | O_CREAT | O_TRUNC

int main() {
    char buf1[] = {"Hello "};   
    char buf2[] = {"World!"};  
    int fd = 0, count = 0;
    const char *path = "test";    

    if ((fd = open(path, FLAGS, 0600)) == -1) {
        printf("Open file failed!\n");
        exit(1);
     }
    count = strlen(buf1);               
    if (write(fd, buf1, count) != count) {
       printf("Write file failed!\n");
       exit(1);
    }

    system("cat test");
    printf("\n");

    if (lseek(fd, 10, SEEK_SET) == -1) {
       printf("lseek failed!\n");
       exit(1);
    }
    count = strlen(buf2);
             
    if (write(fd,buf2,count) != count) {
       printf("write file failed!\n");
       exit(1);  
    }

    system("cat test");
    printf("\n");
    
    return 0;
}