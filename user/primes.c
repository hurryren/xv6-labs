#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int primes(int *fd)
{
    int child_fd[2];
    int prime;
    int temp;
    int child_flag=0;

    if(fork()==0){
        close(fd[1]);
        read(fd[0],&prime,sizeof(int));
        printf("prime %d\n",prime);

        if(pipe(child_fd) <0){
            printf("pipe error\n");
            exit(0);
        }

        while(read(fd[0],&temp,sizeof(int)) > 0){

            if(temp % prime != 0){
                if(child_flag == 0){
                    child_flag = 1;
                    primes(child_fd);
                }

                // printf("temp=[%d]\n", temp);
                write(child_fd[1],&temp,sizeof(int));
            }

            if(temp == 35){
                close(child_fd[1]);
                wait(0);
            }
            // else{
            //     printf("pass data = [%d]\n",temp);
            // }
        }
        close(fd[0]);
        exit(0);
    }else{
        close(fd[0]);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int fd[2];

    if(pipe(fd) <0){
        printf("pipe error\n");
        exit(0);
    }
    for(int i=2;i<36;i++){
        write(fd[1],&i,sizeof(int));
    }

    primes(fd);
    close(fd[1]);

    wait(0);

    exit(0);
}