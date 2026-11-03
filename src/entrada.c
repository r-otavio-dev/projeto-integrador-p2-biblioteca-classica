#include <stdio.h>
#include <string.h>
#include "biblioteca.h"

static void limparEntrada(void) {
    int caractere;

    while ((caractere = getchar()) != '\n' && caractere != EOF) {
    }
}

int lerInteiro(const char mensagem[]) {
    int valor;
    int resultado;

    while (1) {
        printf("%s", mensagem);
        resultado = scanf("%d", &valor);
        if (resultado == 1) {
            limparEntrada();
            return valor;
        }
        if (resultado == EOF) {
            return 0;
        }

        printf("Entrada invalida. Digite um numero inteiro.\n");
        limparEntrada();
    }
}

float lerFloat(const char mensagem[]) {
    float valor;
    int resultado;

    while (1) {
        printf("%s", mensagem);
        resultado = scanf("%f", &valor);
        if (resultado == 1) {
            limparEntrada();
            return valor;
        }
        if (resultado == EOF) {
            return 0;
        }

        printf("Entrada invalida. Digite um numero.\n");
        limparEntrada();
    }
}

char lerCaractere(const char mensagem[]) {
    char texto[20];

    lerTexto(mensagem, texto, sizeof(texto));
    return texto[0];
}

void lerTexto(const char mensagem[], char texto[], int tamanho) {
    printf("%s", mensagem);
    if (fgets(texto, tamanho, stdin) != NULL) {
        if (strchr(texto, '\n') != NULL) {
            texto[strcspn(texto, "\n")] = '\0';
        } else {
            limparEntrada();
        }
    } else {
        texto[0] = '\0';
    }
}
