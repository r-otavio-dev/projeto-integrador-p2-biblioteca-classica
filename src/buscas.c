#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "biblioteca.h"

int localizarLeitor(int codigo) {
    int i;

    for (i = 0; i < totalLeitores; i++) {
        if (ficharioLeitores[i].codigoLeitor == codigo &&
            ficharioLeitores[i].ativo == 1) {
            return i;
        }
    }

    return -1;
}

static int codigoLeitorJaUsado(int codigo) {
    int i;

    for (i = 0; i < totalLeitores; i++) {
        if (ficharioLeitores[i].codigoLeitor == codigo) {
            return 1;
        }
    }

    return 0;
}

static void mostrarLeitor(int i) {
    printf("Carteirinha: %d | Nome: %s | Telefone: %s\n",
           ficharioLeitores[i].codigoLeitor,
           ficharioLeitores[i].nome,
           ficharioLeitores[i].telefone);
    printf("Tipo: %c | Multas: R$ %.2f | Ativo: %s\n",
           ficharioLeitores[i].tipoAssociado,
           ficharioLeitores[i].saldoMultas,
           ficharioLeitores[i].ativo == 1 ? "Sim" : "Nao");
}

static void mostrarLivro(int i) {
    printf("Codigo: %d | Titulo: %s | Autor: %s | Status: %c\n",
           acervoLivros[i].codigo,
           acervoLivros[i].titulo,
           acervoLivros[i].autor,
           acervoLivros[i].statusDisponivel);
}

static void cadastrarLeitor(void) {
    Leitor novoLeitor;

    if (totalLeitores >= CAPACIDADE) {
        printf("Limite de leitores atingido.\n");
        return;
    }

    novoLeitor.codigoLeitor = lerInteiro("Numero da carteirinha: ");
    if (codigoLeitorJaUsado(novoLeitor.codigoLeitor)) {
        printf("Ja existe um leitor com essa carteirinha.\n");
        return;
    }

    lerTexto("Nome completo: ", novoLeitor.nome, sizeof(novoLeitor.nome));
    lerTexto("Telefone: ", novoLeitor.telefone, sizeof(novoLeitor.telefone));
    novoLeitor.tipoAssociado = (char) toupper((unsigned char)
        lerCaractere("Tipo de associado (P = Padrao, V = VIP): "));

    if (novoLeitor.tipoAssociado != 'P' && novoLeitor.tipoAssociado != 'V') {
        printf("Tipo de associado invalido.\n");
        return;
    }

    novoLeitor.saldoMultas = 0;
    novoLeitor.ativo = 1;
    ficharioLeitores[totalLeitores] = novoLeitor;
    totalLeitores++;

    printf("Leitor cadastrado com sucesso.\n");
}

static void listarLeitores(void) {
    int i;
    int encontrou = 0;

    for (i = 0; i < totalLeitores; i++) {
        if (ficharioLeitores[i].ativo == 1) {
            mostrarLeitor(i);
            printf("------------------------------------------------------------\n");
            encontrou = 1;
        }
    }

    if (encontrou == 0) {
        printf("Nenhum leitor cadastrado.\n");
    }
}

static void buscarLeitor(void) {
    int opcao;
    int codigo;
    int i;
    int encontrou = 0;
    char nome[50];

    printf("1. Listar todos\n");
    printf("2. Buscar por carteirinha\n");
    printf("3. Buscar por nome\n");
    opcao = lerInteiro("Opcao: ");

    if (opcao == 1) {
        listarLeitores();
    } else if (opcao == 2) {
        codigo = lerInteiro("Numero da carteirinha: ");
        i = localizarLeitor(codigo);

        if (i == -1) {
            printf("Leitor nao encontrado.\n");
        } else {
            mostrarLeitor(i);
        }
    } else if (opcao == 3) {
        lerTexto("Nome ou parte do nome: ", nome, sizeof(nome));

        for (i = 0; i < totalLeitores; i++) {
            if (ficharioLeitores[i].ativo == 1 &&
                strstr(ficharioLeitores[i].nome, nome) != NULL) {
                mostrarLeitor(i);
                encontrou = 1;
            }
        }

        if (encontrou == 0) {
            printf("Leitor nao encontrado.\n");
        }
    } else {
        printf("Opcao invalida.\n");
    }
}

static void editarLeitor(void) {
    int codigo = lerInteiro("Carteirinha do leitor: ");
    int i = localizarLeitor(codigo);
    char tipo;
    Leitor leitorEditado;

    if (i == -1) {
        printf("Leitor nao encontrado.\n");
        return;
    }

    leitorEditado = ficharioLeitores[i];
    lerTexto("Novo nome: ", leitorEditado.nome,
             sizeof(leitorEditado.nome));
    lerTexto("Novo telefone: ", leitorEditado.telefone,
             sizeof(leitorEditado.telefone));
    tipo = (char) toupper((unsigned char)
        lerCaractere("Novo tipo (P = Padrao, V = VIP): "));

    if (tipo != 'P' && tipo != 'V') {
        printf("Tipo de associado invalido.\n");
        return;
    }

    leitorEditado.tipoAssociado = tipo;
    ficharioLeitores[i] = leitorEditado;
    printf("Leitor atualizado com sucesso.\n");
}

