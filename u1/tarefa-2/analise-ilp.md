# Tarefa 2 — Análise de Paralelismo ao Nível de Instrução (ILP)

## Descrição dos Laços

### Laço 1 — Inicialização independente
```c
for (int i = 0; i < n; i++) {
    v[i] = (double)i * 0.5 + 1.0;
}
```
Cada iteração é **totalmente independente** — `v[i]` não depende de `v[i-1]`. O processador pode alimentar o pipeline com várias iterações simultâneas.

### Laço 2 — Soma com dependência (acumulador único)
```c
double soma = 0.0;
for (int i = 0; i < n; i++) {
    soma += v[i];
}
```
A iteração `i` precisa do resultado de `i-1` para atualizar `soma`. Isso cria uma **cadeia de dependência RAW (Read After Write)** que bloqueia o pipeline — a próxima soma só pode iniciar após a anterior completar o estágio de escrita.

### Laço 3 — Soma com múltiplos acumuladores
```c
double soma0 = 0.0, soma1 = 0.0, soma2 = 0.0, soma3 = 0.0;
for (int i = 0; i < limite; i += 4) {
    soma0 += v[i];
    soma1 += v[i + 1];
    soma2 += v[i + 2];
    soma3 += v[i + 3];
}
return soma0 + soma1 + soma2 + soma3;
```
Cada acumulador forma uma **cadeia independente**. O processador pode executar as 4 somas em paralelo nos estágios do pipeline, explorando ao máximo o ILP.

---

## Resultados Experimentais

> Vetor com **100 milhões** de elementos `double`.

| Laço | Descrição | -O0 (s) | -O2 (s) | -O3 (s) |
|------|-----------|---------|---------|---------|
| 1 | Inicialização independente | 0.622 | 0.296 | 0.270 |
| 2 | Soma dependente (1 acumulador) | 0.218 | 0.075 | 0.071 |
| 3 | Soma multi-acumulador (4 acum.) | 0.080 | 0.048 | 0.057 |

### Speedup do Laço 3 vs Laço 2

| Nível | Speedup |
|-------|---------|
| -O0 | **2.74×** |
| -O2 | **1.56×** |
| -O3 | **1.26×** |

---

## Análise

### 1. Por que o Laço 1 é o mais lento?

Embora **sem dependências**, o Laço 1 realiza **escrita na memória** (`v[i] = ...`). Isso envolve:
- Alocação de linhas de cache (write-allocate)
- Tráfego de barramento para mover dados para a cache/memória
- O gargalo é a **largura de banda de memória**, não o ILP

### 2. Por que o Laço 2 é mais rápido que o 1, mas sofre com dependência?

O Laço 2 faz apenas **leitura** de `v[i]` (já em cache após o Laço 1) e acumula em um registro. Porém, a dependência `soma += v[i]` cria uma cadeia onde:

```
Ciclo:  |  soma += v[0]  |  soma += v[1]  |  soma += v[2]  | ...
        |--- latência --->|--- espera ---->|--- espera ----->|
```

O pipeline fica **parado** esperando o resultado anterior (stall por dependência de dados).

### 3. Por que o Laço 3 é mais rápido?

Com 4 acumuladores independentes, o processador pode **intercalar** as operações:

```
Ciclo 1:  soma0 += v[0]   soma1 += v[1]   soma2 += v[2]   soma3 += v[3]
Ciclo 5:  soma0 += v[4]   soma1 += v[5]   soma2 += v[6]   soma3 += v[7]
          (cada soma é independente → execução paralela no pipeline)
```

### 4. Por que o speedup diminui com níveis maiores de otimização?

| -O0 (2.74×) → -O2 (1.56×) → -O3 (1.26×) |
|---|

Com `-O2` e `-O3`, o compilador aplica automaticamente otimizações que **reduzem o impacto da dependência** no Laço 2:
- **Vetorização (SIMD)**: usa instruções SSE/AVX para processar múltiplos `double` por ciclo
- **Desenrolamento de laço (loop unrolling)**: expande o laço internamente, criando oportunidades de ILP semelhantes ao Laço 3
- **Reordenação de instruções**: o compilador reorganiza para preencher bolhas do pipeline

Isso significa que em `-O3`, o compilador já faz algo parecido com o que fizemos manualmente no Laço 3 — por isso o ganho manual fica menor.

> [!IMPORTANT]
> O Laço 3 é uma técnica de otimização **manual** que o programador aplica no código-fonte. Com `-O0`, essa técnica é essencial (2.74×). Com `-O3`, o compilador já aplica transformações semelhantes automaticamente, reduzindo o ganho manual para 1.26×.

---

## Conclusões

1. **Dependências de dados limitam o ILP**: a cadeia `soma += v[i]` serializa a execução e impede o pipeline de operar plenamente.

2. **Múltiplos acumuladores quebram a dependência**: ao criar cadeias independentes, permitimos que o hardware explore paralelismo intrínseco.

3. **O compilador é inteligente, mas não infalível**: em `-O0` a diferença é gritante (2.74×); em `-O3` o compilador otimiza automaticamente, mas o código explicitamente paralelo ainda tem vantagem.

4. **O estilo do código importa**: escrever código "amigável ao pipeline" (sem dependências desnecessárias) é uma prática importante em computação de alto desempenho.
