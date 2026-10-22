#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

/* Valores provisórios: o grupo deve confirmar o tamanho adequado. */
#define CAPACIDADE_REGISTROS 100
#define LINHAS_GRADE 5
#define COLUNAS_GRADE 5

/*
 * Modelo inicial mínimo para o Checkpoint 1.
 * Os campos definitivos de fichário, acervo e empréstimos não aparecem no PDF.
 */
typedef struct {
    int id;
    char descricao[100];
    int status;
} RegistroBiblioteca;

/* Base em memória RAM e contadores no escopo global, conforme o padrão da aula. */
extern RegistroBiblioteca registros[CAPACIDADE_REGISTROS];
extern int totalRegistros;
extern int gradeControle[LINHAS_GRADE][COLUNAS_GRADE];

/* Integrante A: inserção e cadastro. */
void cadastrarRegistro(void);

/* Integrante B: busca e modificação. */
void buscarRegistro(void);
void modificarRegistro(void);

/* Integrante C: remoção e matriz. */
void removerRegistro(void);
void exibirGradeControle(void);

/* Integrante D: relatórios e ordenação. */
void listarRegistros(void);
void exibirRelatorios(void);
void ordenarRegistrosBubbleSort(void);

#endif

