#ifndef COMUM_H
#define COMUM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

#define MAX_REGISTROS 50

/* Modelagem preservada do projeto do Checkpoint 1. */
struct Livro {
    int codigo;
    char titulo[50];
    char autor[40];
    char categoria[30];
    float valorReposicao;
    char statusDisponivel;
    int ativo;
};
struct Leitor {
    int codigoLeitor;
    char nome[50];
    char telefone[20];
    char tipoAssociado;
    float saldoMultas;
    int ativo;
};
struct Emprestimo {
    int idEmprestimo;
    int codigoLivro;
    int codigoLeitor;
    char nomeLeitor[50];
    int diasPrazo;
    float taxaAtrasoDiaria;
    char situacao;
};
struct Reserva {
    int idReserva;
    char tituloDesejado[50];
    char categoriaObra[30];
    float custoEstimado;
    char prioridade;
    int atendida;
};

struct Livro acervoLivros[MAX_REGISTROS];
struct Leitor ficharioLeitores[MAX_REGISTROS];
struct Emprestimo listaEmprestimos[MAX_REGISTROS];
struct Reserva listaReservas[MAX_REGISTROS];
int totalLivros = 0, totalLeitores = 0;
int totalEmprestimos = 0, totalReservas = 0;
int matrizEstatisticas[4][2];
int modoLote = 0;

/* Interfaces compartilhadas: os corpos ficam nos arquivos dos responsaveis. */
int buscarIndiceLivro(int id);
int buscarIndiceLeitor(int id);
int buscarIndiceEmprestimo(int id);
int buscarIndiceReserva(int id);
int salvarLivrosCSV(void);
int salvarLeitoresCSV(void);
int salvarEmprestimosCSV(void);
int salvarReservasCSV(void);
int carregarLivrosCSV(void);
int carregarLeitoresCSV(void);
int carregarEmprestimosCSV(void);
int carregarReservasCSV(void);
int livroTemEmprestimo(int codigo);
int leitorTemEmprestimo(int codigo);
void atualizarNomesEmprestimos(void);
int salvarEstadoCSV(void);
void buscarLivroPorCodigoOuTitulo(void);
void buscarLivrosPorAutor(void);
void ordenarAcervoBubbleSort(void);

/* Toda entrada e lida como linha: texto invalido nao fica preso no teclado. */
int lerLinha(char *destino, int tamanho) {
    if (fgets(destino, tamanho, stdin) == NULL) {
        if (ferror(stdin)) {
            fprintf(stderr, "[ERRO] Falha ao ler o teclado.\n");
            exit(EXIT_FAILURE);
        }
        printf("\nEntrada encerrada.\n");
        exit(EXIT_SUCCESS);
    }
    if (strchr(destino, '\n') == NULL && !feof(stdin)) {
        int c = getchar();
        if (c == '\r') c = getchar();
        if (c != '\n' && c != EOF) {
            while ((c = getchar()) != '\n' && c != EOF) { }
            return 0;
        }
    }
    /* Retira apenas a terminacao; um CR no meio do texto e invalido. */
    destino[strcspn(destino, "\n")] = '\0';
    if (strlen(destino) > 0 && destino[strlen(destino) - 1] == '\r') destino[strlen(destino) - 1] = '\0';
    if (strchr(destino, '\r') != NULL) return 0;
    return 1;
}

int converterInteiro(const char *texto, int minimo, int maximo, int *valor) {
    char *fim;
    long numero;
    errno = 0;
    numero = strtol(texto, &fim, 10);
    if (*texto == '\0' || *fim != '\0' || errno == ERANGE ||
        numero < minimo || numero > maximo) return 0;
    *valor = (int)numero;
    return 1;
}

int converterReal(const char *texto, float minimo, float maximo, float *valor) {
    char *fim;
    float numero;
    errno = 0;
    numero = strtof(texto, &fim);
    if (*texto == '\0' || *fim != '\0' || errno == ERANGE ||
        !isfinite(numero) || numero < minimo || numero > maximo) return 0;
    *valor = numero;
    return 1;
}

