#ifndef TAVIN_H
#define TAVIN_H

/* Tavin: leitores, buscas textuais e ordenacao Bubble Sort. */

int buscarIndiceLeitor(int id) {
    int i;
    for (i = 0; i < totalLeitores; i++) {
        if (ficharioLeitores[i].codigoLeitor == id) return i;
    }
    return -1;
}

void salvarLeitoresCSV(void) {
    int i;
    FILE *arquivo = abrirArquivo("leitores.csv", "w");
    for (i = 0; i < totalLeitores; i++) {
        fprintf(arquivo, "%d;%s;%s;%c;%.2f;%d\n",
                ficharioLeitores[i].codigoLeitor, ficharioLeitores[i].nome,
                ficharioLeitores[i].telefone, ficharioLeitores[i].tipoAssociado,
                ficharioLeitores[i].saldoMultas, ficharioLeitores[i].ativo);
    }
    fecharArquivo(arquivo);
}

int carregarLeitoresCSV(void) {
    char linha[512], extra;
    int numeroLinha = 0, lidos;
    struct Leitor leitor;
    FILE *arquivo = abrirArquivo("leitores.csv", "r");
    totalLeitores = 0;
    while (fgets(linha, sizeof linha, arquivo) != NULL) {
        numeroLinha++;
        lidos = sscanf(linha, "%4d;%49[^;\r\n];%19[^;\r\n];%c;%f;%1d %c",
                       &leitor.codigoLeitor, leitor.nome, leitor.telefone,
                       &leitor.tipoAssociado, &leitor.saldoMultas,
                       &leitor.ativo, &extra);
        if (lidos != 6 || (strchr(linha, '\n') == NULL && !feof(arquivo)) ||
            totalLeitores >= MAX_REGISTROS || leitor.codigoLeitor <= 0 ||
            buscarIndiceLeitor(leitor.codigoLeitor) != -1 ||
            strspn(leitor.nome, " \t") == strlen(leitor.nome) ||
            strspn(leitor.telefone, " \t") == strlen(leitor.telefone) ||
            (leitor.tipoAssociado != 'P' && leitor.tipoAssociado != 'V') ||
            !(leitor.saldoMultas >= 0 && leitor.saldoMultas <= 1000000) ||
            leitor.ativo != 1) {
            printf("[ERRO] leitores.csv: linha %d invalida ou ID duplicado.\n", numeroLinha);
            fecharArquivo(arquivo);
            return 0;
        }
        ficharioLeitores[totalLeitores] = leitor;
        totalLeitores++;
    }
    fecharArquivo(arquivo);
    return 1;
}

void exibirLeitor(int indice) {
    struct Leitor leitor = ficharioLeitores[indice];
    printf("ID: %d | %s | Tel.: %s | Tipo: %c | Multas: R$ %.2f\n",
           leitor.codigoLeitor, leitor.nome, leitor.telefone,
           leitor.tipoAssociado, leitor.saldoMultas);
}

void listarLeitores(void) {
    int i;
    printf("\n--- FICHARIO: %d leitor(es) ---\n", totalLeitores);
    for (i = 0; i < totalLeitores; i++) exibirLeitor(i);
    if (totalLeitores == 0) printf("Nenhum leitor cadastrado.\n");
}

void buscarLeitor(void) {
    char termo[50];
    int i, encontrados = 0;
    lerTexto("Nome ou parte do telefone do leitor: ", termo, sizeof termo);
    for (i = 0; i < totalLeitores; i++) {
        if (strstr(ficharioLeitores[i].nome, termo) != NULL ||
            strstr(ficharioLeitores[i].telefone, termo) != NULL) {
            exibirLeitor(i);
            encontrados++;
        }
    }
    if (encontrados == 0) printf("Nenhum leitor encontrado.\n");
}

void cadastrarLeitor(void) {
    struct Leitor leitor;
    if (totalLeitores >= MAX_REGISTROS) {
        printf("[ERRO] Fichario lotado: limite de %d leitores.\n", MAX_REGISTROS);
        return;
    }
    leitor.codigoLeitor = lerInteiro("Codigo do leitor: ", 1, 9999);
    if (buscarIndiceLeitor(leitor.codigoLeitor) != -1) {
        printf("[ERRO] Ja existe um leitor com esse codigo.\n");
        return;
    }
    lerTexto("Nome: ", leitor.nome, sizeof leitor.nome);
    lerTexto("Telefone: ", leitor.telefone, sizeof leitor.telefone);
    leitor.tipoAssociado = lerCaractere("Tipo (P=Padrao, V=VIP): ", "PV");
    leitor.saldoMultas = 0;
    leitor.ativo = 1;
    ficharioLeitores[totalLeitores++] = leitor;
    salvarLeitoresCSV();
    printf("[OK] Leitor cadastrado e CSV atualizado.\n");
}

