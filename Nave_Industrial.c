#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define CAPACIDAD_CINTA 10 // Capacidad de la cinta

int cinta[CAPACIDAD_CINTA];  // Aqui van las piezas
int pos_cinta = 0;           // Donde estamos en la cinta

// Semaforos
sem_t sem_produccion;    // Controla cuantas piezas se pueden poner
sem_t sem_robot;         // Controla si el robot puede recoger algo
sem_t sem_control_calidad; // Controla el control de calidad (3 piezas listas)

// Mutex
pthread_mutex_t mutex_cinta = PTHREAD_MUTEX_INITIALIZER;

// Funcion que simula la maquina que inyecta las piezas
void* maquina_inyectora(void* arg) {
    while (1) {
        sem_wait(&sem_produccion); // Esperamos a que haya espacio en la cinta
        pthread_mutex_lock(&mutex_cinta); // Bloqueamos la cinta

        // Agregar pieza a la cinta
        cinta[pos_cinta] = 1; // Ponemos una pieza en la cinta
        printf("Maquina: pieza producida en la posicion %d\n", pos_cinta);
        pos_cinta = (pos_cinta + 1) % CAPACIDAD_CINTA; // Vamos a la siguiente posicion

        pthread_mutex_unlock(&mutex_cinta); // Desbloqueamos la cinta
        sem_post(&sem_robot); // Avisamos al robot que hay una pieza
        sleep(1); // Simulamos un tiempo de produccion
    }
}

// Funcion que simula el robot que recoge piezas y las empaqueta
void* robot_recoge(void* arg) {
    while (1) {
        sem_wait(&sem_robot); // Esperamos a que haya algo que recoger
        pthread_mutex_lock(&mutex_cinta); // Bloqueamos la cinta

        // Recoger pieza de la cinta
        int pieza_recogida = cinta[(pos_cinta - 1 + CAPACIDAD_CINTA) % CAPACIDAD_CINTA];
        cinta[(pos_cinta - 1 + CAPACIDAD_CINTA) % CAPACIDAD_CINTA] = 0; // Limpiamos la posicion
        printf("Robot: recogiendo pieza de la posicion %d\n", (pos_cinta - 1 + CAPACIDAD_CINTA) % CAPACIDAD_CINTA);

        pthread_mutex_unlock(&mutex_cinta); // Desbloqueamos la cinta
        sem_post(&sem_produccion); // Avisamos que hay espacio en la cinta

        sleep(2); // Simulamos el tiempo de empaquetado
    }
}

// Funcion para simular el control de calidad
void* operario_control_calidad(void* arg) {
    while (1) {
        sem_wait(&sem_control_calidad); // Esperamos a tener 3 piezas
        pthread_mutex_lock(&mutex_cinta); // Bloqueamos la cinta

        // Simulamos recoger 3 piezas para control de calidad
        int piezas_controladas = 0;
        for (int i = 0; i < CAPACIDAD_CINTA; i++) {
            if (cinta[i] == 1) {
                printf("Control de Calidad: recogiendo pieza de la posicion %d\n", i);
                cinta[i] = 0; // Limpiamos la posicion
                piezas_controladas++;
                if (piezas_controladas == 3) break; // Ya tenemos 3
            }
        }

        if (piezas_controladas == 3) {
            printf("Control de Calidad: 3 piezas listas para revision\n");
        } else {
            printf("Control de Calidad: No habia suficientes piezas, intentare mas tarde\n");
        }

        pthread_mutex_unlock(&mutex_cinta); // Desbloqueamos la cinta

        sleep(5); // Simulamos el tiempo de control de calidad
    }
}

int main() {
    // Inicializacion de los semaforos
    sem_init(&sem_produccion, 0, CAPACIDAD_CINTA);  // La cinta tiene capacidad para 10 piezas
    sem_init(&sem_robot, 0, 0);                    // No hay piezas inicialmente
    sem_init(&sem_control_calidad, 0, 3);           // Se necesita al menos 3 piezas para control de calidad

    // Crear los hilos
    pthread_t hilo_maquina, hilo_robot, hilo_control_calidad;
    pthread_create(&hilo_maquina, NULL, maquina_inyectora, NULL);
    pthread_create(&hilo_robot, NULL, robot_recoge, NULL);
    pthread_create(&hilo_control_calidad, NULL, operario_control_calidad, NULL);

    // Unir los hilos
    pthread_join(hilo_maquina, NULL);
    pthread_join(hilo_robot, NULL);
    pthread_join(hilo_control_calidad, NULL);

    // Destruir semaforos
    sem_destroy(&sem_produccion);
    sem_destroy(&sem_robot);
    sem_destroy(&sem_control_calidad);

    return 0;
}

