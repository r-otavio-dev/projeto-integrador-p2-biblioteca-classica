# Projeto Integrador P2 - Biblioteca Clássica

Sistema de terminal em C para gerenciar o fichário, o acervo, os empréstimos e as reservas de uma biblioteca.

## Funcionalidades

### Módulo 1 - Pedro: acervo de livros

- Cadastro de livros com validação do limite da memória.
- Listagem, edição e exclusão de livros.
- Controle de disponibilidade do livro.

### Módulo 2 - Tavin: fichário, buscas e ordenação

- Cadastro, listagem, busca, edição e remoção de leitores.
- Busca de livros por código ou título.
- Busca de livros por autor.
- Ordenação do acervo por título usando Bubble Sort.

### Módulo 3 - Kauê: empréstimos e devoluções

- Registro de empréstimos com validação do livro e do leitor.
- Listagem e renovação de empréstimos ativos.
- Devolução do livro e cálculo de multa por atraso.

### Módulo 4 - Joelson: reservas e relatórios

- Cadastro, listagem, edição e remoção de reservas.
- Relatório geral do acervo, leitores, empréstimos e multas.
- Matriz de categorias por disponibilidade do livro.

## Estruturas utilizadas

O projeto possui quatro estruturas e quatro vetores globais:

- `Livro acervoLivros[50]`
- `Leitor ficharioLeitores[50]`
- `Emprestimo listaEmprestimos[50]`
- `Reserva listaReservas[50]`

Os dados são mantidos em memória RAM durante a execução. Ao fechar o programa, os dados são apagados.

## Exigências atendidas

- Quatro módulos separados por integrante.
- Structs com campos `int`, `float`, `char` e strings.
- Vetores globais e validação de capacidade.
- Menus com `do-while` e `switch-case`.
- Funções separadas para cadastro, listagem, busca, edição e remoção.
- Funções com retorno para buscas, contagens e cálculos.
- Busca sequencial, relatório analítico e Bubble Sort.
- Matriz global de categoria por disponibilidade.
- Relacionamento entre livros, leitores e empréstimos.

## Organização

```text
.
├── include/
│   └── biblioteca.h
├── src/
│   ├── buscas.c
│   ├── dados.c
│   ├── entrada.c
│   ├── main.c
│   ├── modulo_emprestimos.c
│   ├── modulo_livros.c
│   └── modulo_reservas_relatorios.c
├── Makefile
└── README.md
```

## Como compilar

Com GCC e Make:

```bash
make
```

Ou diretamente:

```bash
gcc -std=c11 -Wall -Wextra -pedantic -Iinclude src/*.c -o biblioteca
```

Para executar no Linux:

```bash
./biblioteca
```

No Dev-C++, crie um projeto de console em C, adicione os arquivos da pasta `src` e inclua a pasta `include` no caminho de cabeçalhos.

## Observações

- `D` significa livro disponível e `E` significa livro emprestado.
- `P` significa leitor padrão e `V` significa leitor VIP/pesquisador.
- A exclusão de livros e leitores é lógica, preservando os registros relacionados.
- Livros emprestados e leitores com empréstimos ativos não podem ser excluídos.
