#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#define CAPACIDADE 50
#define CATEGORIAS 4
#define SITUACOES_LIVRO 2

typedef struct {
    int codigo;
    char titulo[50];
    char autor[40];
    char categoria[30];
    float valorReposicao;
    char statusDisponivel;
    int ativo;
} Livro;

typedef struct {
    int codigoLeitor;
    char nome[50];
    char telefone[20];
    char tipoAssociado;
    float saldoMultas;
    int ativo;
} Leitor;

typedef struct {
    int idEmprestimo;
    int codigoLivro;
    int codigoLeitor;
    char nomeLeitor[50];
    int diasPrazo;
    float taxaAtrasoDiaria;
    char situacao;
} Emprestimo;

typedef struct {
    int idReserva;
    char tituloDesejado[50];
    char categoriaObra[30];
    float custoEstimado;
    char prioridade;
    int atendida;
} Reserva;

extern Livro acervoLivros[CAPACIDADE];
extern Leitor ficharioLeitores[CAPACIDADE];
extern Emprestimo listaEmprestimos[CAPACIDADE];
extern Reserva listaReservas[CAPACIDADE];
extern int totalLivros;
extern int totalLeitores;
extern int totalEmprestimos;
extern int totalReservas;
extern int matrizEstatisticas[CATEGORIAS][SITUACOES_LIVRO];

int lerInteiro(const char mensagem[]);
float lerFloat(const char mensagem[]);
char lerCaractere(const char mensagem[]);
void lerTexto(const char mensagem[], char texto[], int tamanho);

int localizarLivro(int codigo);
int localizarLeitor(int codigo);
int localizarEmprestimo(int idEmprestimo);
int localizarReserva(int idReserva);

void moduloLivros(void);
void moduloBuscasEFichario(void);
void moduloEmprestimos(void);
void moduloRelatoriosEReservas(void);

#endif
