#ifndef JOELSON_H
#define JOELSON_H

#include "comum.h"

/* Joelson: pedidos de aquisicao/reserva, relatorios e matriz 2D. */
int buscarIndiceReserva(int id) {
    int i;
    for (i = 0; i < totalReservas; i++) {
        if (listaReservas[i].idReserva == id) return i;
    }
    return -1;
}

int salvarReservasCSV(void) {
    int i;
    FILE *arquivo = fopen("reservas.csv.tmp", "w");
    if (arquivo == NULL) {
        printf("[ERRO] Nao foi possivel abrir reservas.csv.tmp.\n");
        return 0;
    }
    for (i = 0; i < totalReservas; i++) {
        fprintf(arquivo, "%d;%s;%s;%.2f;%c;%d\n",
                listaReservas[i].idReserva,
                listaReservas[i].tituloDesejado,
                listaReservas[i].categoriaObra,
                listaReservas[i].custoEstimado,
                listaReservas[i].prioridade,
                listaReservas[i].atendida);
    }
    return fecharCSV(arquivo, "reservas.csv.tmp", "reservas.csv");
}

int carregarReservasCSV(void) {
    char linha[512];
    char *campos[6];
    int numeroLinha = 0;
    struct Reserva reserva;
    FILE *arquivo = fopen("reservas.csv", "r");
    if (arquivo == NULL) {
        printf("[ERRO] Nao foi possivel abrir reservas.csv.\n");
        return 0;
    }
    totalReservas = 0;
    while (fgets(linha, sizeof linha, arquivo) != NULL) {
        numeroLinha++;
        if ((strchr(linha, '\n') == NULL && !feof(arquivo)) ||
            totalReservas >= MAX_REGISTROS ||
            !separarCamposCSV(linha, campos, 6) ||
            !converterInteiro(campos[0], 1, INT_MAX, &reserva.idReserva) ||
            buscarIndiceReserva(reserva.idReserva) != -1 ||
            !copiarCampo(reserva.tituloDesejado, sizeof reserva.tituloDesejado, campos[1]) ||
            !copiarCampo(reserva.categoriaObra, sizeof reserva.categoriaObra, campos[2]) ||
            !converterReal(campos[3], 0.0f, 1000000.0f, &reserva.custoEstimado) ||
            strlen(campos[4]) != 1 || strchr("AMB", campos[4][0]) == NULL ||
            !converterInteiro(campos[5], 0, 1, &reserva.atendida)) {
            printf("[ERRO] reservas.csv: registro invalido na linha %d.\n", numeroLinha);
            fclose(arquivo);
            return 0;
        }
        reserva.prioridade = campos[4][0];
        listaReservas[totalReservas] = reserva;
        totalReservas++;
    }
    if (ferror(arquivo)) {
        printf("[ERRO] Falha durante a leitura de reservas.csv.\n");
        fclose(arquivo);
        return 0;
    }
    fclose(arquivo);
    return 1;
}

void exibirReserva(struct Reserva reserva) {
    printf("ID: %d | %s | Categoria: %s | R$ %.2f | Prioridade: %c | %s\n",
           reserva.idReserva, reserva.tituloDesejado, reserva.categoriaObra,
           reserva.custoEstimado, reserva.prioridade,
           reserva.atendida ? "Atendida" : "Pendente");
}

void preencherDadosReserva(struct Reserva *reserva) {
    lerTexto("Titulo desejado: ", reserva->tituloDesejado, sizeof reserva->tituloDesejado);
    lerTexto("Categoria da obra: ", reserva->categoriaObra, sizeof reserva->categoriaObra);
    reserva->custoEstimado = lerReal("Custo estimado (R$): ", 0.0f, 1000000.0f);
    reserva->prioridade = lerCaractere("Prioridade (A=Alta, M=Media, B=Baixa): ", "AMB");
    reserva->atendida = lerInteiro("Atendida? (0=Nao, 1=Sim): ", 0, 1);
}

