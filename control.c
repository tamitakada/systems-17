#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>

#define KEY 150
#define SEM_KEY 200

/*
union semun {
  int val;                  //used for SETVAL
  struct semid_ds *buf;     //used for IPC_STAT and IPC_SET
  unsigned short  *array;   //used for SETALL
  struct seminfo  *__buf;
};
*/

int main(int argc, char **argv) {
    if (argc > 1) {
        int shmd;
        shmd = shmget(KEY, sizeof(int), IPC_CREAT | 0640);
        
        int semd;
        semd = semget(SEM_KEY, 1, IPC_CREAT | 0644);
        
        if (strcmp(argv[1], "-create") == 0) {
            char * message = argv[2];
            printf("m: %s\n", message);
            
            int fd = open("message", O_CREAT | O_WRONLY | O_TRUNC, 0644);
            int size = 0;
            char *c = argv[2];
            while (*c) {
                write(fd, c, sizeof(char));
                size++;
                c++;
            }
            close(fd);

            int *data;
            data = shmat(shmd, 0, 0);
            *data = size;
            shmdt(data);

            union semun us;
            us.val = 1;
            semctl(semd, 0, SETVAL, us);
        } else if (strcmp(argv[1], "-remove") == 0) {
            int fd = open("message", O_RDONLY, 0);
            
            char * c;
            int bytes = read(fd, c, sizeof(char));
            
            while (bytes > 0) {
                printf("%c", *c);
                bytes = read(fd, c, sizeof(char));
            }
            printf("\n");
            
            shmctl(shmd, IPC_RMID, 0);
            semctl(semd, IPC_RMID, 0);
        }
    }
}

