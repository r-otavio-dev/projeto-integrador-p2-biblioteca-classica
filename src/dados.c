#include "biblioteca.h"

Livro acervoLivros[CAPACIDADE];
Leitor ficharioLeitores[CAPACIDADE];
Emprestimo listaEmprestimos[CAPACIDADE];
Reserva listaReservas[CAPACIDADE];

int totalLivros = 0;
int totalLeitores = 0;
int totalEmprestimos = 0;
int totalReservas = 0;

int matrizEstatisticas[CATEGORIAS][SITUACOES_LIVRO] = {{0}};