static int leitorPossuiEmprestimoAtivo(int codigoLeitor) {
    int i;

    for (i = 0; i < totalEmprestimos; i++) {
        if (listaEmprestimos[i].codigoLeitor == codigoLeitor &&
            listaEmprestimos[i].situacao == 'A') {
            return 1;
        }
    }

    return 0;
}

static void removerLeitor(void) {
    int codigo = lerInteiro("Carteirinha do leitor: ");
    int i = localizarLeitor(codigo);

    if (i == -1) {
        printf("Leitor nao encontrado.\n");
        return;
    }

    if (leitorPossuiEmprestimoAtivo(codigo)) {
        printf("O leitor possui emprestimo ativo e nao pode ser removido.\n");
        return;
    }

    ficharioLeitores[i].ativo = 0;
    printf("Leitor removido do fichario.\n");
}

static void buscarLivroCodigoOuTitulo(void) {
    int opcao;
    int codigo;
    int i;
    int encontrou = 0;
    char titulo[50];

    printf("1. Buscar por codigo\n");
    printf("2. Buscar por titulo\n");
    opcao = lerInteiro("Opcao: ");

    if (opcao == 1) {
        codigo = lerInteiro("Codigo do livro: ");
        i = localizarLivro(codigo);

        if (i == -1) {
            printf("Livro nao encontrado.\n");
        } else {
            mostrarLivro(i);
        }
    } else if (opcao == 2) {
        lerTexto("Titulo ou parte do titulo: ", titulo, sizeof(titulo));

        for (i = 0; i < totalLivros; i++) {
            if (acervoLivros[i].ativo == 1 &&
                strstr(acervoLivros[i].titulo, titulo) != NULL) {
                mostrarLivro(i);
                encontrou = 1;
            }
        }

        if (encontrou == 0) {
            printf("Livro nao encontrado.\n");
        }
    } else {
        printf("Opcao invalida.\n");
    }
}

static void buscarLivrosPorAutor(void) {
    int i;
    int encontrou = 0;
    char autor[40];

    lerTexto("Autor ou parte do nome: ", autor, sizeof(autor));

    for (i = 0; i < totalLivros; i++) {
        if (acervoLivros[i].ativo == 1 &&
            strstr(acervoLivros[i].autor, autor) != NULL) {
            mostrarLivro(i);
            encontrou = 1;
        }
    }

    if (encontrou == 0) {
        printf("Nenhum livro encontrado para esse autor.\n");
    }
}

static void ordenarAcervoBubbleSort(void) {
    int i;
    int j;
    Livro auxiliar;

    if (totalLivros == 0) {
        printf("O acervo esta vazio.\n");
        return;
    }

    for (i = 0; i < totalLivros - 1; i++) {
        for (j = 0; j < totalLivros - 1 - i; j++) {
            if (strcmp(acervoLivros[j].titulo,
                       acervoLivros[j + 1].titulo) > 0) {
                auxiliar = acervoLivros[j];
                acervoLivros[j] = acervoLivros[j + 1];
                acervoLivros[j + 1] = auxiliar;
            }
        }
    }

    printf("Acervo ordenado por titulo:\n");
    for (i = 0; i < totalLivros; i++) {
        if (acervoLivros[i].ativo == 1) {
            mostrarLivro(i);
        }
    }
}

void moduloBuscasEFichario(void) {
    int opcao;

    do {
        printf("\n--- MODULO 2: FICHARIO, BUSCAS E ORDENACAO (TAVIN) ---\n");
        printf("1. Cadastrar leitor\n");
        printf("2. Listar ou buscar leitor\n");
        printf("3. Editar leitor\n");
        printf("4. Remover leitor\n");
        printf("5. Buscar livro por codigo ou titulo\n");
        printf("6. Buscar livros por autor\n");
        printf("7. Ordenar acervo com Bubble Sort\n");
        printf("0. Voltar\n");

        opcao = lerInteiro("Opcao: ");

        switch (opcao) {
            case 1:
                cadastrarLeitor();
                break;
            case 2:
                buscarLeitor();
                break;
            case 3:
                editarLeitor();
                break;
            case 4:
                removerLeitor();
                break;
            case 5:
                buscarLivroCodigoOuTitulo();
                break;
            case 6:
                buscarLivrosPorAutor();
                break;
            case 7:
                ordenarAcervoBubbleSort();
                break;
            case 0:
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
}
