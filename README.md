# Biblioteca Clássica — Etapa 2 / Checkpoint 2

Sistema de biblioteca feito em C para a disciplina de Algoritmos e Pensamento Computacional. O programa mantém livros, leitores, empréstimos e pedidos de reserva/aquisição em vetores, carrega os dados dos CSVs ao iniciar e grava as alterações durante o uso.

A modelagem do Checkpoint 1 foi preservada: as quatro `structs` mantêm seus nomes e campos. Cada integrante possui um módulo `.h` com funções reais de cadastro, listagem, busca, edição e remoção.

## Compilar e executar

É necessário um compilador GCC com suporte a C11. O programa usa apenas a biblioteca padrão de C; não exige instalação de bibliotecas adicionais.

Abra o terminal na pasta que contém `main.c` e os quatro arquivos CSV. No Linux ou macOS:

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror main.c -o biblioteca
./biblioteca
```

No Windows, com GCC disponível no terminal:

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror main.c -o biblioteca.exe
.\biblioteca.exe
```

Compile somente `main.c`: ele inclui os módulos `.h`, que contêm as implementações. Não compile cada `.h` separadamente nem acrescente os headers ao comando do GCC.

Execute sempre na pasta dos CSVs. Os nomes são `livros.csv`, `leitores.csv`, `emprestimos.csv` e `reservas.csv`. Se faltar um arquivo ou houver dados inválidos, a inicialização será cancelada com uma mensagem; o programa não substitui silenciosamente a base por dados vazios.

No uso normal, pressione ENTER após cada operação para ler o resultado. A função `limparTela()` chama `cls` no Windows ou `clear` nos demais sistemas a cada exibição de menu. A opção abaixo desativa pausas e limpeza, facilitando testes com entrada redirecionada:

```sh
./biblioteca --sem-pausa
```

No Windows, use `.\biblioteca.exe --sem-pausa`.

## Organização e responsabilidades

| Arquivo | Responsável e conteúdo |
| --- | --- |
| `pedro.h` | Pedro: acervo de livros, cadastro, listagem, edição, remoção e CSV de livros. |
| `tavin.h` | Tavin: leitores, buscas de leitores/livros, CSV de leitores e Bubble Sort do acervo. |
| `kaue.h` | Kauê: empréstimos, renovação, devolução, cálculo de multas, vínculos entre módulos e CSV de empréstimos. |
| `joelson.h` | Joelson: reservas/aquisições, relatórios matemáticos, matriz bidimensional e CSV de reservas. |
| `comum.h` | Modelos originais, vetores, limites, protótipos, leitura de entradas, utilitários CSV e interface. |
| `main.c` | Integração dos módulos, carregamento inicial, validação dos relacionamentos, menu principal e proteção da gravação. |
| `testes/testar.py` | Verificação automatizada, separada do programa da biblioteca. |

Os vetores têm capacidade para 50 registros de cada tipo. O contador de cada vetor indica quantas posições estão ocupadas. Os campos `ativo` foram mantidos nas structs; os registros carregados/cadastrados são ativos, e a remoção é física: desloca os elementos seguintes e diminui o contador.

## Dados iniciais e formato dos CSVs

A base fornecida contém 10 livros, 8 leitores, 8 empréstimos — 4 ativos e 4 finalizados — e 8 pedidos de reserva/aquisição. Os nomes e títulos representam cadastros coerentes para demonstrar os módulos. Entre os livros técnicos está **C Completo e Total**, de Herbert Schildt, com valor de reposição de R$ 75,00.

Cada linha representa um registro, sem cabeçalho. O separador é `;`. Valores monetários usam ponto decimal e são gravados com duas casas. Não use `;` dentro dos textos. O sistema usa `setlocale(LC_ALL, "")` e mantém `LC_NUMERIC` em `"C"` para que a leitura e a gravação dos decimais funcionem com o mesmo formato em computadores diferentes.

| CSV | Ordem dos campos preservados da struct |
| --- | --- |
| `livros.csv` | `codigo;titulo;autor;categoria;valorReposicao;statusDisponivel;ativo` |
| `leitores.csv` | `codigoLeitor;nome;telefone;tipoAssociado;saldoMultas;ativo` |
| `emprestimos.csv` | `idEmprestimo;codigoLivro;codigoLeitor;nomeLeitor;diasPrazo;taxaAtrasoDiaria;situacao` |
| `reservas.csv` | `idReserva;tituloDesejado;categoriaObra;custoEstimado;prioridade;atendida` |

Exemplo de livro:

```text
1;Dom Casmurro;Machado de Assis;Romance;49.90;E;1
```

