#include "monitor_buffer.h"
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int executar = 1;
int frame_id = 1;

void *produtor(void *arg) {
    while (executar) {
        Frame f = { .id = frame_id++ };
        produzir(f);
        usleep(100000); // 100ms
    }
    return NULL;
}

void *consumidor(void *arg) {
    char *nome = (char *)arg;
    while (executar) {
        consumir(nome);
        usleep(1000000); // 1s
    }
    return NULL;
}

int main() {
    inicializar_monitor();

    pthread_t t_prod, t_cons1, t_cons2;
    pthread_create(&t_prod, NULL, produtor, NULL);
    pthread_create(&t_cons1, NULL, consumidor, "Compressão");
    pthread_create(&t_cons2, NULL, consumidor, "Gravação");

    pthread_join(t_prod, NULL);
    pthread_join(t_cons1, NULL);
    pthread_join(t_cons2, NULL);

    destruir_monitor();
    return 0;
}
