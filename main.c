/* Biblioteca Classica - Checkpoint 2
   Compile somente este arquivo. Cada integrante tem suas funcoes no seu .h. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

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

/* Funcoes usadas entre os modulos. O corpo delas aparece nos .h abaixo. */
int buscarIndiceLivro(int id);
int buscarIndiceLeitor(int id);
int livroTemEmprestimo(int codigo);
int leitorTemEmprestimo(int codigo);
void salvarEmprestimosCSV(void);
void atualizarNomesEmprestimos(void);
void buscarLivroPorCodigoOuTitulo(void);

/* ENTRADAS: lemos uma linha antes de converter para numero. */
int lerLinha(char *texto, int tamanho) {
    int c;
    if (fgets(texto, tamanho, stdin) == NULL) {
        if (ferror(stdin)) {
            printf("[ERRO] Falha ao ler a entrada.\n");
            exit(1);
        }
        exit(0);
    }
    if (strchr(texto, '\n') == NULL && !feof(stdin)) {
        c = getchar();
        if (c == '\r') c = getchar();
        if (c != '\n' && c != EOF) {
            while ((c = getchar()) != '\n' && c != EOF) { }
            return 0;
        }
    }
    texto[strcspn(texto, "\n")] = '\0';
    if (strlen(texto) > 0 && texto[strlen(texto) - 1] == '\r') texto[strlen(texto) - 1] = '\0';
    return strchr(texto, '\r') == NULL;
}

int lerInteiro(const char *pergunta, int minimo, int maximo) {
    char texto[80], sobra;
    int numero;
    do {
        printf("%s", pergunta);
        /* O limite de 9 caracteres evita converter um numero enorme. */
        if (lerLinha(texto, sizeof texto) && sscanf(texto, "%9d %c", &numero, &sobra) == 1 &&
            numero >= minimo && numero <= maximo) return numero;
        printf("[ERRO] Digite um inteiro de %d a %d.\n", minimo, maximo);
    } while (1);
}

float lerReal(const char *pergunta, float minimo, float maximo) {
    char texto[80], sobra;
    float numero;
    do {
        printf("%s", pergunta);
        if (lerLinha(texto, sizeof texto) && sscanf(texto, "%f %c", &numero, &sobra) == 1 &&
            numero >= minimo && numero <= maximo) {
            return (int)(numero * 100.0f + 0.5f) / 100.0f;
        }
        printf("[ERRO] Digite um valor de %.2f a %.2f, com ponto decimal.\n", minimo, maximo);
    } while (1);
}

void lerTexto(const char *pergunta, char *texto, int tamanho) {
    do {
        printf("%s", pergunta);
        if (lerLinha(texto, tamanho) && texto[0] != '\0' &&
            strspn(texto, " \t") != strlen(texto) && strchr(texto, ';') == NULL) return;
        printf("[ERRO] Texto obrigatorio, sem ';', com ate %d bytes.\n", tamanho - 1);
    } while (1);
}

char lerCaractere(const char *pergunta, const char *opcoes) {
    char texto[8];
    do {
        lerTexto(pergunta, texto, sizeof texto);
        if (strlen(texto) == 1 && strchr(opcoes, texto[0]) != NULL) return texto[0];
        printf("[ERRO] Escolha um destes caracteres: %s.\n", opcoes);
    } while (1);
}

void limparTela(void) {
    if (modoLote) return;
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar(void) {
    char texto[8];
    if (!modoLote) {
        printf("\nPressione ENTER para continuar...");
        lerLinha(texto, sizeof texto);
    }
}

/* Se o arquivo falhar, paramos sem mostrar uma mensagem falsa de sucesso. */
FILE *abrirArquivo(const char *nome, const char *modo) {
    FILE *arquivo = fopen(nome, modo);
    if (arquivo == NULL) {
        printf("[ERRO] Nao foi possivel abrir %s.\n", nome);
        exit(1);
    }
    return arquivo;
}

void fecharArquivo(FILE *arquivo) {
    int erro = ferror(arquivo);
    if (fclose(arquivo) != 0) erro = 1;
    if (erro) {
        printf("[ERRO] Falha ao ler ou gravar o arquivo.\n");
        exit(1);
    }
}

/* MODULOS: as funcoes ficam separadas conforme a exigencia da atividade. */
#include "pedro.h"
#include "tavin.h"
#include "kaue.h"
#include "joelson.h"

/* Confere se cada livro emprestado tem exatamente um emprestimo ativo. */
int validarDisponibilidade(void) {
    int i, j, ativos;
    for (i = 0; i < totalLivros; i++) {
        ativos = 0;
        for (j = 0; j < totalEmprestimos; j++) {
            if (listaEmprestimos[j].codigoLivro == acervoLivros[i].codigo &&
                listaEmprestimos[j].situacao == 'A') ativos++;
        }
        if (ativos > 1 || acervoLivros[i].statusDisponivel != (ativos == 1 ? 'E' : 'D')) {
            printf("[ERRO] Disponibilidade inconsistente no livro %d.\n", acervoLivros[i].codigo);
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    int opcao;
    setlocale(LC_ALL, "");
    setlocale(LC_NUMERIC, "C"); /* O CSV sempre usa ponto decimal. */
    if (argc == 2 && strcmp(argv[1], "--sem-pausa") == 0) modoLote = 1;
    else if (argc != 1) {
        printf("Uso: %s [--sem-pausa]\n", argv[0]);
        return 1;
    }
    if (!carregarLivrosCSV() || !carregarLeitoresCSV() ||
        !carregarEmprestimosCSV() || !carregarReservasCSV() || !validarDisponibilidade()) return 1;
    printf("Carregados: %d livros, %d leitores, %d emprestimos e %d reservas.\n",
           totalLivros, totalLeitores, totalEmprestimos, totalReservas);
    do {
        limparTela();
        printf("\n=== BIBLIOTECA CLASSICA ===\n");
        printf("1. Livros - Pedro\n2. Leitores e buscas - Tavin\n");
        printf("3. Emprestimos - Kaue\n4. Reservas e relatorios - Joelson\n0. Sair\n");
        opcao = lerInteiro("Escolha: ", 0, 4);
        switch (opcao) {
            case 1: moduloLivros(); break;
            case 2: moduloBuscasEFichario(); break;
            case 3: moduloEmprestimos(); break;
            case 4: moduloRelatoriosEReservas(); break;
            case 0: printf("Sistema encerrado.\n"); break;
        }
    } while (opcao != 0);
    return 0;
}
