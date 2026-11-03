#include <stdio.h>
#include <string.h>
#include "biblioteca.h"

static void mostrarLivro(int indice) {
    printf("Codigo: %d | Titulo: %s | Autor: %s | Status: %d\n",
           registros[indice].id,
           registros[indice].descricao,
           registros[indice].autor,
           registros[indice].status);
}

void buscarRegistro(void) {
    int opcao;
    int codigo;
    int indice;
    int encontrou = 0;
    char texto[100];

    if (totalRegistros == 0) {
        printf("O acervo esta vazio.\n");
        return;
    }

    printf("\n1. Buscar por codigo\n");
    printf("2. Buscar por titulo\n");
    printf("3. Buscar por autor\n");
    printf("Opcao: ");

    if (scanf("%d", &opcao) != 1) {
        printf("Opcao invalida.\n");
        return;
    }

    getchar();

    if (opcao == 1) {
        printf("Digite o codigo: ");

        if (scanf("%d", &codigo) != 1) {
            printf("Codigo invalido.\n");
            return;
        }

        for (indice = 0; indice < totalRegistros; indice++) {
            if (registros[indice].id == codigo) {
                mostrarLivro(indice);
                encontrou = 1;
                break;
            }
        }
    } else if (opcao == 2) {
        printf("Digite o titulo: ");
        fgets(texto, sizeof(texto), stdin);
        texto[strcspn(texto, "\n")] = '\0';

        for (indice = 0; indice < totalRegistros; indice++) {
            if (strstr(registros[indice].descricao, texto) != NULL) {
                mostrarLivro(indice);
                encontrou = 1;
            }
        }
    } else if (opcao == 3) {
        printf("Digite o autor: ");
        fgets(texto, sizeof(texto), stdin);
        texto[strcspn(texto, "\n")] = '\0';

        for (indice = 0; indice < totalRegistros; indice++) {
            if (strstr(registros[indice].autor, texto) != NULL) {
                mostrarLivro(indice);
                encontrou = 1;
            }
        }
    } else {
        printf("Opcao invalida.\n");
        return;
    }

    if (encontrou == 0) {
        printf("Nenhum livro encontrado.\n");
    }
}

void ordenarRegistrosBubbleSort(void) {
    int i;
    int j;
    RegistroBiblioteca auxiliar;

    if (totalRegistros == 0) {
        printf("O acervo esta vazio.\n");
        return;
    }

    for (i = 0; i < totalRegistros - 1; i++) {
        for (j = 0; j < totalRegistros - 1 - i; j++) {
            if (strcmp(registros[j].descricao,
                       registros[j + 1].descricao) > 0) {
                auxiliar = registros[j];
                registros[j] = registros[j + 1];
                registros[j + 1] = auxiliar;
            }
        }
    }

    printf("\nAcervo ordenado por titulo:\n");
    for (i = 0; i < totalRegistros; i++) {
        mostrarLivro(i);
    }
}
