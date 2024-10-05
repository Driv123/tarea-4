#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_SOLDADOS 1000
#define NUM_MOSTRADORES_COMIDA 5
#define NUM_MOSTRADORES_POSTRE 5
#define NUM_ABRIDORES 60

// Semaforos
sem_t sem_mostradores_comida;  // Controla los mostradores de comida (5 disponibles)
sem_t sem_mostradores_postre;  // Controla los mostradores de postre (5 disponibles)
sem_t sem_abridores;           // Controla los abridores (60 disponibles)

// Funcion para simular un soldado
void* soldado(void* arg) {
    int id = ((int)arg);

    // Soldado entra al comedor
    printf("Soldado %d: entra al comedor\n", id);

    // Soldado va a un mostrador de comida
    sem_wait(&sem_mostradores_comida);  // Espera a que haya un mostrador de comida libre
    printf("Soldado %d: en un mostrador de comida\n", id);

    // Simulamos el tiempo en el mostrador de comida
    sleep(1);

    // Decide si quiere refresco o agua (50% de probabilidad de refresco)
    int quiere_refresco = rand() % 2;
    if (quiere_refresco) {
        sem_wait(&sem_abridores);  // Si quiere refresco, agarra un abridor
        printf("Soldado %d: tomo un refresco y esta usando un abridor\n", id);
        sleep(1);  // Simulamos el tiempo usando el abridor
        sem_post(&sem_abridores);  // Devuelve el abridor
    } else {
        printf("Soldado %d: tomo agua\n", id);
    }

    sem_post(&sem_mostradores_comida);  // Libera el mostrador de comida

    // Decide si quiere postre (50% de probabilidad)
    int quiere_postre = rand() % 2;
    if (quiere_postre) {
        sem_wait(&sem_mostradores_postre);  // Espera a que haya un mostrador de postre libre
        printf("Soldado %d: en un mostrador de postre\n", id);
        sleep(1);  // Simulamos el tiempo en el mostrador de postre
        sem_post(&sem_mostradores_postre);  // Libera el mostrador de postre
    } else {
        printf("Soldado %d: no quiere postre\n", id);
    }

    // Soldado sale del comedor
    printf("Soldado %d: sale del comedor\n", id);

    pthread_exit(NULL);
}

int main() {
    pthread_t soldados[NUM_SOLDADOS];
    int id_soldados[NUM_SOLDADOS];

    // Inicializacion de los semaforos
    sem_init(&sem_mostradores_comida, 0, NUM_MOSTRADORES_COMIDA);  // 5 mostradores de comida
    sem_init(&sem_mostradores_postre, 0, NUM_MOSTRADORES_POSTRE);  // 5 mostradores de postre
    sem_init(&sem_abridores, 0, NUM_ABRIDORES);                    // 60 abridores disponibles

    // Crear los hilos de los soldados
    for (int i = 0; i < NUM_SOLDADOS; i++) {
        id_soldados[i] = i + 1;
        pthread_create(&soldados[i], NULL, soldado, (void*)&id_soldados[i]);
    }

    // Unir los hilos de los soldados
    for (int i = 0; i < NUM_SOLDADOS; i++) {
        pthread_join(soldados[i], NULL);
    }

    // Destruir semaforos
    sem_destroy(&sem_mostradores_comida);
    sem_destroy(&sem_mostradores_postre);
    sem_destroy(&sem_abridores);

    return 0;
}

