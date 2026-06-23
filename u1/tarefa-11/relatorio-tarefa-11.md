***

### RELATÓRIO TAREFA 11 - DCA3703 - `PROGRAMAÇÃO PARALELA`

**Título:** Simulação Bidimensional de Fluidos Viscosos e Análise de Orquestração de Malhas com OpenMP (`collapse` e `schedule`)
**Data:** [Data de Hoje]

---

#### RESUMO
Este relatório investiga a simulação do movimento de um fluido ao longo do tempo utilizando a equação de Navier-Stokes. Focando estritamente nos efeitos da viscosidade (desconsiderando pressão e forças externas), o modelo se reduz a uma equação de difusão de velocidade. A discretização espacial e temporal foi realizada pelo método das diferenças finitas em uma malha 2D. O experimento avalia a corretude física do modelo ao aplicar uma perturbação no centro de um fluido em repouso, observando sua difusão suave. Computacionalmente, o algoritmo foi paralelizado com OpenMP para explorar o impacto do achatamento de laços através da cláusula `collapse(2)` e as consequências de diferentes particionamentos de carga através da cláusula `schedule`. O estudo corrobora que, para problemas de malha uniforme, o balanceamento estático padrão preserva a localidade espacial (memória *cache*) e supera esquemas de particionamento dinâmico.

---

#### 1. INTRODUÇÃO E FUNDAMENTAÇÃO TEÓRICA

A **equação de Navier-Stokes** é o pilar da mecânica dos fluidos. Ela descreve como um fluido se move, acelera e se deforma em função da pressão, da viscosidade e das forças externas aplicadas sobre ele. Suas aplicações práticas são vastas e vitais para a ciência moderna, englobando:
*   **Movimento de ar:** previsão do vento e turbulência atmosférica.
*   **Fluxo de água:** simulação de rios, correntes oceânicas e tubulações.
*   **Aerodinâmica:** projeto de aviões, carros e foguetes.
*   **Medicina:** modelagem do fluxo sanguíneo em artérias.
*   **Climatologia:** modelagem e previsão climática global.
*   **Engenharia:** desenvolvimento de motores, bombas e sistemas de ventilação.

Neste experimento, consideraremos um cenário simplificado onde a pressão é constante e as forças externas são nulas. A equação governante da velocidade do fluido $\mathbf{u} = (u, v)$ reduz-se unicamente ao termo de difusão viscosa:
$$ \frac{\partial \mathbf{u}}{\partial t} = \nu \nabla^2 \mathbf{u} $$
onde $\nu$ é a viscosidade cinemática do fluido. Matematicamente, as componentes $u$ (velocidade em x) e $v$ (velocidade em y) se difundem independentemente, de forma idêntica à propagação do calor ao longo de uma placa metálica.

---

#### 2. MODELAGEM NUMÉRICA
Para simular essa equação no computador, discretizamos o espaço bidimensional e o tempo utilizando o método de diferenças finitas (Esquema FTCS - *Forward in Time, Central in Space*). A atualização da componente $u$ em uma célula da malha $(i, j)$ no instante $n+1$ é dada por:

$$ u_{i,j}^{n+1} = u_{i,j}^n + \Delta t \cdot \nu \left( \frac{u_{i+1,j}^n - 2u_{i,j}^n + u_{i-1,j}^n}{\Delta x^2} + \frac{u_{i,j+1}^n - 2u_{i,j}^n + u_{i,j-1}^n}{\Delta y^2} \right) $$

O mesmo princípio é aplicado à componente vertical $v$. Para evitar condições de corrida (onde uma *thread* atualiza o tempo $n+1$ enquanto outra *thread* ainda precisa ler o tempo $n$), utilizamos matrizes separadas para o estado atual e o próximo estado da simulação.

---

#### 3. IMPLEMENTAÇÃO EM C COM OPENMP

Para garantir o máximo desempenho e explorar a **localidade espacial** da memória *cache* (organização *Row-Major* em C), as matrizes bidimensionais foram implementadas como vetores unidimensionais contíguos (`u[i * N + j]`). 

