#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#define BUFFER_SIZE 5

typedef struct {
    int id;
} Frame;

Frame buffer[BUFFER_SIZE];
int count = 0;
int in = 0;
int out = 0;
int frame_id = 1;
int executar = 1; // flag para parar com CTRL+C

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Tratador de sinal para encerrar com CTRL+C
void parar_execucao(int sig) {
    executar = 0;
    printf("\n\n⛔ Encerrando a execução do sistema de vigilância...\n");
}

void imprimir_estado() {
    printf("📦 Buffer: [");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (i == in && i == out && count > 0)
            printf(" IO%-2d ", buffer[i].id);
        else if (i == in && count > 0)
            printf(" I%-3d ", buffer[i].id);
        else if (i == out && count > 0)
            printf(" O%-3d ", buffer[i].id);
        else if (i < count)
            printf(" %-4d ", buffer[i].id);
        else
            printf(" __   ");
    }
    printf("] (count = %d)\n", count);
}

// PRODUTOR: Detecção de movimento
void *produtor(void *arg) {
    while (executar) {
        Frame f = { .id = frame_id++ };

        pthread_mutex_lock(&mutex);
        if (count < BUFFER_SIZE) {
            buffer[in] = f;
            in = (in + 1) % BUFFER_SIZE;
            count++;

            printf("📸 [Detector] Frame %d produzido.\n", f.id);
            imprimir_estado();
        }
        pthread_mutex_unlock(&mutex);

        usleep(100000); // produz 1 frame a cada 100ms
    }

    return NULL;
}

// CONSUMIDOR: Compressão ou Gravação
void *consumidor(void *arg) {
    char *nome = (char *)arg;

    while (executar) {
        pthread_mutex_lock(&mutex);
        if (count > 0) {
            Frame f = buffer[out];
            out = (out + 1) % BUFFER_SIZE;
            count--;

            printf("🛠️ [%s] Frame %d consumido.\n", nome, f.id);
            imprimir_estado();

            pthread_mutex_unlock(&mutex);
            usleep(1000000); // 1 segundo para consumir
        } else {
            pthread_mutex_unlock(&mutex);
            usleep(10000); // espera ativa mais perceptível
        }
    }

    return NULL;
}


int main() {
    signal(SIGINT, parar_execucao); // permite parar com CTRL+C

    pthread_t t_prod, t_cons1, t_cons2;

    pthread_create(&t_prod, NULL, produtor, NULL);
    pthread_create(&t_cons1, NULL, consumidor, "Compressão");
    pthread_create(&t_cons2, NULL, consumidor, "Gravação");

    pthread_join(t_prod, NULL);
    pthread_join(t_cons1, NULL);
    pthread_join(t_cons2, NULL);

    return 0;
}
