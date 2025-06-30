#include "monitor_buffer.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFER_SIZE 5

static Frame buffer[BUFFER_SIZE];
static int in = 0, out = 0, count = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_cheio = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond_vazio = PTHREAD_COND_INITIALIZER;

void inicializar_monitor() {
    in = out = count = 0;
}

void produzir(Frame f) {
    pthread_mutex_lock(&mutex);

    while (count == BUFFER_SIZE) {
        pthread_cond_wait(&cond_vazio, &mutex);
    }

    buffer[in] = f;
    in = (in + 1) % BUFFER_SIZE;
    count++;

    printf("📸 [Detector] Frame %d produzido.\n", f.id);
    imprimir_estado();

    pthread_cond_signal(&cond_cheio);
    pthread_mutex_unlock(&mutex);
}

Frame consumir(const char *nome) {
    pthread_mutex_lock(&mutex);

    while (count == 0) {
        pthread_cond_wait(&cond_cheio, &mutex);
    }

    Frame f = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    count--;

    printf("🛠️ [%s] Frame %d consumido.\n", nome, f.id);
    imprimir_estado();

    pthread_cond_signal(&cond_vazio);
    pthread_mutex_unlock(&mutex);

    return f;
}

void imprimir_estado() {
    printf("📦 Buffer: [");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (i < count)
            printf(" %-4d ", buffer[i].id);
        else
            printf(" __   ");
    }
    printf("] (count = %d)\n", count);
}
