# Resultados de verificação

Verificação da entrega da Etapa 2 em 15/11/2026 (horário de São Paulo).

Ambiente executado: Linux, GCC 16.2.1. Compilação C11 aprovada com todos os avisos tratados como erros:

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror main.c -o biblioteca
python3 testes/testar.py
```

Resultado da suíte: **18 testes aprovados**, com cenários adicionais dentro de cada teste. Cada execução usa cópias temporárias dos quatro CSVs e reinicia o programa para comprovar a persistência. Os dados iniciais versionados permanecem intactos.

| Teste | Resultado verificado |
| --- | --- |
| 01 | Carga de 10 livros, 8 leitores, 8 empréstimos e 8 reservas; listagens; totais, médias, porcentagens e matriz. |
| 02 | Cadastro, busca, edição e remoção de livro; bloqueio de ID duplicado; reabertura. |
| 03 | CRUD de leitores; busca; edição de nome sincronizada nos empréstimos. |
| 04 | CRUD e busca textual de reservas; edição e remoção persistidas. |
| 05 | Recusa de ID de empréstimo duplicado, livro/leitor inexistente e livro indisponível. |
| 06 | Empréstimo e renovação persistidos; disponibilidade E/D; devolução com multa de R$ 9,00; bloqueio de devolução repetida e remoção de ativo; exclusão de finalizado. |
| 07 | Devolução no prazo sem multa; acumulação de nova multa sobre saldo existente. |
| 08 | Proteção das referências históricas e dos leitores com saldo; remoção após eliminar vínculo permitido. |
| 09 | Buscas por título/autor/leitor; Bubble Sort persistido, desempate por ID e preservação de todos os campos das structs. |
| 10 | Entrada alfabética, negativa, inteira fora de faixa, texto vazio, separador proibido, texto excessivo, NaN, infinito e vírgula decimal recusados; título com exatamente 49 bytes aceito. |
| 11 | Busca, edição, devolução e remoção de IDs ausentes sem alteração dos CSVs. |
| 12 | Carga de 50 registros em cada módulo e bloqueio de novos cadastros na capacidade máxima. |
| 13 | CSV duplicado, malformado, com mais de 50 registros ou CR embutido recusado sem sobrescrita, em cada módulo. |
| 14 | Recusa de referência inexistente, nome divergente, disponibilidade inconsistente, empréstimos ativos concorrentes e CSV ausente. |
| 15 | Falha de abertura simulada em cada um dos quatro temporários; restauração de memória e dos quatro CSVs; reabertura aprovada. |
| 16 | Falha de escrita/fechamento simulada com `/dev/full`; preservação dos CSVs e cancelamento da devolução. |
| 17 | Leitura de CRLF e última linha sem quebra; base vazia sem divisão por zero. |
| 18 | Backup pendente bloqueia inicialização sem sobrescrever arquivos. |

Conferência adicional: os nomes, tipos, ordem e tamanhos dos campos das quatro structs coincidem com o código anexado do Checkpoint 1.

A análise estática adicional do GCC com `-fanalyzer`, mantendo os avisos como erros, também foi aprovada.

Valores esperados da base inicial:

- 6 livros disponíveis e 4 emprestados; ocupação de 40,00%.
- Valor de reposição total de R$ 569,30 e médio de R$ 56,93.
- Multas pendentes de R$ 4,50; 4 empréstimos ativos e 4 finalizados.
- 6 pedidos pendentes e 2 atendidos; aquisições pendentes de R$ 407,50.
- Matriz D/E: Romance 1/2; Técnico 1/1; Ficção 2/1; Outras 2/0.

Os comandos de Windows estão documentados no README; a execução foi validada em Linux. O teste `/dev/full` é específico de Linux e é pulado onde esse dispositivo não existe. A tentativa opcional de executar AddressSanitizer/UndefinedBehaviorSanitizer não pôde ser vinculada neste ambiente, pois as bibliotecas desses instrumentos não estão instaladas; isso não afeta a compilação normal nem os 18 testes executados.

A nota e o domínio individual dependem da avaliação presencial. As limitações sobre datas, pagamentos e recuperação após interrupção estão descritas no README.
