#include <stdio.h>
#include "biblioteca.h"

void cadastrarRegistro(void) {
    if (totalRegistros >= CAPACIDADE_REGISTROS) {
        printf("Limite da base em memoria RAM atingido.\n");
        return;
    }

    printf("[Modulo A] Cadastro ainda nao implementado.\n");
    printf("Definam primeiro os campos de fichario, acervo e emprestimos.\n");
}

