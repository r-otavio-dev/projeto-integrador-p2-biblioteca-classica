#include <stdio.h>
#include "biblioteca.h"

static void exibirCabecalho(void) {
    printf("\n============================================================\n");
    printf("   BIBLIOTECA CLASSICA - FICHARIO, ACERVO E EMPRESTIMOS\n");
    printf("============================================================\n");
}

int main(void) {
    int opcao;

    do {
        exibirCabecalho();
        printf("1. Modulo de Acervo de Livros (Pedro)\n");
        printf("2. Modulo de Fichario, Buscas e Ordenacao (Tavin)\n");
        printf("3. Modulo de Emprestimos e Devolucoes (Kaue)\n");
        printf("4. Modulo de Reservas e Relatorios (Joelson)\n");
        printf("0. Encerrar o sistema\n");

        opcao = lerInteiro("Opcao: ");

        switch (opcao) {
            case 1:
                moduloLivros();
                break;
            case 2:
                moduloBuscasEFichario();
                break;
            case 3:
                moduloEmprestimos();
                break;
            case 4:
                moduloRelatoriosEReservas();
                break;
            case 0:
                printf("Encerrando o sistema.\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    return 0;
}
