#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "biblioteca.h"

int localizarReserva(int idReserva) {
    int i;

    for (i = 0; i < totalReservas; i++) {
        if (listaReservas[i].idReserva == idReserva) {
            return i;
        }
    }

    return -1;
}

static void cadastrarReserva(void) {
    Reserva novaReserva;

    if (totalReservas >= CAPACIDADE) {
        printf("Limite de reservas atingido.\n");
        return;
    }

    novaReserva.idReserva = lerInteiro("ID da reserva: ");
    if (localizarReserva(novaReserva.idReserva) != -1) {
        printf("Ja existe uma reserva com esse ID.\n");
        return;
    }

    lerTexto("Titulo desejado: ", novaReserva.tituloDesejado,
             sizeof(novaReserva.tituloDesejado));
    lerTexto("Categoria: ", novaReserva.categoriaObra,
             sizeof(novaReserva.categoriaObra));
    novaReserva.custoEstimado = lerFloat("Custo estimado: R$ ");
    novaReserva.prioridade = (char) toupper((unsigned char)
        lerCaractere("Prioridade (A = Alta, M = Media, B = Baixa): "));

    if (novaReserva.custoEstimado < 0 ||
        (novaReserva.prioridade != 'A' &&
         novaReserva.prioridade != 'M' &&
         novaReserva.prioridade != 'B')) {
        printf("Custo ou prioridade invalida.\n");
        return;
    }

    novaReserva.atendida = 0;
    listaReservas[totalReservas] = novaReserva;
    totalReservas++;

    printf("Reserva cadastrada com sucesso.\n");
}

static void listarReservas(void) {
    int i;

    if (totalReservas == 0) {
        printf("Nenhuma reserva cadastrada.\n");
        return;
    }

    for (i = 0; i < totalReservas; i++) {
        printf("ID: %d | Titulo: %s | Categoria: %s\n",
               listaReservas[i].idReserva,
               listaReservas[i].tituloDesejado,
               listaReservas[i].categoriaObra);
        printf("Custo: R$ %.2f | Prioridade: %c | Atendida: %s\n",
               listaReservas[i].custoEstimado,
               listaReservas[i].prioridade,
               listaReservas[i].atendida == 1 ? "Sim" : "Nao");
        printf("------------------------------------------------------------\n");
    }
}

static void editarReserva(void) {
    int id = lerInteiro("ID da reserva: ");
    int i = localizarReserva(id);
    int atendida;
    char prioridade;
    Reserva reservaEditada;

    if (i == -1) {
        printf("Reserva nao encontrada.\n");
        return;
    }

    reservaEditada = listaReservas[i];
    lerTexto("Novo titulo: ", reservaEditada.tituloDesejado,
             sizeof(reservaEditada.tituloDesejado));
    lerTexto("Nova categoria: ", reservaEditada.categoriaObra,
             sizeof(reservaEditada.categoriaObra));
    reservaEditada.custoEstimado = lerFloat("Novo custo: R$ ");
    prioridade = (char) toupper((unsigned char)
        lerCaractere("Nova prioridade (A, M ou B): "));
    atendida = lerInteiro("Foi atendida? (1 = Sim, 0 = Nao): ");

    if (reservaEditada.custoEstimado < 0 ||
        (prioridade != 'A' && prioridade != 'M' && prioridade != 'B') ||
        (atendida != 0 && atendida != 1)) {
        printf("Dados invalidos.\n");
        return;
    }

    reservaEditada.prioridade = prioridade;
    reservaEditada.atendida = atendida;
    listaReservas[i] = reservaEditada;
    printf("Reserva atualizada com sucesso.\n");
}

static void removerReserva(void) {
    int id = lerInteiro("ID da reserva: ");
    int i = localizarReserva(id);
    int j;

    if (i == -1) {
        printf("Reserva nao encontrada.\n");
        return;
    }

    for (j = i; j < totalReservas - 1; j++) {
        listaReservas[j] = listaReservas[j + 1];
    }
    totalReservas--;

    printf("Reserva removida com sucesso.\n");
}

static int contarLivrosDisponiveis(void) {
    int i;
    int quantidade = 0;

    for (i = 0; i < totalLivros; i++) {
        if (acervoLivros[i].ativo == 1 &&
            acervoLivros[i].statusDisponivel == 'D') {
            quantidade++;
        }
    }

    return quantidade;
}

