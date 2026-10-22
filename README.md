# Projeto Integrador P2 - Biblioteca Clássica

Sistema de gestão retrô (CRUD em C) para o tema oficial 7: **Biblioteca Clássica (Fichário, Acervo e Empréstimos)**.

> Estado atual: estrutura inicial do Checkpoint 1. O programa compila e apresenta o menu-base, mas as regras de negócio dos quatro módulos ainda devem ser implementadas pelo grupo.

## Exigências do professor

### Objetivo e padrão de qualidade

- Desenvolver, em **quarteto**, um software completo de terminal em C.
- Organizar o sistema em **4 módulos independentes**, com responsabilidade direta de um integrante por módulo.
- Manter a base de dados em memória RAM usando `struct` e vetor de tamanho adequado.
- Usar menu interativo em loop contínuo com `do-while` e `switch-case`.
- Modularizar rigorosamente o código em funções separadas por integrante.
- Incluir processamento analítico: buscas, relatórios e ordenação.
- Usar funções `void` para tarefas sem retorno e funções com tipo de retorno para cálculos/regras de negócio.
- No padrão apresentado em aula, manter a estrutura e o vetor da base em escopo global para que as funções possam acessá-los sem ponteiros avançados.

### Os quatro módulos obrigatórios

| Responsável | Módulo | Responsabilidade indicada pelo professor |
|---|---|---|
| Integrante A | Inserção e cadastro | Validação dos limites da memória RAM |
| Integrante B | Busca e modificação | Atualização de status/dados |
| Integrante C | Remoção e matriz | Grade de controle relacionada ao tema |
| Integrante D | Relatórios e ordenação | Cálculos analíticos e Bubble Sort |

### Avaliação - total de 5,0 pontos

- **Checkpoint 1 - 1,0 ponto:** arquitetura inicial, `struct`, menu-base e criação do repositório no GitHub.
- **Checkpoint 2 - 1,0 ponto:** módulos funcionais de cadastro e listagem em funções separadas.
- **Checkpoint 3 - 1,0 ponto:** sistema completo com relatórios, ordenação e auditoria de commits no Git.
- **Entrega final - 2,0 pontos:** gravação no MS Teams, com câmera aberta, demonstração do sistema rodando e código explicado por cada integrante, além do `.zip` do código comentado.

### Avaliação individual por pares

- Cada estudante distribui **6 fichas** entre os colegas do grupo.
- A nota individual segue a fórmula:

```text
nota_individual = nota_grupo * (0,5 + 0,5 * fichas_recebidas / 6)
```

- Pela explicação do professor, quem participa plenamente pode manter 100% da nota; quem não participa pode perder metade da nota do grupo.

### Cuidados técnicos enfatizados na aula

- Em vetores de tamanho `N`, os índices válidos vão de `0` a `N - 1`.
- Sempre validar a capacidade antes de inserir um novo registro.
- Ao procurar maior/menor valor, inicializar a comparação com um dado real do vetor, e não com zero arbitrário.
- Usar o endereço correto no `scanf` (por exemplo, `&registro.campo` para campos numéricos).
- Evitar concentrar toda a lógica em um `main()` monolítico; o `main()` deve coordenar as chamadas dos módulos.

## O que não foi definido no PDF

O material não informa datas dos checkpoints, nomes dos integrantes, campos definitivos das estruturas, capacidade oficial dos vetores, significado da matriz para a biblioteca, regras de prazo/multa de empréstimos, persistência em arquivo, padrão de nomes ou visibilidade do repositório. Essas decisões precisam ser confirmadas com o professor ou definidas pelo grupo.

## Sugestão inicial de implementação

As decisões abaixo são apenas uma base técnica e **não são requisitos oficiais**:

- `RegistroBiblioteca` começa com `id`, `descricao` e `status`; o grupo deve substituir esses campos pelo modelo aprovado para fichário, acervo e empréstimos.
- `CAPACIDADE_REGISTROS` começa em 100 apenas como valor provisório.
- A grade de controle é declarada, mas seu significado e dimensões ficam pendentes de definição.
- Cada módulo obrigatório fica em um arquivo `.c` separado, facilitando a divisão do trabalho e a auditoria de commits.
- O repositório começa privado para proteger o trabalho acadêmico em andamento.

## Organização

```text
.
├── include/
│   └── biblioteca.h
├── src/
│   ├── dados.c
│   ├── main.c
│   ├── modulo_a_cadastro.c
│   ├── modulo_b_busca_modificacao.c
│   ├── modulo_c_remocao_matriz.c
│   └── modulo_d_relatorios_ordenacao.c
├── .gitignore
├── Makefile
└── README.md
```

## Como compilar

Com GCC/MinGW:

```bash
make
```

Ou diretamente:

```bash
gcc -std=c11 -Wall -Wextra -pedantic -Iinclude src/*.c -o biblioteca
```

No Dev-C++, crie um projeto de console em C, adicione todos os arquivos de `src/` ao projeto e inclua a pasta `include/` no caminho de cabeçalhos.

## Próximos passos do grupo

1. Preencher os nomes e associar um integrante a cada módulo.
2. Confirmar com o professor os campos das estruturas e o papel da matriz no tema 7.
3. Definir a capacidade adequada dos vetores.
4. Cada integrante implementar seu próprio módulo e fazer commits identificáveis.
5. Testar os limites dos vetores, entradas inválidas, buscas sem resultado e ordenação.
6. Manter o código comentado para a entrega final.

## Divisão da equipe

| Módulo | Integrante | Status |
|---|---|---|
| A - Inserção e cadastro | A definir | Estrutura criada |
| B - Busca e modificação | A definir | Estrutura criada |
| C - Remoção e matriz | A definir | Estrutura criada |
| D - Relatórios e ordenação | A definir | Estrutura criada |

