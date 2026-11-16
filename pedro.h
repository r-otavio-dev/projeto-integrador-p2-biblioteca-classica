#ifndef PEDRO_H
#define PEDRO_H

/* Pedro: acervo, persistencia e manutencao de livros. */
#include "comum.h"

int buscarIndiceLivro(int id) {
    int i;
    for (i = 0; i < totalLivros; i++) {
        if (acervoLivros[i].codigo == id) return i;
    }
    return -1;
}

int salvarLivrosCSV(void) {
    int i;
    FILE *arquivo = fopen("livros.csv.tmp", "w");
    if (arquivo == NULL) {
        printf("[ERRO] Nao foi possivel abrir livros.csv.tmp.\n");
        return 0;
    }
    for (i = 0; i < totalLivros; i++) {
        fprintf(arquivo, "%d;%s;%s;%s;%.2f;%c;%d\n",
                acervoLivros[i].codigo, acervoLivros[i].titulo,
                acervoLivros[i].autor, acervoLivros[i].categoria,
                acervoLivros[i].valorReposicao,
                acervoLivros[i].statusDisponivel, acervoLivros[i].ativo);
    }
    return fecharCSV(arquivo, "livros.csv.tmp", "livros.csv");
}

int carregarLivrosCSV(void) {
    char linha[512], *campos[7];
    int numeroLinha = 0;
    FILE *arquivo = fopen("livros.csv", "r");
    struct Livro livro;
    if (arquivo == NULL) {
        printf("[ERRO] Nao foi possivel ler livros.csv.\n");
        return 0;
    }
    totalLivros = 0;
    while (fgets(linha, sizeof linha, arquivo) != NULL) {
        numeroLinha++;
        if ((strchr(linha, '\n') == NULL && !feof(arquivo)) ||
            totalLivros >= MAX_REGISTROS ||
            !separarCamposCSV(linha, campos, 7) ||
            !converterInteiro(campos[0], 1, INT_MAX, &livro.codigo) ||
            buscarIndiceLivro(livro.codigo) != -1 ||
            !copiarCampo(livro.titulo, sizeof livro.titulo, campos[1]) ||
            !copiarCampo(livro.autor, sizeof livro.autor, campos[2]) ||
            !copiarCampo(livro.categoria, sizeof livro.categoria, campos[3]) ||
            !converterReal(campos[4], 0, 1000000, &livro.valorReposicao) ||
            strlen(campos[5]) != 1 || strchr("DE", campos[5][0]) == NULL ||
            !converterInteiro(campos[6], 1, 1, &livro.ativo)) {
            printf("[ERRO] livros.csv: linha %d invalida ou ID duplicado.\n", numeroLinha);
            fclose(arquivo);
            return 0;
        }
        livro.statusDisponivel = campos[5][0];
        acervoLivros[totalLivros++] = livro;
    }
    if (ferror(arquivo)) {
        printf("[ERRO] Falha durante a leitura de livros.csv.\n");
        fclose(arquivo);
        return 0;
    }
    fclose(arquivo);
    return 1;
}

void exibirLivro(int indice) {
    struct Livro livro = acervoLivros[indice];
    printf("ID: %d | %s | Autor: %s | Categoria: %s | R$ %.2f | %s\n",
           livro.codigo, livro.titulo, livro.autor, livro.categoria,
           livro.valorReposicao,
           livro.statusDisponivel == 'D' ? "Disponivel" : "Emprestado");
}

void listarLivros(void) {
    int i;
    printf("\n--- ACERVO: %d livro(s) ---\n", totalLivros);
    for (i = 0; i < totalLivros; i++) exibirLivro(i);
    if (totalLivros == 0) printf("Nenhum livro cadastrado.\n");
}

void cadastrarLivro(void) {
    struct Livro livro;
    if (totalLivros >= MAX_REGISTROS) {
        printf("[ERRO] Acervo lotado: limite de %d livros.\n", MAX_REGISTROS);
        return;
    }
    livro.codigo = lerInteiro("Codigo do livro: ", 1, INT_MAX);
    if (buscarIndiceLivro(livro.codigo) != -1) {
        printf("[ERRO] Ja existe um livro com esse codigo.\n");
        return;
    }
    lerTexto("Titulo: ", livro.titulo, sizeof livro.titulo);
    lerTexto("Autor: ", livro.autor, sizeof livro.autor);
    lerTexto("Categoria: ", livro.categoria, sizeof livro.categoria);
    livro.valorReposicao = lerReal("Valor de reposicao (R$): ", 0, 1000000);
    livro.statusDisponivel = 'D';
    livro.ativo = 1;
    acervoLivros[totalLivros++] = livro;
    if (salvarEstadoCSV()) printf("[OK] Livro cadastrado e CSV atualizado.\n");
}

void editarLivro(void) {
    int codigo = lerInteiro("Codigo do livro a editar: ", 1, INT_MAX);
    int indice = buscarIndiceLivro(codigo);
    struct Livro livro;
    if (indice == -1) {
        printf("[ERRO] Livro nao encontrado.\n");
        return;
    }
    livro = acervoLivros[indice];
    exibirLivro(indice);
    /* Codigo e disponibilidade permanecem controlados pelo sistema. */
    lerTexto("Novo titulo: ", livro.titulo, sizeof livro.titulo);
    lerTexto("Novo autor: ", livro.autor, sizeof livro.autor);
    lerTexto("Nova categoria: ", livro.categoria, sizeof livro.categoria);
    livro.valorReposicao = lerReal("Novo valor de reposicao (R$): ", 0, 1000000);
    acervoLivros[indice] = livro;
    if (salvarEstadoCSV()) printf("[OK] Livro editado e CSV atualizado.\n");
}

void removerLivro(void) {
    int codigo = lerInteiro("Codigo do livro a remover: ", 1, INT_MAX);
    int indice = buscarIndiceLivro(codigo);
    int i;
    if (indice == -1) {
        printf("[ERRO] Livro nao encontrado.\n");
        return;
    }
    if (livroTemEmprestimo(codigo)) {
        printf("[ERRO] Livro vinculado a emprestimo: preserve o historico.\n");
        return;
    }
    for (i = indice; i < totalLivros - 1; i++) acervoLivros[i] = acervoLivros[i + 1];
    totalLivros--;
    if (salvarEstadoCSV()) printf("[OK] Livro removido e CSV atualizado.\n");
}

void moduloLivros(void) {
    int opcao;
    do {
        limparTela();
        printf("\n--- ACERVO DE LIVROS (Pedro) ---\n");
        printf("1. Cadastrar livro\n2. Listar livros\n3. Editar livro\n");
        printf("4. Remover livro\n5. Buscar por codigo ou titulo\n0. Voltar\n");
        opcao = lerInteiro("Escolha: ", 0, 5);
        switch (opcao) {
            case 1: cadastrarLivro(); break;
            case 2: listarLivros(); break;
            case 3: editarLivro(); break;
            case 4: removerLivro(); break;
            case 5: buscarLivroPorCodigoOuTitulo(); break;
            case 0: break;
        }
        if (opcao != 0) pausar();
    } while (opcao != 0);
}

#endif