static int contarEmprestimosAtivos(void) {
    int i;
    int quantidade = 0;

    for (i = 0; i < totalEmprestimos; i++) {
        if (listaEmprestimos[i].situacao == 'A') {
            quantidade++;
        }
    }

    return quantidade;
}

static int contarLeitoresAtivos(void) {
    int i;
    int quantidade = 0;

    for (i = 0; i < totalLeitores; i++) {
        if (ficharioLeitores[i].ativo == 1) {
            quantidade++;
        }
    }

    return quantidade;
}

static float somarMultasPendentes(void) {
    int i;
    float total = 0;

    for (i = 0; i < totalLeitores; i++) {
        if (ficharioLeitores[i].ativo == 1) {
            total += ficharioLeitores[i].saldoMultas;
        }
    }

    return total;
}

static float somarReservasPendentes(void) {
    int i;
    float total = 0;

    for (i = 0; i < totalReservas; i++) {
        if (listaReservas[i].atendida == 0) {
            total += listaReservas[i].custoEstimado;
        }
    }

    return total;
}

static void exibirRelatorioGeral(void) {
    int disponiveis = contarLivrosDisponiveis();
    int emprestados = contarEmprestimosAtivos();

    printf("\n--- RELATORIO GERAL ---\n");
    printf("Livros ativos: %d\n", disponiveis + emprestados);
    printf("Livros disponiveis: %d\n", disponiveis);
    printf("Livros emprestados: %d\n", emprestados);
    printf("Leitores ativos: %d\n", contarLeitoresAtivos());
    printf("Emprestimos ativos: %d\n", emprestados);
    printf("Multas pendentes: R$ %.2f\n", somarMultasPendentes());
    printf("Custo das reservas pendentes: R$ %.2f\n",
           somarReservasPendentes());
}

static int indiceCategoria(const char categoria[]) {
    if (strcmp(categoria, "Romance") == 0) {
        return 0;
    }
    if (strcmp(categoria, "Tecnico") == 0) {
        return 1;
    }
    if (strcmp(categoria, "Ficcao") == 0) {
        return 2;
    }

    return 3;
}

static void atualizarMatrizEstatisticas(void) {
    int i;
    int linha;
    int coluna;

    for (linha = 0; linha < CATEGORIAS; linha++) {
        for (coluna = 0; coluna < SITUACOES_LIVRO; coluna++) {
            matrizEstatisticas[linha][coluna] = 0;
        }
    }

    for (i = 0; i < totalLivros; i++) {
        if (acervoLivros[i].ativo == 1) {
            linha = indiceCategoria(acervoLivros[i].categoria);
            coluna = acervoLivros[i].statusDisponivel == 'E' ? 1 : 0;
            matrizEstatisticas[linha][coluna]++;
        }
    }
}

static void exibirMatrizEstatisticas(void) {
    const char *nomesCategorias[CATEGORIAS] = {
        "Romance", "Tecnico", "Ficcao", "Outras"
    };
    int i;

    atualizarMatrizEstatisticas();

    printf("\nCategoria       Disponiveis  Emprestados\n");
    for (i = 0; i < CATEGORIAS; i++) {
        printf("%-15s %-12d %d\n",
               nomesCategorias[i],
               matrizEstatisticas[i][0],
               matrizEstatisticas[i][1]);
    }
}

void moduloRelatoriosEReservas(void) {
    int opcao;

    do {
        printf("\n--- MODULO 4: RESERVAS E RELATORIOS (JOELSON) ---\n");
        printf("1. Cadastrar reserva ou aquisicao\n");
        printf("2. Listar reservas\n");
        printf("3. Editar reserva\n");
        printf("4. Remover reserva\n");
        printf("5. Exibir relatorio geral\n");
        printf("6. Exibir matriz de estatisticas\n");
        printf("0. Voltar\n");

        opcao = lerInteiro("Opcao: ");

        switch (opcao) {
            case 1:
                cadastrarReserva();
                break;
            case 2:
                listarReservas();
                break;
            case 3:
                editarReserva();
                break;
            case 4:
                removerReserva();
                break;
            case 5:
                exibirRelatorioGeral();
                break;
            case 6:
                exibirMatrizEstatisticas();
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}