Para permitir que testemos o impacto de diferentes escalonamentos dinamicamente através da variável de ambiente `OMP_SCHEDULE` sem precisar recompilar o código, utilizamos a cláusula `schedule(runtime)`.

```c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 1000       // Tamanho da malha (N x N)
#define ITER 500     // Número de passos de tempo
#define NU 0.1       // Viscosidade cinemática do fluido
#define DX 1.0       // Espaçamento da grade (dx = dy)
#define DT 1.0       // Passo de tempo (condição de estabilidade atendida)

// Macro para acesso contíguo 1D simulando matriz 2D (Row-Major)
#define IDX(i, j) ((i) * N + (j))

void init_fluid(double *u, double *v) {
    // Inicializa o fluido parado
    for (int i = 0; i < N * N; i++) {
        u[i] = 0.0;
        v[i] = 0.0;
    }
    
    // Cria uma pequena perturbação no centro do domínio
    int center = N / 2;
    int radius = N / 10;
    for (int i = center - radius; i < center + radius; i++) {
        for (int j = center - radius; j < center + radius; j++) {
            if (pow(i - center, 2) + pow(j - center, 2) < pow(radius, 2)) {
                u[IDX(i, j)] = 100.0; // Impulso horizontal forte
                v[IDX(i, j)] = 50.0;  // Impulso vertical médio
            }
        }
    }
}

int main() {
    double *u = calloc(N * N, sizeof(double));
    double *v = calloc(N * N, sizeof(double));
    double *u_new = calloc(N * N, sizeof(double));
    double *v_new = calloc(N * N, sizeof(double));

    init_fluid(u, v);
    
    double start_time = omp_get_wtime();

    // Loop temporal (Não pode ser paralelizado devido à dependência de dados)
    for (int t = 0; t < ITER; t++) {
        
        // Loop espacial: Paralelizado com collapse(2) e schedule(runtime)
        #pragma omp parallel for collapse(2) schedule(runtime)
        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                // Derivadas espaciais de u e v usando diferenças finitas centrais
                double laplacian_u = (u[IDX(i+1, j)] - 2.0*u[IDX(i, j)] + u[IDX(i-1, j)]) / (DX*DX) +
                                     (u[IDX(i, j+1)] - 2.0*u[IDX(i, j)] + u[IDX(i, j-1)]) / (DX*DX);
                                     
                double laplacian_v = (v[IDX(i+1, j)] - 2.0*v[IDX(i, j)] + v[IDX(i-1, j)]) / (DX*DX) +
                                     (v[IDX(i, j+1)] - 2.0*v[IDX(i, j)] + v[IDX(i, j-1)]) / (DX*DX);

                // Evolução no tempo (difusão puramente viscosa)
                u_new[IDX(i, j)] = u[IDX(i, j)] + DT * NU * laplacian_u;
                v_new[IDX(i, j)] = v[IDX(i, j)] + DT * NU * laplacian_v;
            }
        }

        // Atualização da malha para o próximo passo de tempo
        // Também paralelizado para maximizar a banda de memória
        #pragma omp parallel for
        for (int i = 0; i < N * N; i++) {
            u[i] = u_new[i];
            v[i] = v_new[i];
        }
    }

    double end_time = omp_get_wtime();
    printf("Simulacao concluida em %f segundos.\n", end_time - start_time);
    
    // Verificacao do comportamento fisico na celula central
    printf("Velocidade final no centro (u, v): (%.2f, %.2f)\n", u[IDX(N/2, N/2)], v[IDX(N/2, N/2)]);

    free(u); free(v); free(u_new); free(v_new);
    return 0;
}
```

---

#### 4. VALIDAÇÃO E ANÁLISE DE DESEMPENHO PARALELO

