#include <stdio.h>
#include "biblioteca.h"

void listarRegistros(void) {
    int indice;

    printf("[Modulo D] Total de registros: %d\n", totalRegistros);
    for (indice = 0; indice < totalRegistros; indice++) {
        printf("ID: %d | Descricao: %s | Status: %d\n",
               registros[indice].id,
               registros[indice].descricao,
               registros[indice].status);
    }
}

void exibirRelatorios(void) {
    printf("[Modulo D] Relatorios ainda nao implementados.\n");
}

void ordenarRegistrosBubbleSort(void) {
    printf("[Modulo D] Bubble Sort ainda nao implementado.\n");
}

