#ifndef KAUE_H
#define KAUE_H

/* Kaue: emprestimos/devolucoes. Os IDs dos cadastros ligam os tres vetores. */
int buscarIndiceEmprestimo(int id) {
    int i;
    for (i = 0; i < totalEmprestimos; i++) {
        if (listaEmprestimos[i].idEmprestimo == id) return i;
    }
    return -1;
}

int livroTemEmprestimo(int codigo) {
    int i;
    for (i = 0; i < totalEmprestimos; i++) {
        if (listaEmprestimos[i].codigoLivro == codigo) return 1;
    }
    return 0;
}

int leitorTemEmprestimo(int codigo) {
    int i;
    for (i = 0; i < totalEmprestimos; i++) {
        if (listaEmprestimos[i].codigoLeitor == codigo) return 1;
    }
    return 0;
}

int livroPossuiEmprestimoAtivo(int codigo) {
    int i;
    for (i = 0; i < totalEmprestimos; i++) {
        if (listaEmprestimos[i].codigoLivro == codigo && listaEmprestimos[i].situacao == 'A') return 1;
    }
    return 0;
}

void atualizarNomesEmprestimos(void) {
    int i, leitor;
    for (i = 0; i < totalEmprestimos; i++) {
        leitor = buscarIndiceLeitor(listaEmprestimos[i].codigoLeitor);
        if (leitor != -1) strcpy(listaEmprestimos[i].nomeLeitor, ficharioLeitores[leitor].nome);
    }
}

void salvarEmprestimosCSV(void) {
    int i;
    FILE *arquivo = abrirArquivo("emprestimos.csv", "w");
    for (i = 0; i < totalEmprestimos; i++) {
        struct Emprestimo e = listaEmprestimos[i];
        fprintf(arquivo, "%d;%d;%d;%s;%d;%.2f;%c\n", e.idEmprestimo,
                e.codigoLivro, e.codigoLeitor, e.nomeLeitor,
                e.diasPrazo, e.taxaAtrasoDiaria, e.situacao);
    }
    fecharArquivo(arquivo);
}

int carregarEmprestimosCSV(void) {
    char linha[512], extra;
    int numeroLinha = 0, livro, leitor, lidos;
    struct Emprestimo e;
    FILE *arquivo = fopen("emprestimos.csv", "r");
    if (arquivo == NULL) {
        printf("[ERRO] Nao foi possivel ler emprestimos.csv.\n");
        return 0;
    }
    totalEmprestimos = 0;
    while (fgets(linha, sizeof linha, arquivo) != NULL) {
        numeroLinha++;
        /* O ultimo %c so le se houver lixo depois dos sete campos. */
        lidos = sscanf(linha, "%4d;%4d;%4d;%49[^;\r\n];%3d;%f;%c %c",
                       &e.idEmprestimo, &e.codigoLivro, &e.codigoLeitor,
                       e.nomeLeitor, &e.diasPrazo, &e.taxaAtrasoDiaria,
                       &e.situacao, &extra);
        if ((strchr(linha, '\n') == NULL && !feof(arquivo)) ||
            totalEmprestimos >= MAX_REGISTROS || lidos != 7 ||
            e.idEmprestimo <= 0 || e.codigoLivro <= 0 || e.codigoLeitor <= 0 ||
            buscarIndiceEmprestimo(e.idEmprestimo) != -1 ||
            e.diasPrazo < 1 || e.diasPrazo > 365 ||
            !(e.taxaAtrasoDiaria >= 0 && e.taxaAtrasoDiaria <= 1000) ||
            (e.situacao != 'A' && e.situacao != 'F')) {
            printf("[ERRO] emprestimos.csv: registro invalido na linha %d.\n", numeroLinha);
            fecharArquivo(arquivo);
            return 0;
        }
        livro = buscarIndiceLivro(e.codigoLivro);
        leitor = buscarIndiceLeitor(e.codigoLeitor);
        if (livro == -1 || leitor == -1 || !acervoLivros[livro].ativo ||
            !ficharioLeitores[leitor].ativo ||
            strcmp(e.nomeLeitor, ficharioLeitores[leitor].nome) != 0 ||
            (e.situacao == 'A' && livroPossuiEmprestimoAtivo(e.codigoLivro))) {
            printf("[ERRO] emprestimos.csv: vinculos inconsistentes na linha %d.\n", numeroLinha);
            fecharArquivo(arquivo);
            return 0;
        }
        listaEmprestimos[totalEmprestimos++] = e;
    }
    fecharArquivo(arquivo);
    return 1;
}

