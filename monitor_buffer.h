#ifndef MONITOR_BUFFER_H
#define MONITOR_BUFFER_H

typedef struct {
    int id;
} Frame;

void inicializar_monitor();
void destruir_monitor();
void produzir(Frame f);
Frame consumir(const char *nome);
void imprimir_estado();

#endif