void cadastrarReserva(void) {
    struct Reserva nova;
    if (totalReservas >= MAX_REGISTROS) {
        printf("[ERRO] Limite de %d reservas atingido.\n", MAX_REGISTROS);
        return;
    }
    nova.idReserva = lerInteiro("ID da nova reserva: ", 1, INT_MAX);
    if (buscarIndiceReserva(nova.idReserva) != -1) {
        printf("[ERRO] ID de reserva ja cadastrado.\n");
        return;
    }
    preencherDadosReserva(&nova);
    listaReservas[totalReservas] = nova;
    totalReservas++;
    if (salvarEstadoCSV()) printf("[OK] Reserva cadastrada e salva no CSV.\n");
}

void listarReservas(void) {
    int i;
    printf("\n--- PEDIDOS DE RESERVA / AQUISICAO (%d) ---\n", totalReservas);
    if (totalReservas == 0) printf("Nenhum pedido cadastrado.\n");
    for (i = 0; i < totalReservas; i++) exibirReserva(listaReservas[i]);
}

void buscarReservas(void) {
    char termo[50];
    int i, encontrados = 0;
    lerTexto("Parte do titulo ou categoria (diferencia maiusculas): ", termo, sizeof termo);
    for (i = 0; i < totalReservas; i++) {
        if (strstr(listaReservas[i].tituloDesejado, termo) != NULL ||
            strstr(listaReservas[i].categoriaObra, termo) != NULL) {
            exibirReserva(listaReservas[i]);
            encontrados++;
        }
    }
    if (encontrados == 0) printf("Nenhuma reserva encontrada.\n");
}

void editarReserva(void) {
    int id = lerInteiro("ID da reserva a editar: ", 1, INT_MAX);
    int posicao = buscarIndiceReserva(id);
    struct Reserva editada;
    if (posicao == -1) {
        printf("[ERRO] Reserva nao encontrada.\n");
        return;
    }
    exibirReserva(listaReservas[posicao]);
    editada = listaReservas[posicao];
    preencherDadosReserva(&editada);
    listaReservas[posicao] = editada;
    if (salvarEstadoCSV()) printf("[OK] Reserva editada e salva no CSV.\n");
}

void removerReserva(void) {
    int i;
    int id = lerInteiro("ID da reserva a remover: ", 1, INT_MAX);
    int posicao = buscarIndiceReserva(id);
    if (posicao == -1) {
        printf("[ERRO] Reserva nao encontrada.\n");
        return;
    }
    for (i = posicao; i < totalReservas - 1; i++) {
        listaReservas[i] = listaReservas[i + 1];
    }
    totalReservas--;
    if (salvarEstadoCSV()) printf("[OK] Reserva removida da memoria e do CSV.\n");
}

void relatorioGeral(void) {
    int i, livros = 0, disponiveis = 0, emprestados = 0, leitores = 0;
    int ativos = 0, finalizados = 0, pendentes = 0, atendidas = 0;
    int alta = 0, media = 0, baixa = 0;
    double valorAcervo = 0.0, multas = 0.0, custoPendente = 0.0;
    double ocupacao, valorMedio;
    for (i = 0; i < totalLivros; i++) {
        if (!acervoLivros[i].ativo) continue;
        livros++;
        valorAcervo += acervoLivros[i].valorReposicao;
        if (acervoLivros[i].statusDisponivel == 'D') disponiveis++;
        if (acervoLivros[i].statusDisponivel == 'E') emprestados++;
    }
    for (i = 0; i < totalLeitores; i++) {
        if (ficharioLeitores[i].ativo) {
            leitores++;
            multas += ficharioLeitores[i].saldoMultas;
        }
    }
    for (i = 0; i < totalEmprestimos; i++) {
        if (listaEmprestimos[i].situacao == 'A') ativos++;
        if (listaEmprestimos[i].situacao == 'F') finalizados++;
    }
    for (i = 0; i < totalReservas; i++) {
        if (listaReservas[i].atendida) {
            atendidas++;
        } else {
            pendentes++;
            custoPendente += listaReservas[i].custoEstimado;
            if (listaReservas[i].prioridade == 'A') alta++;
            if (listaReservas[i].prioridade == 'M') media++;
            if (listaReservas[i].prioridade == 'B') baixa++;
        }
    }
    ocupacao = livros > 0 ? 100.0 * emprestados / livros : 0.0;
    valorMedio = livros > 0 ? valorAcervo / livros : 0.0;
    printf("\n--- RESUMO ANALITICO DA BIBLIOTECA ---\n");
    printf("Livros ativos: %d | Disponiveis: %d | Emprestados: %d\n", livros, disponiveis, emprestados);
    printf("Taxa de ocupacao do acervo: %.2f%%\n", ocupacao);
    printf("Valor total de reposicao: R$ %.2f | Valor medio: R$ %.2f\n", valorAcervo, valorMedio);
    printf("Leitores ativos: %d | Multas pendentes: R$ %.2f\n", leitores, multas);
    printf("Emprestimos ativos: %d | Finalizados: %d | Total: %d\n", ativos, finalizados, totalEmprestimos);
    printf("Reservas pendentes: %d | Atendidas: %d | Total: %d\n", pendentes, atendidas, totalReservas);
    printf("Custo total das aquisicoes pendentes: R$ %.2f\n", custoPendente);
    printf("Prioridades pendentes: Alta=%d | Media=%d | Baixa=%d\n", alta, media, baixa);
}

