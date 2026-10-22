#include <stdio.h>
#include "biblioteca.h"

void removerRegistro(void) {
    printf("[Modulo C] Remocao ainda nao implementada.\n");
}

void exibirGradeControle(void) {
    int linha;
    int coluna;

    printf("[Modulo C] Grade provisoria; confirmem o significado com o professor.\n");
    for (linha = 0; linha < LINHAS_GRADE; linha++) {
        for (coluna = 0; coluna < COLUNAS_GRADE; coluna++) {
            printf("%d ", gradeControle[linha][coluna]);
        }
        printf("\n");
    }
}