int lerInteiro(const char *mensagem, int minimo, int maximo) {
    char linha[128];
    int valor;
    for (;;) {
        printf("%s", mensagem);
        if (lerLinha(linha, sizeof linha) && converterInteiro(linha, minimo, maximo, &valor)) return valor;
        printf("[ERRO] Digite um inteiro entre %d e %d.\n", minimo, maximo);
    }
}

float lerReal(const char *mensagem, float minimo, float maximo) {
    char linha[128];
    float valor;
    for (;;) {
        printf("%s", mensagem);
        if (lerLinha(linha, sizeof linha) && converterReal(linha, minimo, maximo, &valor)) {
            /* Os campos reais do projeto sao monetarios: guardar centavos. */
            return (float)((long)(valor * 100.0f + 0.5f)) / 100.0f;
        }
        printf("[ERRO] Digite um numero entre %.2f e %.2f (decimal com ponto).\n", minimo, maximo);
    }
}

void lerTexto(const char *mensagem, char *destino, int tamanho) {
    for (;;) {
        printf("%s", mensagem);
        if (lerLinha(destino, tamanho) && destino[0] != '\0' &&
            strspn(destino, " \t") != strlen(destino) && strchr(destino, ';') == NULL) return;
        printf("[ERRO] Texto obrigatorio, sem ponto e virgula, com ate %d bytes.\n", tamanho - 1);
    }
}

char lerCaractere(const char *mensagem, const char *permitidos) {
    char linha[8];
    for (;;) {
        lerTexto(mensagem, linha, sizeof linha);
        if (strlen(linha) == 1 && strchr(permitidos, linha[0]) != NULL) return linha[0];
        printf("[ERRO] Escolha um destes caracteres: %s.\n", permitidos);
    }
}

void limparTela(void) {
    if (modoLote) return;
#ifdef _WIN32
    (void)system("cls");
#else
    (void)system("clear");
#endif
}

void pausar(void) {
    char linha[8];
    if (!modoLote) {
        printf("\nPressione ENTER para continuar...");
        (void)lerLinha(linha, sizeof linha);
    }
}

/* CSV sem cabecalho, com campos nao vazios e delimitador ';'. */
int separarCamposCSV(char *linha, char *campos[], int quantidade) {
    int i;
    char *proximo;
    size_t tamanho = strlen(linha);
    if (tamanho > 0 && linha[tamanho - 1] == '\n') linha[--tamanho] = '\0';
    if (tamanho > 0 && linha[tamanho - 1] == '\r') linha[--tamanho] = '\0';
    if (strchr(linha, '\r') != NULL || strchr(linha, '\n') != NULL) return 0;
    campos[0] = linha;
    for (i = 1; i < quantidade; i++) {
        proximo = strchr(campos[i - 1], ';');
        if (proximo == NULL) return 0;
        *proximo = '\0';
        campos[i] = proximo + 1;
    }
    if (strchr(campos[quantidade - 1], ';') != NULL) return 0;
    for (i = 0; i < quantidade; i++) if (campos[i][0] == '\0') return 0;
    return 1;
}

int copiarCampo(char *destino, int tamanho, const char *origem) {
    if (strlen(origem) >= (size_t)tamanho || strspn(origem, " \t") == strlen(origem)) return 0;
    strcpy(destino, origem);
    return 1;
}

int fecharCSV(FILE *arquivo, const char *temporario, const char *destino) {
    int ok = !ferror(arquivo);
    if (fclose(arquivo) != 0) ok = 0;
    if (!ok || rename(temporario, destino) != 0) {
        printf("[ERRO] Nao foi possivel gravar %s.\n", destino);
        remove(temporario);
        return 0;
    }
    return 1;
}

#endif