void exibirEmprestimo(int indice) {
    struct Emprestimo e = listaEmprestimos[indice];
    int livro = buscarIndiceLivro(e.codigoLivro);
    printf("ID: %d | Livro: %d - %s | Leitor: %d - %s\n",
           e.idEmprestimo, e.codigoLivro,
           livro != -1 ? acervoLivros[livro].titulo : "Livro ausente",
           e.codigoLeitor, e.nomeLeitor);
    printf("Prazo: %d dias | Multa diaria: R$ %.2f | Situacao: %c (%s)\n",
           e.diasPrazo, e.taxaAtrasoDiaria, e.situacao,
           e.situacao == 'A' ? "ativo" : "finalizado");
}

void cadastrarEmprestimo(void) {
    struct Emprestimo e;
    int livro, leitor;
    if (totalEmprestimos >= MAX_REGISTROS) {
        printf("[ERRO] Limite de %d emprestimos atingido.\n", MAX_REGISTROS);
        return;
    }
    e.idEmprestimo = lerInteiro("ID do novo emprestimo: ", 1, 9999);
    if (buscarIndiceEmprestimo(e.idEmprestimo) != -1) {
        printf("[ERRO] ID de emprestimo ja cadastrado.\n");
        return;
    }
    e.codigoLivro = lerInteiro("Codigo do livro: ", 1, 9999);
    livro = buscarIndiceLivro(e.codigoLivro);
    if (livro == -1 || !acervoLivros[livro].ativo) {
        printf("[ERRO] Livro inexistente ou inativo.\n");
        return;
    }
    if (acervoLivros[livro].statusDisponivel != 'D' || livroPossuiEmprestimoAtivo(e.codigoLivro)) {
        printf("[ERRO] Livro indisponivel: ja esta emprestado.\n");
        return;
    }
    e.codigoLeitor = lerInteiro("Codigo do leitor: ", 1, 9999);
    leitor = buscarIndiceLeitor(e.codigoLeitor);
    if (leitor == -1 || !ficharioLeitores[leitor].ativo) {
        printf("[ERRO] Leitor inexistente ou inativo.\n");
        return;
    }
    strcpy(e.nomeLeitor, ficharioLeitores[leitor].nome);
    e.diasPrazo = lerInteiro("Prazo de devolucao (dias, 1 a 365): ", 1, 365);
    e.taxaAtrasoDiaria = lerReal("Multa por dia de atraso (R$, 0 a 1000): ", 0.0f, 1000.0f);
    e.situacao = 'A';
    listaEmprestimos[totalEmprestimos++] = e;
    acervoLivros[livro].statusDisponivel = 'E';
    salvarLivrosCSV();
    salvarEmprestimosCSV();
    printf("[OK] Emprestimo cadastrado; livro marcado como emprestado.\n");
}

void listarEmprestimos(void) {
    int i;
    printf("\n--- TODOS OS EMPRESTIMOS (%d) ---\n", totalEmprestimos);
    for (i = 0; i < totalEmprestimos; i++) exibirEmprestimo(i);
    if (totalEmprestimos == 0) printf("Nenhum emprestimo cadastrado.\n");
}

void listarEmprestimosAtivos(void) {
    int i, encontrados = 0;
    printf("\n--- EMPRESTIMOS ATIVOS ---\n");
    for (i = 0; i < totalEmprestimos; i++) {
        if (listaEmprestimos[i].situacao == 'A') {
            exibirEmprestimo(i);
            encontrados++;
        }
    }
    printf("Total de emprestimos ativos: %d.\n", encontrados);
}

void buscarEmprestimo(void) {
    char termo[50];
    int modo, i, livro, indice, encontrados = 0;
    modo = lerInteiro("Buscar por 1. ID ou 2. Nome do leitor/titulo do livro: ", 1, 2);
    if (modo == 1) {
        indice = buscarIndiceEmprestimo(lerInteiro("ID procurado: ", 1, 9999));
        if (indice != -1) exibirEmprestimo(indice);
        else printf("Nenhum emprestimo encontrado.\n");
        return;
    }
    lerTexto("Parte do nome do leitor ou titulo do livro: ", termo, sizeof termo);
    for (i = 0; i < totalEmprestimos; i++) {
        livro = buscarIndiceLivro(listaEmprestimos[i].codigoLivro);
        if (strstr(listaEmprestimos[i].nomeLeitor, termo) != NULL ||
            (livro != -1 && strstr(acervoLivros[livro].titulo, termo) != NULL)) {
            exibirEmprestimo(i);
            encontrados++;
        }
    }
    printf("Emprestimos encontrados: %d.\n", encontrados);
}

