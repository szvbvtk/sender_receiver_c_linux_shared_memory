#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h> // do sleep potem usunac

#define BUFSIZE 1024

int main(int argc, char **argv){
        if(argc < 3){
        fprintf(stderr, "Nieprawidłowa ilość argumentów\n"); 
        return 1;
    }

    key_t key = ftok(argv[1], 'a');

    if(key == -1) {
        fprintf(stderr, "Nie udało się wygenerować klucza\n");
        return 2;
    }

    int shmid = shmget(key, BUFSIZE, 0666);

    if(shmid == -1){
        fprintf(stderr, "Nie udało się przydzielić bloku pamięci współdzielonej\n");
        return 3;
    }

    void *memseg = shmat(shmid, NULL, 0);

    if(memseg == (void *) -1){
        fprintf(stderr, "Nie udało się dołączyć bloku pamięci współdzieloenj\n");
        return 4;
    }

    FILE *fp = fopen(argv[2], "wb");

    int bytes_read;
    // while ((bytes_read = fwrite(memseg, 1, BUFSIZE, fp)) > 0) {
    //     printf("Odebrano %d bajtow\n", bytes_read);
    //         if (feof(fp)) {
    //         break;
    //     }
    // }

    // fprintf(stdout, "%s \n", (char *)memseg);
    // fwrite((char *)memseg, 1, sizeof(char) * strlen((char *)memseg), fp);



    fclose(fp);

    if(shmdt(memseg) == -1){
        fprintf(stderr, "Nie udało się odłączyć bloku pamięci współdzielonej\n");
        return 5;
    }

    return 0;
}