int categoriaDaMatriz(const char *categoria) {
    if (strcmp(categoria, "Romance") == 0) return 0;
    if (strcmp(categoria, "Tecnico") == 0) return 1;
    if (strcmp(categoria, "Ficcao") == 0) return 2;
    return 3;
}

void relatorioMatriz(void) {
    const char *categorias[4] = {"Romance", "Tecnico", "Ficcao", "Outras"};
    int linha, coluna, i, totalDisponiveis = 0, totalEmprestados = 0;
    /* Cada livro contribui com uma unidade em uma unica celula. */
    for (linha = 0; linha < 4; linha++) {
        for (coluna = 0; coluna < 2; coluna++) matrizEstatisticas[linha][coluna] = 0;
    }
    for (i = 0; i < totalLivros; i++) {
        if (!acervoLivros[i].ativo) continue;
        linha = categoriaDaMatriz(acervoLivros[i].categoria);
        coluna = acervoLivros[i].statusDisponivel == 'E' ? 1 : 0;
        matrizEstatisticas[linha][coluna]++;
    }
    printf("\n--- MATRIZ [4 CATEGORIAS][2 STATUS] ---\n");
    printf("%-12s %12s %12s %8s\n", "Categoria", "Disponiveis", "Emprestados", "Total");
    for (linha = 0; linha < 4; linha++) {
        printf("%-12s %12d %12d %8d\n", categorias[linha],
               matrizEstatisticas[linha][0], matrizEstatisticas[linha][1],
               matrizEstatisticas[linha][0] + matrizEstatisticas[linha][1]);
        totalDisponiveis += matrizEstatisticas[linha][0];
        totalEmprestados += matrizEstatisticas[linha][1];
    }
    printf("%-12s %12d %12d %8d\n", "TOTAL", totalDisponiveis, totalEmprestados,
           totalDisponiveis + totalEmprestados);
    printf("Outras inclui toda categoria diferente de Romance, Tecnico e Ficcao.\n");
}

void moduloRelatoriosEReservas(void) {
    int opcao;
    do {
        limparTela();
        printf("\n--- MODULO 4: RESERVAS E RELATORIOS (Joelson) ---\n");
        printf("1. Cadastrar pedido de reserva / aquisicao\n");
        printf("2. Listar pedidos\n");
        printf("3. Editar pedido\n");
        printf("4. Remover pedido\n");
        printf("5. Relatorio geral\n");
        printf("6. Matriz por categoria x disponibilidade\n");
        printf("7. Buscar pedidos por titulo ou categoria\n");
        printf("0. Voltar\n");
        opcao = lerInteiro("Escolha: ", 0, 7);
        switch (opcao) {
            case 1: cadastrarReserva(); break;
            case 2: listarReservas(); break;
            case 3: editarReserva(); break;
            case 4: removerReserva(); break;
            case 5: relatorioGeral(); break;
            case 6: relatorioMatriz(); break;
            case 7: buscarReservas(); break;
            case 0: break;
        }
        if (opcao != 0) pausar();
    } while (opcao != 0);
}

#endif