As funções `carregar...CSV()` leem linha por linha, separam os campos e validam tamanho dos textos, limites numéricos, quantidade de campos, valores permitidos e IDs repetidos. A integração também confere se os empréstimos apontam para livros/leitores existentes e se o status de cada livro corresponde aos empréstimos ativos.

Os IDs são inteiros positivos e únicos dentro de cada módulo. Depois do cadastro, o ID permanece imutável; a edição altera os demais dados. Ao remover um registro, seu ID deixa de ocupar uma posição no vetor e pode ser usado em um cadastro futuro, desde que não exista outro registro com esse ID no módulo.

## Regras de funcionamento

### Livros, leitores e buscas

Um livro novo começa com status `D` — disponível. O status `E` — emprestado — é controlado pelas operações de empréstimo/devolução e não pode ser alterado manualmente na edição do livro.

O leitor é do tipo `P` — padrão — ou `V` — VIP/pesquisador. O cadastro começa com saldo de multas igual a zero. A edição preserva esse saldo e atualiza o nome guardado nos empréstimos vinculados ao leitor. O sistema acumula multas na devolução; não possui uma opção de pagamento/baixa de multas.

As buscas textuais usam `strstr`: uma parte do título, autor, nome, telefone ou categoria pode localizar o registro, conforme o módulo. Elas diferenciam maiúsculas de minúsculas. A busca de livro também aceita o código exato.

Um livro ou leitor não pode ser removido enquanto existir qualquer empréstimo que o referencie, incluindo os finalizados. Para liberar esse vínculo, devolva o livro caso o empréstimo esteja ativo e depois remova o empréstimo finalizado no módulo de Kauê. A remoção do histórico não apaga multas existentes; um leitor com saldo positivo continua protegido contra remoção.

### Empréstimos, renovação e devolução

Antes de cadastrar um empréstimo, o programa valida o ID, a existência do livro e do leitor e a disponibilidade da obra. O nome do leitor é copiado do cadastro; não é digitado livremente no empréstimo. Após a gravação, o livro passa de `D` para `E`. Um mesmo livro não pode ter dois empréstimos ativos.

A renovação permite editar o prazo e a taxa diária de um empréstimo ativo. O prazo informado é o **total de dias permitidos desde o empréstimo**, e não uma quantidade adicional de dias.

Na devolução, o operador informa quantos dias decorreram desde o empréstimo. As structs originais não têm campos de data; por isso, o programa não calcula o período usando o calendário do computador.

```text
dias de atraso = maior valor entre 0 e (dias decorridos - prazo total)
multa = dias de atraso × taxa diária
novo saldo do leitor = saldo anterior + multa
```

Por exemplo: prazo de 7 dias, devolução após 10 dias e taxa de R$ 1,50 geram 3 dias de atraso e multa de R$ 4,50. Na mesma operação, a situação do empréstimo muda de `A` para `F`, o livro volta para `D` e a multa é somada ao leitor. Tentar devolver novamente um empréstimo finalizado é bloqueado, evitando cobrar a mesma devolução duas vezes.

O prazo aceito vai de 1 a 365 dias; a taxa diária, de R$ 0,00 a R$ 1.000,00; e os dias decorridos, de 0 a 36.500. Uma devolução que excederia o limite de R$ 1.000.000,00 de saldo de multas é recusada sem finalizar o empréstimo.

### Reservas e aquisições

A `struct Reserva` original representa um pedido de obra/reserva para o acervo: título desejado, categoria, custo estimado, prioridade (`A`, `M` ou `B`) e indicador de atendimento (`0` ou `1`). Ela não possui um código de leitor nem representa uma fila de empréstimo vinculada a uma pessoa.

Marcar um pedido como atendido atualiza seu cadastro e os relatórios; não cadastra automaticamente um livro. O cadastro da obra adquirida é feito no módulo de Pedro.

### Relatórios, matriz 2D e Bubble Sort

O relatório geral calcula quantidades disponíveis/emprestadas, taxa de ocupação do acervo, valor total e médio de reposição, saldo total de multas, empréstimos ativos/finalizados e custo das aquisições pendentes. As divisões verificam se o denominador é zero antes de calcular uma média ou porcentagem.

A matriz `matrizEstatisticas[4][2]` cruza **categoria × disponibilidade**. As linhas são `Romance`, `Tecnico`, `Ficcao` e `Outras`; as colunas são disponível e emprestado. A comparação das três primeiras categorias é exata, com `strcmp`; qualquer outra escrita/categoria, como `Historia`, `Poesia` ou `Técnico`, entra em `Outras`. Cada livro contribui para uma única célula. Os totais permitem verificar a distribuição e identificar quais categorias estão mais emprestadas.

