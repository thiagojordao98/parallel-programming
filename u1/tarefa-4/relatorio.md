# Benchmark OpenMP: Memory-bound e Compute-bound

## Objetivo

Este trabalho implementa dois programas paralelos em C com OpenMP para comparar o comportamento de aplicações limitadas por memória e por processamento.

- **Memory-bound**: soma simples de vetores, com acesso sequencial aos dados.
- **Compute-bound**: cálculo matemático intensivo, com muitas operações por elemento.

A proposta é variar o número de threads, medir o tempo de execução e analisar quando o desempenho melhora, estabiliza ou piora.

## Estrutura dos programas

### 1. `memory_bound.c`

Esse programa executa a soma de dois vetores e grava o resultado em um terceiro vetor. A parte paralela usa:

```c
#pragma omp parallel for
```

Como a operação por elemento é pequena e o acesso à memória é dominante, espera-se ganho limitado por largura de banda de memória e por custo de sincronização.

### 2. `compute_bound.c`

Esse programa aplica uma função matemática custosa em cada elemento de um vetor. A parte paralela também usa:

```c
#pragma omp parallel for
```

Aqui o custo computacional por iteração é alto, então o paralelismo tende a escalar melhor até o ponto em que a competição por recursos internos do processador começa a dominar.

## Como compilar e executar

```bash
make
./memory_bound [tamanho] [threads_max]
./compute_bound [tamanho] [threads_max]
```

Exemplos:

```bash
./memory_bound 100000000 8
./compute_bound 20000000 8
```

## Metodologia de medição

1. Executar cada programa com diferentes números de threads, de 1 até o máximo definido.
2. Registrar o tempo de execução de cada configuração.
3. Comparar o comportamento entre as duas classes de problema.

## Ambiente de execução (CPU)

Saída relevante do `lscpu`:

- **CPU(s): 8** (CPUs lógicas disponíveis)
- **Thread(s) per núcleo: 2**
- Portanto: **4 núcleos físicos** e **SMT 2x** (4C/8T)

Interpretação para os testes:
- **1–4 threads**: tende a escalar usando núcleos físicos distintos.
- **5–8 threads**: passa a usar SMT (2 threads competindo dentro do mesmo núcleo).

## Tabela de resultados

### Memory-bound

| Threads | Tempo (s) | Speedup |
|---|---:|---:|
| 1 |  |  |
| 2 |  |  |
| 4 |  |  |
| 8 |  |  |

### Compute-bound

Tamanho: 20000000

| Threads | Tempo (s) | Speedup |
|---|---:|---:|
| 1 | 14.447682 | 1.000 |
| 2 | 7.214163  | 2.002 |
| 3 | 4.807778  | 3.004 |
| 4 | 3.622517  | 3.987 |
| 5 | 3.141636  | 4.598 |
| 6 | 2.626450  | 5.500 |
| 7 | 2.267322  | 6.372 |
| 8 | 2.009815  | 7.188 |

## Análise (observada)

No compute-bound, houve ganho expressivo até 4 threads (núcleos físicos) e o desempenho continuou melhorando até 8 threads, indicando que o SMT também ajudou neste caso específico. Isso pode ocorrer quando parte do tempo é perdida com latências (ex.: funções matemáticas transcendentes) e o SMT consegue manter as unidades ocupadas.

Ainda assim, em workloads compute-bound mais “puros” (ex.: muitas operações vetoriais/FMA sem stalls), é comum o ganho após 4 threads reduzir bastante ou até piorar, pois 2 threads no mesmo núcleo competem por unidades de execução, registradores, cache e largura de banda interna.

## Reflexão

O multithreading de hardware tende a ajudar mais em programas memory-bound quando uma thread fica esperando dados da memória e outra pode ocupar a execução. Já em programas compute-bound, as threads competem mais diretamente pelos recursos do processador, o que pode reduzir ou até anular o ganho.

## Conclusão

Os dois benchmarks permitem observar que OpenMP é útil, mas o ganho depende fortemente do perfil da aplicação. Programas com muito acesso à memória tendem a saturar rapidamente. Programas com muita conta matemática escalam melhor, até o ponto em que a disputa por recursos e o overhead paralelizam o custo em vez de reduzi-lo.
