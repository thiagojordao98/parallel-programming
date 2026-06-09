# Análise da Tarefa 5: Contagem de Números Primos

## Objetivo

Contar quantos números primos existem de 2 até `n` e comparar as versões **SEQUENCIAL** e **PARALELA** (OpenMP com `#pragma omp parallel for` e `reduction`).

## Resultado observado

Com `n = 500000`, o programa retornou a mesma contagem nas duas versões:

- Quantidade de primos: **41538**
- Validação: **OK (valores iguais)**

## Comparação de tempo

| Configuração        | Tempo sequencial (s) | Tempo paralelo (s) | Speedup |
| ------------------- | -------------------: | -----------------: | ------: |
| `OMP_NUM_THREADS=1` |             0.056639 |           0.074840 |   0.76x |
| `OMP_NUM_THREADS=8` |             0.056430 |           0.037893 |   1.49x |

## Conclusão

- A versão paralela mantém o mesmo resultado da versão sequencial.
- Com 8 threads, houve ganho de desempenho.
- Com 1 thread, a versão paralela ficou mais lenta devido ao overhead da paralelização.

## Testes Adicionais (n = 1000000)

| Threads | Tempo sequencial (s) | Tempo paralelo (s) | Primos (Paralelo) | Resultado | Speedup |
| :-----: | -------------------: | -----------------: | ----------------: | --------- | ------: |
|  **1**  |             0.245000 |           0.212000 |             78498 | OK        |   1.16x |
|  **2**  |             0.247000 |           0.158000 |             78253 | **ERRO**  |   1.56x |
|  **4**  |             0.264000 |           0.101000 |             77366 | **ERRO**  |   2.61x |
|  **6**  |             0.235000 |           0.100000 |             77406 | **ERRO**  |   2.35x |
|  **8**  |             0.335000 |           0.100000 |             77391 | **ERRO**  |   3.35x |
| **12**  |             0.240000 |           0.095000 |             77289 | **ERRO**  |   2.53x |

> **Nota sobre os ERROS:** Como visto, a execução com mais de 1 thread apresentou contagens incorretas. Isso ocorre devido a uma **condição de corrida (race condition)**: todas as threads tentam incrementar a variável `count` simultaneamente. Para corrigir isso sem comprometer o desempenho, deve-se utilizar a diretiva `reduction(+:count)` no OpenMP, o que garante a soma correta dos valores contados pelas threads.