O Bubble Sort de Tavin compara títulos com `strcmp`, troca livros vizinhos fora de ordem e repete as passagens até concluir a ordenação. A troca copia a **struct inteira**, mantendo código, autor, categoria, valor e status ligados ao título correto. Títulos iguais são desempatados pelo código. A ordem segue os bytes comparados por `strcmp` — ordem ASCII para os textos sem acentos da base, sem tratamento especial de acentos ou maiúsculas. O acervo ordenado é gravado no CSV e reaparece nessa ordem ao reiniciar.

## Como as alterações são salvas

Cada função `salvar...CSV()` abre um arquivo temporário com `fopen(..., "w")`, reescreve todo o vetor com `fprintf`, verifica a conclusão e renomeia o temporário para o CSV correspondente. Assim, editar e remover não deixam linhas antigas no arquivo.

Uma operação pode mudar mais de um módulo: devolver um livro altera empréstimos, livros e leitores. Por isso, `salvarEstadoCSV()` coordena os quatro CSVs. Antes de gravar, renomeia os CSVs anteriores para arquivos `.bak`. Se ocorrer uma falha detectada durante a operação, tenta restaurar esses backups e recupera também os vetores a partir de uma cópia do último estado confirmado na memória. Uma operação só mostra a mensagem de sucesso se a gravação tiver sido concluída.

Essa proteção trata erros detectados pelo programa; não é uma transação garantida contra queda de energia ou encerramento forçado no meio da gravação. Use uma instância do programa por vez e mantenha uma cópia da base antes de demonstrações que alterem os dados.

### Recuperar arquivos após interrupção

Se o programa encontrar um `.bak` ao iniciar, ele interrompe o carregamento para evitar continuar sobre uma gravação incompleta. Com o programa **fechado**:

1. Faça uma cópia dos CSVs, `.bak` e `.tmp` existentes antes de mexer neles.
2. Recupere **todos os backups existentes da operação interrompida** sobre seus CSVs correspondentes: `livros.csv.bak` sobre `livros.csv`, `leitores.csv.bak` sobre `leitores.csv`, e assim por diante. Não recupere somente um módulo por conveniência.
3. Se a falha aconteceu durante a proteção inicial dos arquivos, os CSVs que não chegaram a gerar um `.bak` ainda não haviam sido alterados por essa operação.
4. Após recuperar o conjunto, remova os temporários `.tmp` e reabra o programa. A validação inicial precisa aceitar os vínculos e disponibilidades antes de continuar.

Há uma situação diferente: se a gravação foi concluída e o programa mostrou o alerta para remover um **backup antigo** que não conseguiu apagar, os CSVs novos já foram confirmados. Nesse caso, preserve os CSVs confirmados e apague apenas os backups antigos indicados no alerta. Não os restaure como se fossem uma operação cancelada. Se uma interrupção aconteceu durante essa limpeza e não for possível identificar um conjunto consistente, use uma cópia completa da base; não misture versões por tentativa.

## Roteiro de demonstração na bancada

Comece por uma cópia da pasta para que o ensaio não altere a base inicial da entrega. A equipe deve deixar o código compilado e o repositório aberto antes de chamar o professor.

1. Liste os livros e leitores. Em empréstimos, compare a listagem dos ativos com a listagem completa. Mostre as quantidades carregadas dos CSVs.
2. Busque `Machado` por autor e `Casmurro` por título. Busque um leitor por parte do nome. Explique por que `strstr` permite encontrar parte de um texto.
3. Cadastre um livro com ID livre, como `11`, e um leitor com ID livre, como `9`. Tente repetir um desses IDs para mostrar o bloqueio.
4. Cadastre o empréstimo `9`, vinculando livro `11` e leitor `9`, prazo de `7` dias e taxa de `1.50`. Liste o livro para mostrar que ficou emprestado. Tente emprestar novamente o mesmo livro para mostrar a validação.
5. Renove/edite um empréstimo ativo e mostre que o novo prazo corresponde ao total de dias desde a retirada. Para o exemplo de multa, mantenha prazo de `7` e taxa de `1.50`.
6. Devolva o empréstimo `9` com `10` dias decorridos. Confira a multa de R$ 4,50 no leitor e a liberação do livro. Tente devolver outra vez para mostrar o bloqueio.
7. Edite os dados do livro/leitor e um pedido de reserva. Feche e reabra o programa para conferir a persistência. O nome editado do leitor também deve aparecer no empréstimo.
8. Tente remover o livro ainda referenciado pelo histórico. Depois remova o empréstimo finalizado e remova o livro. O leitor com multa permanece protegido. Para demonstrar a remoção de leitor com sucesso, cadastre e remova outro leitor sem empréstimos e sem saldo.
9. Cadastre um pedido de reserva/aquisição com ID livre, busque-o por parte do título, edite-o e remova-o. Reabra para mostrar que a remoção permaneceu no CSV.
10. Mostre o relatório geral, a matriz e a ordenação por Bubble Sort. Reinicie para conferir que a ordem do acervo foi persistida.

