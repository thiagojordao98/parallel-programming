# Análise: Aproximação de Pi e o Custo Computacional da Precisão

## 1. Resultados Obtidos
Executando o algoritmo de aproximação de Pi (usando a série de Leibniz), obtivemos a seguinte relação entre o número de iterações, o tempo de execução e a precisão do resultado final:

| Iterações     | Tempo (s)       | Valor Estimado  | Erro Absoluto  |
|---------------|-----------------|-----------------|----------------|
| 10            | 0.000005        | 3.0418396189    | 0.0997530347   |
| 100           | 0.000001        | 3.1315929036    | 0.0099997500   |
| 1000          | 0.000002        | 3.1405926538    | 0.0009999997   |
| 10000         | 0.000019        | 3.1414926536    | 0.0001000000   |
| 100000        | 0.000169        | 3.1415826536    | 0.0000100000   |
| 1000000       | 0.001634        | 3.1415916536    | 0.0000010000   |
| 10000000      | 0.016464        | 3.1415925536    | 0.0000001000   |
| 100000000     | 0.188943        | 3.1415926436    | 0.0000000100   |
| 1000000000    | 1.845652        | 3.1415926526    | 0.0000000010   |

**Valor real de Pi:** 3.141592653589793

## 2. Análise da Acurácia vs. Processamento
A tabela acima demonstra o que chamamos de _trade-off_ entre precisão e tempo computacional. A série de Leibniz converge muito lentamente. Nota-se que para ganharmos **uma casa decimal a mais de precisão** (ou seja, diminuir o erro absoluto por um fator de 10), precisamos aumentar o número de iterações também em cerca de **10 vezes**.

Por consequência, o tempo de execução (tempo de CPU) também cresce linearmente de acordo com a quantidade de iterações. Por exemplo, passar de 100.000.000 iterações para 1.000.000.000 de iterações fez o tempo saltar de ~0.18s para ~1.84s (multiplicando por ~10x), o que nos rendeu apenas um pequeno aumento de precisão.

Isso nos mostra empiricamente que **alcançar maior acurácia exige mais esforço computacional e processamento**.

## 3. Reflexão sobre Aplicações Reais

Este comportamento se repete de maneira sistemática na computação do mundo real, especialmente em áreas que demandam processamento intensivo:

1. **Simulações Físicas (Clima, Astrofísica, Engenharia):**
   Muitas simulações dependem da integração numérica para prever o comportamento ao longo do tempo. Calcular o tempo com passos grandes (poucas "iterações") é rápido, mas o erro acumula e a previsão perde o sentido; calcular com passos pequenos (inúmeras iterações) eleva drasticamente a precisão da simulação, mas requer supercomputadores processando por dias. Há sempre um limite sobre quanto tempo temos para esperar por um resultado vs. quão preciso ele precisa ser para tomar decisões no mundo real (ex.: prever a rota de um furacão a tempo de evacuar uma cidade).

2. **Inteligência Artificial e Machine Learning:**
   O treinamento de redes neurais é fundamentalmente baseado na minimização de uma função de erro de forma iterativa (Gradient Descent). Assim como no cálculo do $\pi$, o modelo melhora muito rápido nas iterações iniciais, mas os ganhos tornam-se decrescentes depois de um tempo. Exigir 1% a mais de acurácia em um modelo de classificação (ou um LLM avançado) pode representar dezenas ou centenas de vezes mais dados para treinar e um aumento gigantesco no tempo de computação das GPUs, resultando em enormes custos de energia elétrica.

3. **Renderização de Gráficos e Ray Tracing:**
   Em computação gráfica voltada para realismo, o algoritmo de *Path Tracing* calcula a cor de cada pixel com base no lançamento de "raios de luz". Para um resultado sem ruídos, é preciso aumentar massivamente o "número de amostras" (iterações). Alcançar a perfeição visual demanda multiplicar o tempo de render de minutos para horas em um único frame.

### Conclusão
Em suma, seja aproximando o Pi, renderizando um filme ou treinando uma IA de linguagem, um conceito computacional onipresente é que **os ganhos iniciais de precisão são baratos, enquanto alcançar o limite máximo da perfeição consome exponencialmente mais tempo, recursos e energia.** Caberá sempre aos desenvolvedores e engenheiros avaliar qual nível de precisão é suficiente para os recursos disponíveis.