void editarEmprestimo(void) {
    int indice;
    indice = buscarIndiceEmprestimo(lerInteiro("ID do emprestimo a renovar/editar: ", 1, 9999));
    if (indice == -1) {
        printf("[ERRO] Emprestimo nao encontrado.\n");
        return;
    }
    if (listaEmprestimos[indice].situacao != 'A') {
        printf("[ERRO] Somente emprestimos ativos podem ser renovados.\n");
        return;
    }
    exibirEmprestimo(indice);
    printf("O novo prazo corresponde ao TOTAL de dias permitidos desde o emprestimo.\n");
    listaEmprestimos[indice].diasPrazo = lerInteiro("Novo prazo total (dias, 1 a 365): ", 1, 365);
    listaEmprestimos[indice].taxaAtrasoDiaria = lerReal("Nova multa diaria (R$, 0 a 1000): ", 0.0f, 1000.0f);
    salvarEmprestimosCSV();
    printf("[OK] Emprestimo atualizado na memoria e no CSV.\n");
}

void devolverEmprestimo(void) {
    int indice, livro, leitor, diasDecorridos, diasAtraso;
    float multa, novoSaldo;
    indice = buscarIndiceEmprestimo(lerInteiro("ID do emprestimo a devolver: ", 1, 9999));
    if (indice == -1) {
        printf("[ERRO] Emprestimo nao encontrado.\n");
        return;
    }
    if (listaEmprestimos[indice].situacao != 'A') {
        printf("[ERRO] Este emprestimo ja foi devolvido.\n");
        return;
    }
    livro = buscarIndiceLivro(listaEmprestimos[indice].codigoLivro);
    leitor = buscarIndiceLeitor(listaEmprestimos[indice].codigoLeitor);
    if (livro == -1 || leitor == -1) {
        printf("[ERRO] Vinculo do emprestimo com livro/leitor invalido.\n");
        return;
    }
    exibirEmprestimo(indice);
    diasDecorridos = lerInteiro("Total de dias decorridos desde o emprestimo (0 a 36500): ", 0, 36500);
    diasAtraso = diasDecorridos - listaEmprestimos[indice].diasPrazo;
    if (diasAtraso < 0) diasAtraso = 0;
    multa = diasAtraso * listaEmprestimos[indice].taxaAtrasoDiaria;
    novoSaldo = ficharioLeitores[leitor].saldoMultas + multa;
    if (!(novoSaldo >= 0 && novoSaldo <= 1000000.0f)) {
        printf("[ERRO] A devolucao excederia o saldo maximo de multas de R$ 1000000.00.\n");
        return;
    }
    listaEmprestimos[indice].situacao = 'F';
    acervoLivros[livro].statusDisponivel = 'D';
    ficharioLeitores[leitor].saldoMultas = novoSaldo;
    salvarLivrosCSV();
    salvarLeitoresCSV();
    salvarEmprestimosCSV();
    printf("[OK] Devolucao registrada; livro disponivel. Atraso: %d dia(s). Multa: R$ %.2f.\n",
           diasAtraso, multa);
    printf("Saldo de multas do leitor: R$ %.2f.\n", novoSaldo);
}

void removerEmprestimo(void) {
    int indice, i;
    indice = buscarIndiceEmprestimo(lerInteiro("ID do emprestimo a remover: ", 1, 9999));
    if (indice == -1) {
        printf("[ERRO] Emprestimo nao encontrado.\n");
        return;
    }
    if (listaEmprestimos[indice].situacao == 'A') {
        printf("[ERRO] Devolva o livro antes de remover o emprestimo.\n");
        return;
    }
    for (i = indice; i < totalEmprestimos - 1; i++) listaEmprestimos[i] = listaEmprestimos[i + 1];
    totalEmprestimos--;
    salvarEmprestimosCSV();
    printf("[OK] Historico removido do CSV; multas existentes foram preservadas.\n");
}

void moduloEmprestimos(void) {
    int opcao;
    do {
        limparTela();
        printf("\n--- MODULO 3: EMPRESTIMOS E DEVOLUCOES (Kaue) ---\n");
        printf("1. Registrar novo emprestimo\n");
        printf("2. Listar emprestimos ativos\n");
        printf("3. Editar / renovar emprestimo\n");
        printf("4. Registrar devolucao\n");
        printf("5. Buscar emprestimo\n");
        printf("6. Remover emprestimo finalizado\n");
        printf("7. Listar todos os emprestimos\n");
        printf("0. Voltar ao menu principal\n");
        opcao = lerInteiro("Escolha uma acao: ", 0, 7);
        switch (opcao) {
            case 1: cadastrarEmprestimo(); break;
            case 2: listarEmprestimosAtivos(); break;
            case 3: editarEmprestimo(); break;
            case 4: devolverEmprestimo(); break;
            case 5: buscarEmprestimo(); break;
            case 6: removerEmprestimo(); break;
            case 7: listarEmprestimos(); break;
            case 0: break;
        }
        if (opcao != 0) pausar();
    } while (opcao != 0);
}

#endif
