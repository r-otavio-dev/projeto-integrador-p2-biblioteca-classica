#include "comum.h"
#include "pedro.h"
#include "tavin.h"
#include "kaue.h"
#include "joelson.h"

/* Copia do ultimo estado salvo para desfazer uma operacao se o disco falhar. */
struct Estado {
    struct Livro livros[MAX_REGISTROS];
    struct Leitor leitores[MAX_REGISTROS];
    struct Emprestimo emprestimos[MAX_REGISTROS];
    struct Reserva reservas[MAX_REGISTROS];
    int livrosTotal, leitoresTotal, emprestimosTotal, reservasTotal;
};
struct Estado estadoConfirmado;
const char *arquivosCSV[4] = {"livros.csv", "leitores.csv", "emprestimos.csv", "reservas.csv"};

void memorizarEstado(void) {
    memcpy(estadoConfirmado.livros, acervoLivros, sizeof acervoLivros);
    memcpy(estadoConfirmado.leitores, ficharioLeitores, sizeof ficharioLeitores);
    memcpy(estadoConfirmado.emprestimos, listaEmprestimos, sizeof listaEmprestimos);
    memcpy(estadoConfirmado.reservas, listaReservas, sizeof listaReservas);
    estadoConfirmado.livrosTotal = totalLivros;
    estadoConfirmado.leitoresTotal = totalLeitores;
    estadoConfirmado.emprestimosTotal = totalEmprestimos;
    estadoConfirmado.reservasTotal = totalReservas;
}

void restaurarMemoria(void) {
    memcpy(acervoLivros, estadoConfirmado.livros, sizeof acervoLivros);
    memcpy(ficharioLeitores, estadoConfirmado.leitores, sizeof ficharioLeitores);
    memcpy(listaEmprestimos, estadoConfirmado.emprestimos, sizeof listaEmprestimos);
    memcpy(listaReservas, estadoConfirmado.reservas, sizeof listaReservas);
    totalLivros = estadoConfirmado.livrosTotal;
    totalLeitores = estadoConfirmado.leitoresTotal;
    totalEmprestimos = estadoConfirmado.emprestimosTotal;
    totalReservas = estadoConfirmado.reservasTotal;
}

int existeArquivo(const char *nome) {
    FILE *arquivo = fopen(nome, "r");
    if (arquivo == NULL) return 0;
    fclose(arquivo);
    return 1;
}

/* Cada modulo reescreve seu vetor com fopen("w"). Os .bak permitem desfazer
   uma falha entre a gravacao de um modulo e a de outro. Nao desligue o
   computador durante a gravacao; uma interrupcao exige recuperar os .bak. */
int salvarEstadoCSV(void) {
    char backups[4][64];
    int copiados = 0, ok = 1;
    int i;
    for (i = 0; i < 4; i++) {
        snprintf(backups[i], sizeof backups[i], "%s.bak", arquivosCSV[i]);
        if (existeArquivo(backups[i]) || rename(arquivosCSV[i], backups[i]) != 0) {
            printf("[ERRO] Nao foi possivel proteger %s antes da gravacao.\n", arquivosCSV[i]);
            ok = 0;
            break;
        }
        copiados++;
    }
    if (ok) ok = salvarLivrosCSV();
    if (ok) ok = salvarLeitoresCSV();
    if (ok) ok = salvarEmprestimosCSV();
    if (ok) ok = salvarReservasCSV();
    if (!ok) {
        int restaurado = 1;
        for (i = 0; i < copiados; i++) {
            if (rename(backups[i], arquivosCSV[i]) != 0) {
                /* No Windows, rename exige que o destino ainda nao exista. */
                remove(arquivosCSV[i]);
                if (rename(backups[i], arquivosCSV[i]) != 0) restaurado = 0;
            }
        }
        restaurarMemoria();
        if (!restaurado) {
            fprintf(stderr, "[ERRO] Recupere os CSVs a partir dos .bak antes de reabrir.\n");
            exit(EXIT_FAILURE);
        }
        printf("[ERRO] Operacao cancelada; dados anteriores preservados.\n");
        return 0;
    }
    for (i = 0; i < 4; i++) {
        if (remove(backups[i]) != 0) printf("[ALERTA] Remova o backup antigo %s antes da proxima gravacao.\n", backups[i]);
    }
    memorizarEstado();
    return 1;
}

