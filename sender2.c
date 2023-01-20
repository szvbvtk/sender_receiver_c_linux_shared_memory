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
        return 1;
    }

    int shmid = shmget(key, BUFSIZE, 0666 | IPC_CREAT | IPC_EXCL);

    if(shmid == -1){
        fprintf(stderr, "Nie udało się przydzielić bloku pamięci współdzielonej\n");
        return 1;
    }

    char *memseg = shmat(shmid, NULL, 0);

    if(memseg == (char *) -1){
        fprintf(stderr, "Nie udało się dołączyć bloku pamięci współdzielonej\n");
        return 1;
    }

    // stworzenie i dolaczenie bloku pamieci
    char buf[BUFSIZE];
    char end_of_transfer[] = "$!KoniecPrzesylu!$";
    char start_of_transfer[] = "$!StartPrzesylu!$";
    char wait[] = "$!Spinlock!$";

    FILE *fp = fopen(argv[2], "r");
    int bytes_read;
    
    printf("Oczekiwanie na odbiorcę\n");
    while(strcmp(memseg, start_of_transfer) != 0){};
    printf("Rozpoczęcie przesyłu\n");

    int i = 1;
    while ((bytes_read = fread(memseg, 1, BUFSIZE, fp)) > 0) {
        fprintf(stdout, "%d. Przesłano %d bajtów...\n", i, bytes_read );
        i += 1;
        while (strcmp(memseg, wait) != 0) {}  
    }
    strcpy(memseg, end_of_transfer);
    printf("Plik został przesłany\n");
    fclose(fp);

    // odlaczenie i usuniecie bloku pamieci
    if(shmdt(memseg) == -1){
        fprintf(stderr, "Nie udało się odłączyć bloku pamięci współdzielonej\n");
        return 1;
    }

    if(shmctl (shmid, IPC_RMID, NULL) == -1) {
        fprintf(stderr, "Nie udało się zwolnić bloku pamięci współdzielonej\n");
        return 1;
    }

    return 0;
}