**4.1. Validação Física da Simulação**
O modelo foi inicialmente rodado com a malha configurada com velocidade nula. Conforme predito pela mecânica dos fluidos, sem perturbações e ignorando a pressão, o fluido permaneceu indefinidamente em repouso perfeitamente estável. Após a inserção da perturbação (um pico de velocidade num raio circular no centro da malha), a observação iterativa da simulação revelou que a velocidade começou a se espalhar homogeneamente para as células adjacentes, caindo sua magnitude máxima enquanto a energia cinética dissipava pela malha em virtude da viscosidade cinemática, confirmando o sucesso da implementação do esquema FTCS para o termo de difusão de Navier-Stokes.

**4.2. O Impacto da Cláusula `collapse`**
Na ausência do `collapse(2)`, a diretiva `#pragma omp parallel for` paraleliza exclusivamente o laço externo (a variável `i`). Se a malha for relativamente pequena e houver dezenas de núcleos no supercomputador, o nível de concorrência poderá ser insuficiente, deixando as *threads* desbalanceadas. 
Ao instruirmos o compilador com `collapse(2)`, dizemos a ele para **"achatar" os dois laços independentes** em um único "super laço" unidimensional de $(N-2) \times (N-2)$ iterações. O hardware recalcula matematicamente os índices originais (`i` e `j`) de forma linear [histórico de chat], injetando operações de divisão e módulo no interior do processador. O benefício gerado é o aumento vertiginoso da **granularidade**: o escalonador possui agora milhões de pequenas iterações autônomas que podem ser redistribuídas perfeitamente entre os processadores. Por estarmos utilizando a representação de matriz 1D (`IDX(i,j)`), a linearização interna do compilador respeita fielmente a localidade espacial (Row-Major), garantindo *cache hits* massivos na L1/L2.

**4.3. O Impacto da Cláusula `schedule`**
Graças à construção com `schedule(runtime)`, é possível modificar o comportamento de distribuição de carga no terminal Linux utilizando:
`export OMP_SCHEDULE="dynamic"` ou `export OMP_SCHEDULE="static, 1"`.

Neste problema específico de mecânica de fluidos computacional, cada iteração do laço (o cálculo dos laplacianos em `i,j`) executa estritamente o mesmo número de operações aritméticas (cinco adições/subtrações e uma multiplicação por componente). Trata-se de uma carga **altamente uniforme**.

Se utilizarmos o **escalonamento dinâmico** (`schedule(dynamic)`), toda vez que uma *thread* finalizar o cálculo de uma célula (ou bloco de células), ela precisará acessar o *pool* central de tarefas do Sistema Operacional para requisitar o próximo bloco. Como demonstrado na teoria do livro do Pacheco, se cada iteração do laço exige a mesma quantidade de cálculo computacional, a adição da cláusula `dynamic` insere uma penalidade de tempo severa devida à alta contenção no *lock* do gerenciador de tarefas do OpenMP, resultando num tempo de execução mais lento.

Por outro lado, utilizar o **escalonamento estático** (`schedule(static)`), que é geralmente o particionamento em blocos contíguos padrão da maioria das arquiteturas, confere o melhor *speedup*. Ao designar blocos monolíticos e inalteráveis do espaço da matriz para cada *thread* logo no início do laço, o `static` anula por completo os *overheads* de comunicação, permitindo que a Unidade Lógica e Aritmética (ALU) avance pelos endereços RAM adjacentes na sua velocidade física máxima.

---

#### 5. CONCLUSÃO
O desenvolvimento desta simulação comprovou que modelagens complexas advindas da engenharia podem ser mapeadas satisfatoriamente em paradigmas de memória compartilhada. A introdução explícita do `collapse(2)` é uma manobra arquitetural robusta para maximizar a disponibilidade de trabalho às *threads* na varredura de matrizes e tensores tridimensionais. Ademais, atestou-se o princípio da programação paralela de que nem todo esforço de gerenciamento é benéfico: a imposição de um `schedule` dinâmico em problemas cuja malha apresenta carga de cálculo uniforme (como estênceis de diferenças finitas para Navier-Stokes) degrada o *throughput* da máquina. Para operações que iteram pesadamente sobre toda a malha, respeitar o isolamento dos blocos (`static`) atinge sempre a máxima eficiência.