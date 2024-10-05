//Este codigo en principio no tiene ningun fallo/error que solucionar, el analisis se anexa en el documento word.


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex;
int saldo =0;

void* realizarRetiro(void* arg){
  int monto= *(int*)arg;
  pthread_mutex_lock(&mutex);
  printf("Realizando retiro de %d/n",monto);
  saldo -= monto;
  printf("saldo restate: %d/n", saldo);
  pthread_mutex_unlock(&mutex);
  pthread_exit(NULL);
  
}

void* realizarDeposito(void* arg){
    int monto = *(int*)arg;
    pthread_mutex_lock(&mutex);
    printf("realizando deposito de %d/n",monto);
    saldo += monto;
    printf("saldo actual %d/n",saldo);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main(){
    pthread_t hiloRetiro;
    pthread_t hiloDeposito;
    int montoRetiro=100;
    int montoDeposito=200;
    pthread_mutex_init (&mutex,NULL);
    pthread_create (&hiloRetiro,NULL,realizarRetiro,&montoRetiro);
    pthread_create (&hiloDeposito,NULL,realizarDeposito,&montoDeposito);
    pthread_join (hiloRetiro,NULL);
    pthread_join (hiloDeposito,NULL);
    pthread_mutex_destroy (&mutex);
    return 0;
    
}
