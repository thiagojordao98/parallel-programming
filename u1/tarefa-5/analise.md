# Análise da Tarefa 5: Contagem de Números Primos

## Objetivo
Contar quantos números primos existem de 2 até `n` e comparar as versões **SEQUENCIAL** e **PARALELA** (OpenMP com `#pragma omp parallel for`).

## Resultado observado
Com `n = 500000`, o programa retornou a mesma contagem nas duas versões:

- Quantidade de primos: **41538**
- Validação: **OK (valores iguais)**

## Comparação de tempo

| Configuração | Tempo sequencial (s) | Tempo paralelo (s) | Speedup |
|---|---:|---:|---:|
| `OMP_NUM_THREADS=1` | 0.056639 | 0.074840 | 0.76x |
| `OMP_NUM_THREADS=8` | 0.056430 | 0.037893 | 1.49x |

## Conclusão
- A versão paralela mantém o mesmo resultado da versão sequencial.
- Com 8 threads, houve ganho de desempenho.
- Com 1 thread, a versão paralela ficou mais lenta devido ao overhead da paralelização.
