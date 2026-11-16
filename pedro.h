#ifndef PEDRO_H
#define PEDRO_H

/* Pedro: acervo, persistencia e manutencao de livros. */

int buscarIndiceLivro(int id) {
    int i;
    for (i = 0; i < totalLivros; i++) {
        if (acervoLivros[i].codigo == id) return i;
    }
    return -1;
}

void salvarLivrosCSV(void) {
    int i;
    FILE *arquivo = abrirArquivo("livros.csv", "w");
    for (i = 0; i < totalLivros; i++) {
        fprintf(arquivo, "%d;%s;%s;%s;%.2f;%c;%d\n",
                acervoLivros[i].codigo, acervoLivros[i].titulo,
                acervoLivros[i].autor, acervoLivros[i].categoria,
                acervoLivros[i].valorReposicao,
                acervoLivros[i].statusDisponivel, acervoLivros[i].ativo);
    }
    fecharArquivo(arquivo);
}

int carregarLivrosCSV(void) {
    char linha[512], extra;
    int numeroLinha = 0, lidos;
    struct Livro livro;
    FILE *arquivo = abrirArquivo("livros.csv", "r");
    totalLivros = 0;
    while (fgets(linha, sizeof linha, arquivo) != NULL) {
        numeroLinha++;
        /* As larguras deixam espaco para o '\0' no final de cada texto.
           O ultimo %c detecta qualquer conteudo extra depois dos 7 campos. */
        lidos = sscanf(linha, "%4d;%49[^;\r\n];%39[^;\r\n];%29[^;\r\n];%f;%c;%1d %c",
                       &livro.codigo, livro.titulo, livro.autor, livro.categoria,
                       &livro.valorReposicao, &livro.statusDisponivel,
                       &livro.ativo, &extra);
        if (lidos != 7 || (strchr(linha, '\n') == NULL && !feof(arquivo)) ||
            totalLivros >= MAX_REGISTROS || livro.codigo <= 0 ||
            buscarIndiceLivro(livro.codigo) != -1 ||
            strspn(livro.titulo, " \t") == strlen(livro.titulo) ||
            strspn(livro.autor, " \t") == strlen(livro.autor) ||
            strspn(livro.categoria, " \t") == strlen(livro.categoria) ||
            !(livro.valorReposicao >= 0 && livro.valorReposicao <= 1000000) ||
            (livro.statusDisponivel != 'D' && livro.statusDisponivel != 'E') ||
            livro.ativo != 1) {
            printf("[ERRO] livros.csv: linha %d invalida ou ID duplicado.\n", numeroLinha);
            fecharArquivo(arquivo);
            return 0;
        }
        acervoLivros[totalLivros] = livro;
        totalLivros++;
    }
    fecharArquivo(arquivo);
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
    livro.codigo = lerInteiro("Codigo do livro: ", 1, 9999);
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
    salvarLivrosCSV();
    printf("[OK] Livro cadastrado e CSV atualizado.\n");
}

void editarLivro(void) {
    int codigo = lerInteiro("Codigo do livro a editar: ", 1, 9999);
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
    salvarLivrosCSV();
    printf("[OK] Livro editado e CSV atualizado.\n");
}

void removerLivro(void) {
    int codigo = lerInteiro("Codigo do livro a remover: ", 1, 9999);
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
    salvarLivrosCSV();
    printf("[OK] Livro removido e CSV atualizado.\n");
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
