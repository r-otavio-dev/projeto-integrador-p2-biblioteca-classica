# Biblioteca Clássica — Checkpoint 2

Versão simples, com vetores, structs, funções, `if`, `for` e arquivos CSV. São **cinco arquivos de código**. Os quatro `.h` foram mantidos porque o enunciado exige um módulo por integrante e proíbe entregar tudo apenas no `main.c`.

## Compilar e executar

Na pasta do projeto, com GCC instalado:

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror main.c -o biblioteca
./biblioteca
```

No Windows:

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror main.c -o biblioteca.exe
.\biblioteca.exe
```

Compile somente `main.c`. Ele inclui os quatro `.h`. Execute na pasta dos CSVs. O programa usa apenas a biblioteca padrão de C.

## Onde está cada parte

| Arquivo | Conteúdo |
| --- | --- |
| `main.c` | As quatro structs originais, vetores, leitura do teclado, abertura dos arquivos, carregamento e menu principal. |
| `pedro.h` | Pedro: cadastrar, listar, buscar, editar, remover e salvar livros. A busca de livros é escrita por Tavin e chamada neste menu. |
| `tavin.h` | Tavin: leitores, buscas por título/autor/nome e Bubble Sort. |
| `kaue.h` | Kauê: empréstimos, renovação, devolução, multa e histórico. |
| `joelson.h` | Joelson: reservas/aquisições, relatórios e matriz 2D. |

Cada opção do menu chama uma função. Não há funções em construção, camada de backups, cópias do estado ou leitor genérico de CSV.

## Como funciona

- **Dados:** cada módulo possui um vetor de até 50 registros. Os códigos vão de 1 a 9999, não podem repetir dentro do mesmo módulo e permanecem fixos na edição.
- **CSV:** `livros.csv`, `leitores.csv`, `emprestimos.csv` e `reservas.csv` usam `;`, sem cabeçalho e com ponto decimal. A base inicial tem 10 livros e 8 registros em cada outro módulo.
- **Leitura:** ao abrir, `fgets` lê uma linha e `sscanf` coloca seus campos na struct. Máscaras como `%49[^;\r\n]` limitam o texto ao tamanho do campo. Dados inválidos cancelam a inicialização.
- **Gravação:** `salvar...CSV()` abre o CSV do módulo com `"w"` e reescreve seu vetor com `fprintf`. Depois de editar ou remover, o arquivo já fica atualizado. Um erro de arquivo interrompe o programa sem mostrar sucesso; esta versão não faz recuperação automática se uma gravação for interrompida.
- **Remoção:** o `for` desloca os elementos seguintes uma posição para a esquerda e reduz o total. Livro/leitor referenciado em empréstimo não pode ser removido; primeiro devolva e remova o empréstimo finalizado. Leitor com multa também não pode ser removido.
- **Busca:** `strstr` encontra parte de um texto; `strcmp` compara textos. As buscas diferenciam maiúsculas e minúsculas.
- **Empréstimo:** valida livro, leitor e disponibilidade, copia o nome cadastrado e muda o livro de `D` para `E`.
- **Devolução:** o operador informa os dias decorridos; as structs originais não têm datas. A multa é `max(0, dias decorridos - prazo) × taxa diária`. Ela soma ao saldo do leitor, o empréstimo passa de `A` para `F` e o livro volta a `D`. Devolver duas vezes é bloqueado. O programa registra o saldo, sem uma função de pagamento.
- **Renovação:** edita o prazo total desde a retirada e a taxa diária de um empréstimo ativo.
- **Reserva:** é um pedido de aquisição/obra, conforme a struct original. Pode ser marcado como atendido; não cria livro automaticamente nem uma fila por leitor.
- **Relatórios:** somam valores de reposição, multas e custos pendentes, calculam média e porcentagem e contam empréstimos ativos/finalizados.
- **Matriz:** `matrizEstatisticas[4][2]` conta livros por categoria e disponibilidade. As categorias exatas são `Romance`, `Tecnico`, `Ficcao` e `Outras` para as demais.
- **Bubble Sort:** dois `for` comparam títulos com `strcmp` e trocam a struct inteira. Títulos iguais são ordenados pelo código. A ordenação é salva em `livros.csv`.

## Para explicar na banca

1. `#define MAX_REGISTROS 50` define o limite fixo dos vetores.
2. `buscarIndice...()` percorre o vetor e retorna a posição do código ou `-1`.
3. `FILE *` representa o arquivo aberto; `"r"` lê e `"w"` reescreve. A abertura e o fechamento são conferidos.
4. Em `sscanf`, `&campo` passa o endereço de um número; os textos já representam o início do vetor de caracteres. A variável `extra` detecta conteúdo sobrando depois do último campo.
5. As funções de entrada usam `fgets` para evitar que letras travem a leitura numérica. `%9d` limita a conversão do inteiro; `lerInteiro` confere a faixa permitida. `lerReal` guarda os valores em centavos.
6. Os `.h` têm proteção contra inclusão repetida. Ficam incluídos após as structs e os auxiliares do `main.c`, pois usam esses dados.
7. `locale.h` configura a apresentação; `LC_NUMERIC` mantém o ponto decimal do CSV. `limparTela()` chama `cls` ou `clear`, e a pausa deixa o resultado visível.

Para demonstrar, cadastre livro `11`, leitor `9` e empréstimo `9`, com prazo `7` e taxa `1.50`. Devolva após `10` dias: multa esperada **R$ 4,50**. Feche e reabra para mostrar a persistência. Edite/remova um pedido de reserva e mostre a matriz e a ordenação.

## Verificação

A versão simplificada passou por compilação GCC C11 sem avisos e **16 testes** em Linux: CRUD dos quatro módulos, buscas, IDs repetidos/inválidos, empréstimos, multas, persistência, matriz, Bubble Sort e CSVs inválidos. Os testes usam cópias temporárias dos dados.

Opcionalmente, com Python 3 e GCC:

```sh
python3 testes/testar.py
```

O programa C não depende de Python. `./biblioteca --sem-pausa` desativa pausas e limpeza para os testes. A implementação mantém os requisitos técnicos do nível Excelente; a nota também depende da explicação individual na banca.
