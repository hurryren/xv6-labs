#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char *argv[]){
    int pid;
    int fd[2];
    unsigned char buf=0x00;


    if(pipe(fd) < 0){
        fprintf(0,"pipe error\n");
        exit(1);
    }

    pid = fork();
    if(pid == 0){
        read(fd[0],&buf,1);
        printf("%d: received ping\n",getpid());
        close(fd[0]);
        write(fd[1],&buf,1);
        close(fd[1]);
    }else{
        write(fd[1],&buf,1);
        close(fd[1]);
        wait(0);
        read(fd[0],&buf,1);
        printf("%d: received pong\n",getpid());
        close(fd[0]);
    }
    exit(0);
}