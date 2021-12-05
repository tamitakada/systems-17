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

int main() {
    int semd;
    semd = semget(SEM_KEY, 1, 0);
    
    int n = semctl(semd, 0, GETVAL);
    if (n > 0) {
        struct sembuf sb;
        sb.sem_num = 0;
        sb.sem_flg = SEM_UNDO;
        sb.sem_op = -1;

        semop(semd, &sb, 1);
        
        int shmd;
        shmd = shmget(KEY, sizeof(int), 0640);
        
        int *data;
        data = shmat(shmd, 0, 0);
        
        int fd = open("message", O_RDONLY, 0);
        
        lseek(fd, *data, SEEK_SET);
        
        printf("Last line added: ");
        char c;
        int size = 0;
        int bytes = read(fd, &c, sizeof(char));
        while (bytes > 0) {
            printf("%c", c);
            size++;
            bytes = read(fd, &c, sizeof(char));
        }
        *data = size + *data;
    
        char next_line[100];
        printf("\nAdd a new line:\t");
        fgets(next_line, sizeof(next_line), stdin);
        int new_size = strcspn(next_line, "\n");
        next_line[new_size] = 0;
        
        close(fd);
        fd = open("message", O_WRONLY | O_APPEND, 0);
        write(fd, next_line, new_size);
        close(fd);

        sb.sem_op = 1;
        semop(semd, &sb, 1);
    }
}
