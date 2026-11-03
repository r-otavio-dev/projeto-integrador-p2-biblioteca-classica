#include <stdio.h>
#include <string.h>
#include "biblioteca.h"

int localizarEmprestimo(int idEmprestimo) {
    int i;

    for (i = 0; i < totalEmprestimos; i++) {
        if (listaEmprestimos[i].idEmprestimo == idEmprestimo) {
            return i;
        }
    }

    return -1;
}

static float calcularMulta(int diasAtraso, float taxaDiaria) {
    return diasAtraso * taxaDiaria;
}

static void registrarEmprestimo(void) {
    Emprestimo novoEmprestimo;
    int indiceLivro;
    int indiceLeitor;

    if (totalEmprestimos >= CAPACIDADE) {
        printf("Limite de emprestimos atingido.\n");
        return;
    }

    novoEmprestimo.idEmprestimo = lerInteiro("ID do emprestimo: ");
    if (localizarEmprestimo(novoEmprestimo.idEmprestimo) != -1) {
        printf("Ja existe um emprestimo com esse ID.\n");
        return;
    }

    novoEmprestimo.codigoLivro = lerInteiro("Codigo do livro: ");
    indiceLivro = localizarLivro(novoEmprestimo.codigoLivro);
    if (indiceLivro == -1) {
        printf("Livro nao encontrado.\n");
        return;
    }

    if (acervoLivros[indiceLivro].statusDisponivel != 'D') {
        printf("O livro nao esta disponivel.\n");
        return;
    }

    novoEmprestimo.codigoLeitor = lerInteiro("Carteirinha do leitor: ");
    indiceLeitor = localizarLeitor(novoEmprestimo.codigoLeitor);
    if (indiceLeitor == -1) {
        printf("Leitor nao encontrado ou inativo.\n");
        return;
    }

    novoEmprestimo.diasPrazo = lerInteiro("Prazo em dias: ");
    novoEmprestimo.taxaAtrasoDiaria = lerFloat("Multa por dia de atraso: R$ ");

    if (novoEmprestimo.diasPrazo <= 0 ||
        novoEmprestimo.taxaAtrasoDiaria < 0) {
        printf("Prazo ou taxa invalida.\n");
        return;
    }

    strncpy(novoEmprestimo.nomeLeitor, ficharioLeitores[indiceLeitor].nome,
            sizeof(novoEmprestimo.nomeLeitor) - 1);
    novoEmprestimo.nomeLeitor[sizeof(novoEmprestimo.nomeLeitor) - 1] = '\0';
    novoEmprestimo.situacao = 'A';

    listaEmprestimos[totalEmprestimos] = novoEmprestimo;
    totalEmprestimos++;
    acervoLivros[indiceLivro].statusDisponivel = 'E';

    printf("Emprestimo registrado com sucesso.\n");
}

static void listarEmprestimosAtivos(void) {
    int i;
    int encontrou = 0;

    for (i = 0; i < totalEmprestimos; i++) {
        if (listaEmprestimos[i].situacao == 'A') {
            printf("ID: %d | Livro: %d | Leitor: %d - %s\n",
                   listaEmprestimos[i].idEmprestimo,
                   listaEmprestimos[i].codigoLivro,
                   listaEmprestimos[i].codigoLeitor,
                   listaEmprestimos[i].nomeLeitor);
            printf("Prazo: %d dias | Multa diaria: R$ %.2f\n",
                   listaEmprestimos[i].diasPrazo,
                   listaEmprestimos[i].taxaAtrasoDiaria);
            printf("------------------------------------------------------------\n");
            encontrou = 1;
        }
    }

    if (encontrou == 0) {
        printf("Nenhum emprestimo ativo.\n");
    }
}

static void renovarEmprestimo(void) {
    int id = lerInteiro("ID do emprestimo: ");
    int i = localizarEmprestimo(id);
    int novoPrazo;

    if (i == -1 || listaEmprestimos[i].situacao != 'A') {
        printf("Emprestimo ativo nao encontrado.\n");
        return;
    }

    novoPrazo = lerInteiro("Novo prazo em dias: ");
    if (novoPrazo <= 0) {
        printf("Prazo invalido.\n");
        return;
    }

    listaEmprestimos[i].diasPrazo = novoPrazo;
    printf("Emprestimo renovado com sucesso.\n");
}

static void registrarDevolucao(void) {
    int id = lerInteiro("ID do emprestimo: ");
    int i = localizarEmprestimo(id);
    int indiceLivro;
    int indiceLeitor;
    int diasAtraso;
    float multa;

    if (i == -1 || listaEmprestimos[i].situacao != 'A') {
        printf("Emprestimo ativo nao encontrado.\n");
        return;
    }

    diasAtraso = lerInteiro("Dias de atraso (0 se devolvido no prazo): ");
    if (diasAtraso < 0) {
        printf("Quantidade de dias invalida.\n");
        return;
    }

    multa = calcularMulta(diasAtraso,
                          listaEmprestimos[i].taxaAtrasoDiaria);
    indiceLivro = localizarLivro(listaEmprestimos[i].codigoLivro);
    indiceLeitor = localizarLeitor(listaEmprestimos[i].codigoLeitor);

    listaEmprestimos[i].situacao = 'F';
    if (indiceLivro != -1) {
        acervoLivros[indiceLivro].statusDisponivel = 'D';
    }
    if (indiceLeitor != -1) {
        ficharioLeitores[indiceLeitor].saldoMultas += multa;
    }

    printf("Devolucao registrada. Multa gerada: R$ %.2f\n", multa);
}

void moduloEmprestimos(void) {
    int opcao;

    do {
        printf("\n--- MODULO 3: EMPRESTIMOS E DEVOLUCOES (KAUE) ---\n");
        printf("1. Registrar emprestimo\n");
        printf("2. Listar emprestimos ativos\n");
        printf("3. Renovar prazo\n");
        printf("4. Registrar devolucao\n");
        printf("0. Voltar\n");

        opcao = lerInteiro("Opcao: ");

        switch (opcao) {
            case 1:
                registrarEmprestimo();
                break;
            case 2:
                listarEmprestimosAtivos();
                break;
            case 3:
                renovarEmprestimo();
                break;
            case 4:
                registrarDevolucao();
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}
