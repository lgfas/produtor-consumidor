#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>

#define BUFFER_SIZE 5

typedef struct {
    int id;
} Frame;

Frame buffer[BUFFER_SIZE];
int in = 0, out = 0;
int frame_id = 1;
int executar = 1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_vazio;  // controla espaço disponível
sem_t sem_cheio;  // controla itens disponíveis

// Exibe o estado atual do buffer
void imprimir_estado() {
    printf("📦 Buffer: [");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (i == in && i == out)
            printf(" IO__ ");
        else if (i == in)
            printf(" I__  ");
        else if (i == out)
            printf(" O__  ");
        else
            printf(" __   ");
    }
    printf("]\n");
}

// PRODUTOR: Detecção de movimento
void *produtor(void *arg) {
    while (executar) {
        Frame f = { .id = frame_id++ };

        sem_wait(&sem_vazio);           // Espera por espaço no buffer
        pthread_mutex_lock(&mutex);

        buffer[in] = f;
        in = (in + 1) % BUFFER_SIZE;

        printf("📸 [Detector] Frame %d produzido.\n", f.id);
        imprimir_estado();

        pthread_mutex_unlock(&mutex);
        sem_post(&sem_cheio);           // Sinaliza que há item disponível

        usleep(100000); // 100ms entre produções
    }

    return NULL;
}

// CONSUMIDOR: Compressão ou Gravação
void *consumidor(void *arg) {
    char *nome = (char *)arg;

    while (executar) {
        sem_wait(&sem_cheio);           // Espera por item disponível
        pthread_mutex_lock(&mutex);

        Frame f = buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        printf("🛠️ [%s] Frame %d consumido.\n", nome, f.id);
        imprimir_estado();

        pthread_mutex_unlock(&mutex);
        sem_post(&sem_vazio);           // Sinaliza espaço livre

        usleep(1000000); // 1 segundo para processar frame
    }

    return NULL;
}

int main() {

    pthread_t t_prod, t_cons1, t_cons2;

    sem_init(&sem_vazio, 0, BUFFER_SIZE); // começa com buffer cheio de espaço
    sem_init(&sem_cheio, 0, 0);           // começa vazio

    pthread_create(&t_prod, NULL, produtor, NULL);
    pthread_create(&t_cons1, NULL, consumidor, "Compressão");
    pthread_create(&t_cons2, NULL, consumidor, "Gravação");

    pthread_join(t_prod, NULL);
    pthread_join(t_cons1, NULL);
    pthread_join(t_cons2, NULL);

    sem_destroy(&sem_vazio);
    sem_destroy(&sem_cheio);

    return 0;
}