## Pontos para cada integrante explicar

- **Pedro:** como localizar um ID no vetor; por que um livro novo começa disponível; como a remoção desloca as posições; por que referências de empréstimos bloqueiam a exclusão; como `fprintf` reescreve o CSV.
- **Tavin:** diferença entre `strcmp` e `strstr`; busca sequencial; atualização do nome nos empréstimos; comparação de vizinhos no Bubble Sort; motivo para trocar a struct inteira e diminuir o limite a cada passagem.
- **Kauê:** como os códigos cruzam os módulos; por que checar livro/leitor antes de modificar o vetor; transições `D → E → D` e `A → F`; fórmula da multa; diferença entre dias decorridos e prazo total; proteção contra devolução duplicada.
- **Joelson:** finalidade de cada linha/coluna da matriz; como zerar e preencher suas células; soma, média e porcentagem dos relatórios; tratamento de divisão por zero; pedidos pendentes/atendidos e seus custos; integração no `main.c`.

Todos devem conseguir explicar também:

- `#define MAX_REGISTROS 50` substitui um nome por uma constante durante o pré-processamento e define o tamanho fixo dos vetores. Não é uma variável alterável em execução.
- As proteções `#ifndef`, `#define` e `#endif` dos headers impedem incluir o mesmo conteúdo duas vezes na mesma compilação.
- `FILE *` aponta para o controle de um arquivo aberto. `fopen` pode retornar `NULL`; o programa verifica isso antes de usar o ponteiro. `fclose` encerra o acesso ao arquivo.
- `fgets` lê uma linha com limite de tamanho. `separarCamposCSV` procura os separadores `;`, coloca `\0` em suas posições e guarda ponteiros para o início de cada campo.
- `strtol` e `strtof` convertem textos em números. O ponteiro de fim permite rejeitar entradas como `12abc`; a verificação dos limites e de `errno` trata valores fora da faixa.
- `&variavel` obtém o endereço da variável para uma função poder escrever nela. Em `preencherDadosReserva(&nova)`, o ponteiro recebido permite preencher a reserva; `reserva->campo` acessa seu campo pelo ponteiro.
- `memcpy` copia um bloco de memória com tamanho explícito. Aqui ele guarda/restaura os vetores completos no estado confirmado. Os contadores também são guardados/restaurados.
- `strcmp(a, b)` devolve zero se os textos forem iguais e sinal negativo/positivo conforme a ordem. `strstr(texto, termo)` devolve um ponteiro para a ocorrência do trecho ou `NULL` se não encontrar.
- Um vetor usa um índice; a matriz usa dois. Em `matrizEstatisticas[linha][coluna]`, os índices representam uma categoria e um status.

## Verificações automatizadas

O programa C não depende de Python. Para executar a suíte de verificação separada, é necessário Python 3 e GCC:

```sh
python3 testes/testar.py
```

O roteiro automatizado deve ser executado a partir da pasta do projeto. Consulte a saída do comando para os resultados de cada verificação.

Os resultados da entrega, a cobertura dos 18 testes e os limites do ambiente de verificação estão registrados em [TESTES.md](TESTES.md).

## Relação com os critérios do Checkpoint 2

| Critério | Implementação para demonstrar |
| --- | --- |
| 2.1 — CSV e módulos | Quatro módulos por integrante; carga automática de oito ou mais registros por módulo; gravação, edição e remoção persistidas. |
| 2.2 — Subfunções e buscas | Cada ação de submenu chama uma função; validação de entradas/IDs; buscas textuais reais com `strstr`; comparação com `strcmp`; CRUD dos quatro módulos. |
| 2.3 — Integração e análise | Validação livro/leitor, atualização da disponibilidade, cálculo de multa, relatórios matemáticos, matriz 4 × 2 e Bubble Sort de structs persistido. |
| 2.4 — Domínio e usabilidade | `locale.h`, `limparTela()`, mensagens de validação, pausas de leitura e explicações técnicas por responsável. |

O projeto foi organizado para apresentar os recursos exigidos para o nível Excelente. A avaliação e a nota final dependem da validação do professor e do domínio individual demonstrado por cada integrante na arguição.