void editarLeitor(void) {
    int codigo = lerInteiro("Codigo do leitor a editar: ", 1, 9999);
    int indice = buscarIndiceLeitor(codigo);
    struct Leitor leitor;
    if (indice == -1) {
        printf("[ERRO] Leitor nao encontrado.\n");
        return;
    }
    leitor = ficharioLeitores[indice];
    exibirLeitor(indice);
    lerTexto("Novo nome: ", leitor.nome, sizeof leitor.nome);
    lerTexto("Novo telefone: ", leitor.telefone, sizeof leitor.telefone);
    leitor.tipoAssociado = lerCaractere("Novo tipo (P=Padrao, V=VIP): ", "PV");
    ficharioLeitores[indice] = leitor;
    atualizarNomesEmprestimos();
    salvarLeitoresCSV();
    salvarEmprestimosCSV();
    printf("[OK] Leitor editado e CSV atualizado.\n");
}

void removerLeitor(void) {
    int codigo = lerInteiro("Codigo do leitor a remover: ", 1, 9999);
    int indice = buscarIndiceLeitor(codigo);
    int i;
    if (indice == -1) {
        printf("[ERRO] Leitor nao encontrado.\n");
        return;
    }
    if (leitorTemEmprestimo(codigo) || ficharioLeitores[indice].saldoMultas > 0) {
        printf("[ERRO] Leitor possui emprestimos no historico ou multas pendentes.\n");
        return;
    }
    for (i = indice; i < totalLeitores - 1; i++) ficharioLeitores[i] = ficharioLeitores[i + 1];
    totalLeitores--;
    salvarLeitoresCSV();
    printf("[OK] Leitor removido e CSV atualizado.\n");
}

void buscarLivroPorCodigoOuTitulo(void) {
    int opcao = lerInteiro("Buscar livro por 1=Codigo ou 2=Titulo: ", 1, 2);
    int i, encontrados = 0;
    if (opcao == 1) {
        int codigo = lerInteiro("Codigo do livro: ", 1, 9999);
        i = buscarIndiceLivro(codigo);
        if (i != -1) {
            exibirLivro(i);
            encontrados++;
        }
    } else {
        char termo[50];
        lerTexto("Titulo ou parte do titulo: ", termo, sizeof termo);
        for (i = 0; i < totalLivros; i++) {
            if (strstr(acervoLivros[i].titulo, termo) != NULL) {
                exibirLivro(i);
                encontrados++;
            }
        }
    }
    if (encontrados == 0) printf("Nenhum livro encontrado.\n");
}

void buscarLivrosPorAutor(void) {
    char termo[40];
    int i, encontrados = 0;
    lerTexto("Autor ou parte do nome: ", termo, sizeof termo);
    for (i = 0; i < totalLivros; i++) {
        if (strstr(acervoLivros[i].autor, termo) != NULL) {
            exibirLivro(i);
            encontrados++;
        }
    }
    if (encontrados == 0) printf("Nenhum livro encontrado.\n");
}

void ordenarAcervoBubbleSort(void) {
    int passagem, i, comparacao;
    struct Livro auxiliar;
    /* Compara vizinhos e troca a struct inteira: campos continuam juntos. */
    for (passagem = 0; passagem < totalLivros - 1; passagem++) {
        for (i = 0; i < totalLivros - 1 - passagem; i++) {
            comparacao = strcmp(acervoLivros[i].titulo, acervoLivros[i + 1].titulo);
            if (comparacao > 0 ||
                (comparacao == 0 && acervoLivros[i].codigo > acervoLivros[i + 1].codigo)) {
                auxiliar = acervoLivros[i];
                acervoLivros[i] = acervoLivros[i + 1];
                acervoLivros[i + 1] = auxiliar;
            }
        }
    }
    salvarLivrosCSV();
    printf("[OK] Acervo ordenado por titulo com Bubble Sort e salvo no CSV.\n");
    listarLivros();
}

void moduloBuscasEFichario(void) {
    int opcao;
    do {
        limparTela();
        printf("\n--- LEITORES, BUSCAS E ORDENACAO (Tavin) ---\n");
        printf("1. Cadastrar leitor\n2. Listar leitores\n3. Editar leitor\n");
        printf("4. Remover leitor\n5. Buscar livro por codigo ou titulo\n");
        printf("6. Buscar livros por autor\n7. Ordenar acervo (Bubble Sort)\n");
        printf("8. Buscar leitor por nome ou telefone\n0. Voltar\n");
        opcao = lerInteiro("Escolha: ", 0, 8);
        switch (opcao) {
            case 1: cadastrarLeitor(); break;
            case 2: listarLeitores(); break;
            case 3: editarLeitor(); break;
            case 4: removerLeitor(); break;
            case 5: buscarLivroPorCodigoOuTitulo(); break;
            case 6: buscarLivrosPorAutor(); break;
            case 7: ordenarAcervoBubbleSort(); break;
            case 8: buscarLeitor(); break;
            case 0: break;
        }
        if (opcao != 0) pausar();
    } while (opcao != 0);
}

#endif
