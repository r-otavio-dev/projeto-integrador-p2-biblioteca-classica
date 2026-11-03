#include <ctype.h>
#include <stdio.h>
#include "biblioteca.h"

int localizarLivro(int codigo) {
    int i;

    for (i = 0; i < totalLivros; i++) {
        if (acervoLivros[i].codigo == codigo && acervoLivros[i].ativo == 1) {
            return i;
        }
    }

    return -1;
}

static int codigoLivroJaUsado(int codigo) {
    int i;

    for (i = 0; i < totalLivros; i++) {
        if (acervoLivros[i].codigo == codigo) {
            return 1;
        }
    }

    return 0;
}

static void mostrarLivro(int i) {
    printf("Codigo: %d | Titulo: %s | Autor: %s\n",
           acervoLivros[i].codigo,
           acervoLivros[i].titulo,
           acervoLivros[i].autor);
    printf("Categoria: %s | Valor: R$ %.2f | Status: %c\n",
           acervoLivros[i].categoria,
           acervoLivros[i].valorReposicao,
           acervoLivros[i].statusDisponivel);
}

static void cadastrarLivro(void) {
    Livro novoLivro;

    if (totalLivros >= CAPACIDADE) {
        printf("Limite de livros atingido.\n");
        return;
    }

    novoLivro.codigo = lerInteiro("Codigo do livro: ");
    if (codigoLivroJaUsado(novoLivro.codigo)) {
        printf("Ja existe um livro com esse codigo.\n");
        return;
    }

    lerTexto("Titulo: ", novoLivro.titulo, sizeof(novoLivro.titulo));
    lerTexto("Autor: ", novoLivro.autor, sizeof(novoLivro.autor));
    lerTexto("Categoria (Romance, Tecnico, Ficcao ou Outra): ",
             novoLivro.categoria, sizeof(novoLivro.categoria));
    novoLivro.valorReposicao = lerFloat("Valor de reposicao: R$ ");

    if (novoLivro.valorReposicao < 0) {
        printf("O valor nao pode ser negativo.\n");
        return;
    }

    novoLivro.statusDisponivel = 'D';
    novoLivro.ativo = 1;
    acervoLivros[totalLivros] = novoLivro;
    totalLivros++;

    printf("Livro cadastrado com sucesso.\n");
}

static void listarLivros(void) {
    int i;
    int encontrou = 0;

    printf("\n--- LIVROS CADASTRADOS ---\n");
    for (i = 0; i < totalLivros; i++) {
        if (acervoLivros[i].ativo == 1) {
            mostrarLivro(i);
            printf("------------------------------------------------------------\n");
            encontrou = 1;
        }
    }

    if (encontrou == 0) {
        printf("Nenhum livro cadastrado.\n");
    }
}

static void editarLivro(void) {
    int codigo = lerInteiro("Codigo do livro que deseja editar: ");
    int i = localizarLivro(codigo);
    Livro livroEditado;

    if (i == -1) {
        printf("Livro nao encontrado.\n");
        return;
    }

    livroEditado = acervoLivros[i];
    lerTexto("Novo titulo: ", livroEditado.titulo,
             sizeof(livroEditado.titulo));
    lerTexto("Novo autor: ", livroEditado.autor,
             sizeof(livroEditado.autor));
    lerTexto("Nova categoria: ", livroEditado.categoria,
             sizeof(livroEditado.categoria));
    livroEditado.valorReposicao = lerFloat("Novo valor de reposicao: R$ ");

    if (livroEditado.valorReposicao < 0) {
        printf("O valor nao pode ser negativo.\n");
        return;
    }

    acervoLivros[i] = livroEditado;
    printf("Livro atualizado com sucesso.\n");
}

static void excluirLivro(void) {
    int codigo = lerInteiro("Codigo do livro que deseja excluir: ");
    int i = localizarLivro(codigo);

    if (i == -1) {
        printf("Livro nao encontrado.\n");
        return;
    }

    if (acervoLivros[i].statusDisponivel == 'E') {
        printf("O livro esta emprestado e nao pode ser excluido.\n");
        return;
    }

    acervoLivros[i].ativo = 0;
    printf("Livro excluido do acervo.\n");
}

void moduloLivros(void) {
    int opcao;

    do {
        printf("\n--- MODULO 1: ACERVO DE LIVROS (PEDRO) ---\n");
        printf("1. Cadastrar livro\n");
        printf("2. Listar livros\n");
        printf("3. Editar livro\n");
        printf("4. Excluir livro\n");
        printf("0. Voltar\n");

        opcao = lerInteiro("Opcao: ");

        switch (opcao) {
            case 1:
                cadastrarLivro();
                break;
            case 2:
                listarLivros();
                break;
            case 3:
                editarLivro();
                break;
            case 4:
                excluirLivro();
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}
