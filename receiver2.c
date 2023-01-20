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
        fprintf(stderr, "Nie udało się uzyskać dostępu do bloku pamięci współdzielonej\n");
        return 3;
    }

    char *memseg = shmat(shmid, NULL, 0);

    if(memseg == (char *) -1){
        fprintf(stderr, "Nie udało się dołączyć bloku pamięci współdzieloenj\n");
        return 4;
    }

    FILE *fp = fopen(argv[2], "wb");

    int bytes_read;
    char end_of_transfer[] = "$!KoniecPrzesylu!$";
    char wait[] = "$!Spinlock!$";
    char start_of_transfer[] = "$!StartPrzesylu!$";


    strcpy(memseg, start_of_transfer);
    while(strcmp(memseg, start_of_transfer) == 0){};

    int i = 1;
    while (strcmp(end_of_transfer, memseg) != 0){
        fwrite(memseg, 1, BUFSIZE, fp);
        printf("%d. odebrano\n", i);
        i += 1;
        strcpy(memseg, wait);
        while (strcmp(memseg, wait) == 0) {}  
    }

    fclose(fp);
    printf("Transfer pliku został zakończony\n");
    if(shmdt(memseg) == -1){
        fprintf(stderr, "Nie udało się odłączyć bloku pamięci współdzielonej\n");
        return 5;
    }

    return 0;
}