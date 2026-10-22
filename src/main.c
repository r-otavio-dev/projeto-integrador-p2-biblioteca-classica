#include <stdio.h>
#include "biblioteca.h"

static void exibirCabecalho(void) {
    printf("\n========================================\n");
    printf("  BIBLIOTECA CLASSICA - PROJETO P2\n");
    printf("========================================\n");
}

static void exibirMenu(void) {
    printf("\n1. Inserir/cadastrar registro\n");
    printf("2. Buscar registro\n");
    printf("3. Modificar registro\n");
    printf("4. Remover registro\n");
    printf("5. Exibir grade de controle\n");
    printf("6. Listar registros\n");
    printf("7. Exibir relatorios\n");
    printf("8. Ordenar registros (Bubble Sort)\n");
    printf("0. Sair\n");
    printf("Opcao: ");
}

int main(void) {
    int opcao;

    do {
        exibirCabecalho();
        exibirMenu();

        if (scanf("%d", &opcao) != 1) {
            int caractere;
            while ((caractere = getchar()) != '\n' && caractere != EOF) {
                /* Descarta a entrada inválida. */
            }
            opcao = -1;
        }

        switch (opcao) {
            case 1:
                cadastrarRegistro();
                break;
            case 2:
                buscarRegistro();
                break;
            case 3:
                modificarRegistro();
                break;
            case 4:
                removerRegistro();
                break;
            case 5:
                exibirGradeControle();
                break;
            case 6:
                listarRegistros();
                break;
            case 7:
                exibirRelatorios();
                break;
            case 8:
                ordenarRegistrosBubbleSort();
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