/* Checagem dos relacionamentos: CSV inconsistente nunca e sobrescrito. */
int validarIntegracao(void) {
    int i, j;
    for (i = 0; i < totalEmprestimos; i++) {
        int livro = buscarIndiceLivro(listaEmprestimos[i].codigoLivro);
        int leitor = buscarIndiceLeitor(listaEmprestimos[i].codigoLeitor);
        if (livro < 0 || leitor < 0 ||
            !acervoLivros[livro].ativo || !ficharioLeitores[leitor].ativo ||
            strcmp(listaEmprestimos[i].nomeLeitor, ficharioLeitores[leitor].nome) != 0) {
            fprintf(stderr, "[ERRO] Emprestimo %d possui referencia ou nome de leitor inconsistente.\n", listaEmprestimos[i].idEmprestimo);
            return 0;
        }
    }
    for (i = 0; i < totalLivros; i++) {
        int ativos = 0;
        for (j = 0; j < totalEmprestimos; j++) {
            if (listaEmprestimos[j].codigoLivro == acervoLivros[i].codigo &&
                listaEmprestimos[j].situacao == 'A') ativos++;
        }
        if (ativos > 1 || acervoLivros[i].statusDisponivel != (ativos == 1 ? 'E' : 'D')) {
            fprintf(stderr, "[ERRO] Disponibilidade inconsistente no livro %d.\n", acervoLivros[i].codigo);
            return 0;
        }
    }
    return 1;
}

int carregarSistema(void) {
    int i;
    char backup[64];
    for (i = 0; i < 4; i++) {
        snprintf(backup, sizeof backup, "%s.bak", arquivosCSV[i]);
        if (existeArquivo(backup)) {
            fprintf(stderr, "[ERRO] Backup %s encontrado. Consulte a recuperacao no README.\n", backup);
            return 0;
        }
    }
    if (!carregarLivrosCSV() || !carregarLeitoresCSV() ||
        !carregarEmprestimosCSV() || !carregarReservasCSV() || !validarIntegracao()) return 0;
    memorizarEstado();
    return 1;
}

void exibirCabecalho(void) {
    printf("\n============================================================\n");
    printf(" BIBLIOTECA CLASSICA - CHECKPOINT 2\n");
    printf("============================================================\n");
    printf("1. Acervo de livros (Pedro)\n");
    printf("2. Leitores, buscas e Bubble Sort (Tavin)\n");
    printf("3. Emprestimos e devolucoes (Kaue)\n");
    printf("4. Reservas, relatorios e matriz (Joelson)\n");
    printf("0. Encerrar\n");
}

int main(int argc, char *argv[]) {
    int opcao;
    setlocale(LC_ALL, "");
    /* O ponto decimal do CSV permanece igual em computadores diferentes. */
    setlocale(LC_NUMERIC, "C");
    if (argc == 2 && strcmp(argv[1], "--sem-pausa") == 0) modoLote = 1;
    else if (argc != 1) {
        fprintf(stderr, "Uso: %s [--sem-pausa]\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (!carregarSistema()) {
        fprintf(stderr, "Inicializacao cancelada. Confira os CSVs na pasta atual.\n");
        return EXIT_FAILURE;
    }
    printf("Carregados: %d livros, %d leitores, %d emprestimos e %d reservas.\n",
           totalLivros, totalLeitores, totalEmprestimos, totalReservas);
    do {
        limparTela();
        exibirCabecalho();
        opcao = lerInteiro("Escolha: ", 0, 4);
        switch (opcao) {
            case 1: moduloLivros(); break;
            case 2: moduloBuscasEFichario(); break;
            case 3: moduloEmprestimos(); break;
            case 4: moduloRelatoriosEReservas(); break;
            case 0: printf("Sistema encerrado. Dados salvos a cada alteracao.\n"); break;
        }
    } while (opcao != 0);
    return EXIT_SUCCESS;
}
