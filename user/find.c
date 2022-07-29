#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


void
find(char *path,char *target)
{
    char buf[64], *p;
    int fd;

    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    memcpy(buf, path,strlen(path));
    p = buf+strlen(path);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0 || strcmp(de.name,".") == 0 || strcmp(de.name,"..")==0)
            continue;


        memcpy(p,de.name,DIRSIZ);
        *(p+DIRSIZ) = 0;

        if(stat(buf, &st) < 0){
            fprintf(2, "find: cannot stat %s\n", path);
            close(fd);
            return;
        }

        if(st.type == T_FILE && strcmp(de.name, target) == 0){
            printf("%s\n",buf);
        }

        if(st.type == T_DIR){
            find(buf,target);
        }

    }

    close(fd);
}

int
main(int argc, char *argv[])
{

  if(argc != 3){
    printf("usage: find dir target\n");
    exit(1);
  }

  find(argv[1], argv[2]);

  exit(0);
}



// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"
// #include "kernel/fs.h"


// void
// find(char *path,char *target, char *target_path)
// {
//   char buf[64], *p;
//   char buf1[64],*p1;
//   int fd;

//   struct dirent de;
//   struct stat st;

//   if((fd = open(path, 0)) < 0){
//     fprintf(2, "find: cannot open %s\n", path);
//     return;
//   }

//   if(fstat(fd, &st) < 0){
//     fprintf(2, "find: cannot stat %s\n", path);
//     close(fd);
//     return;
//   }

//   switch(st.type){
//   case T_FILE:

//     if(read(fd, &de, sizeof(de)) == sizeof(de)){
//         printf("de.name=[%s]\n",de.name);
//     }
//     // printf("path=[%s], target=[%s], taget_path=[%s]\n",path, target, target_path);
//     if(strcmp(path,target_path) == 0){
//         printf("%s\n",path);
//     }

//     break;

//   case T_DIR:
//     if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
//       printf("ls: path too long\n");
//       break;
//     }
//     strcpy(buf, path);
//     p = buf+strlen(buf);
//     *p++ = '/';
//     strcpy(buf1, buf);
//     p1 = buf1+strlen(buf1);
//     while(read(fd, &de, sizeof(de)) == sizeof(de)){
//         if(de.inum == 0)
//             continue;

//         if(strcmp(de.name,".") == 0 || strcmp(de.name,"..")==0)
//             continue;
//         memmove(p, de.name, DIRSIZ);
//         p[DIRSIZ] = 0;
//         // printf("taget_path=[%s]\n", buf1);
//         memcpy(p1,target,strlen(target));
//         *(p1+strlen(target)) = 0;
//         // printf("taget_path=[%s]\n", buf1);
//         find(buf,target,buf1);
//     }
//     break;
//     }
//     close(fd);
// }

// int
// main(int argc, char *argv[])
// {

//   if(argc != 3){
//     printf("usage: find dir target\n");
//     exit(1);
//   }

//   find(argv[1], argv[2], argv[2]);

//   exit(0);
// }
