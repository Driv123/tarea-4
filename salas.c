#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>

#define SHMKEY 75
#define TRUE 1
#define MAX_TURNOS 5  // Definimos un máximo de turnos

int region_critica(int pid);
int region_no_critica(int pid);
int esperando_turno(int pid);

int main() {
    int shmid, pid, turnos = 0;  // Agregamos un contador de turnos
    char *addr, *turno;

    pid = getpid();
    shmid = shmget(SHMKEY, 10, 0777 | IPC_CREAT);
    
    if (shmid == -1) {
        perror("Error al crear el segmento de memoria compartida");
        exit(1);
    }

    addr = shmat(shmid, 0, 0);
    if (addr == (char *)(-1)) {
        perror("Error al asociar el segmento de memoria compartida");
        exit(1);
    }

    turno = (char *) addr;

    // Inicializar la variable de turno
    if (pid == 1) {
        *turno = 1;  // Solo el proceso 1 inicializa el turno
    } else {
        *turno = 2;  // El proceso 2 espera su turno
    }

    while (turnos < MAX_TURNOS) {  // Cambiamos la condición del bucle
        if (*turno == pid) {
            region_critica(pid);
            *turno = (*turno == 1) ? 2 : 1;  // Alternar el turno entre procesos
            region_no_critica(pid);
            esperando_turno(pid);
            turnos++;  // Incrementamos el contador de turnos
        } else {
            esperando_turno(pid);  // Espera si no es su turno
            sleep(1);  // Agrega un pequeño retraso para evitar un bucle de espera ocupada
        }
    }

    printf("\n[0] Proceso %d ha terminado después de %d turnos.\n", pid, MAX_TURNOS);

    if (shmdt(addr) == -1) {
        perror("Error al desasociar el segmento de memoria compartida");
        exit(1);
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("Error al eliminar el segmento de memoria compartida");
        exit(1);
    }

    return 0;
}

int esperando_turno(int pid) {
    printf("\n [|] Proceso %d esperando turno...", pid);
    sleep(2);
    return 0;
}

int region_critica(int pid) {
    printf("\n\n [0] Proceso %d en región crítica, cambiando variables...", pid);
    for (int i = 0; i < 5; i++) {
        printf("\n [0] Turno %d", i + 1);
        sleep(1);
    }
    return 0;
}

int region_no_critica(int pid) {
    printf("\n [0] Proceso %d ha terminado su turno.", pid);
    return 0;
}
