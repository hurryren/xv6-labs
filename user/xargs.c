#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// void print_buf(char **data,int len){
//     for(int i=0; i < len; i++){
//         printf("%s\n",data[i]);
//     }
//     printf("\n");
// }



int main(int argc, char *argv[])
{

    char buf_[16][MAXARG];
    char *buf[16];
    for(int k=0;k<16;k++){
        buf[k] = buf_[k];
    }


    int i;
    int j=0;
    char temp_buf;
    // printf("xargs\n");

    for (i = 1; i < argc; i++){
        // printf("argv[%d]=%s\n",i,argv[i]);
        memcpy(buf[i-1],argv[i],MAXARG);
        // printf("buf[%d]=%s\n",i-1,buf[i-1]);
    }
    i--;
    // print_buf(buf, argc-1);
    // printf("i=%d\n", i);

    while(read(0,&temp_buf,sizeof(char) > 0)){
        if(temp_buf == '\n'){
            buf[i][j] = 0;
            i++;
            j=0;
        }else{
            buf[i][j++] = temp_buf;
        }
    }
    buf[i+1]=0;

    // print_buf(buf,i);
    // printf("i=%d\n", i);



    // printf("buf[0]=%s\n", buf[0]);
    // printf("buf[%d]=%s\n", i,buf[i]);

    if(fork()==0){
        exec(buf[0], buf);
    }else{
        wait(0);
    }

    exit(0);
}



// int main(int argc, char *argv[])
// {

//     char *buf[8];
//     buf[0]="echo";
//     buf[1]="hello";


//     if(fork()==0){
//         exec(buf[0],buf);
//     }else{
//         wait(0);
//     }



//     exit(0);
// }
