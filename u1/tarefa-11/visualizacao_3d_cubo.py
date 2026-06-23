import numpy as np
import matplotlib.pyplot as plt

# Parâmetros físicos e de simulação 3D
N = 30           # Tamanho da malha (N x N x N)
iters = 50       # Número de passos no tempo
nu = 0.1         # Viscosidade cinemática
dt = 1.0         # Passo de tempo
dx = 1.0         # Espaçamento da grade espacial

# 1. Inicializa o cubo fluido parado (velocidade 0)
u = np.zeros((N, N, N))

# 2. Cria a perturbação 3D (uma esfera de velocidade no centro)
center = N // 2
radius = 4
for i in range(N):
    for j in range(N):
        for k in range(N):
            if (i - center)**2 + (j - center)**2 + (k - center)**2 < radius**2:
                u[i, j, k] = 100.0

u_perturbacao_inicial = np.copy(u)

# 3. Evolução no Tempo em 3D
u_new = np.copy(u)
for step in range(iters):
    for i in range(1, N-1):
        for j in range(1, N-1):
            for k in range(1, N-1):
                laplacian = (u[i+1, j, k] - 2*u[i, j, k] + u[i-1, j, k]) / (dx**2) + \
                            (u[i, j+1, k] - 2*u[i, j, k] + u[i, j-1, k]) / (dx**2) + \
                            (u[i, j, k+1] - 2*u[i, j, k] + u[i, j, k-1]) / (dx**2)
                
                u_new[i, j, k] = u[i, j, k] + dt * nu * laplacian
                
    u = np.copy(u_new)

# 4. Configuração do Plot Volumétrico (Scatter 3D com transparência)
fig = plt.figure(figsize=(15, 5))

def plot_volumetrico(ax, matriz, titulo):
    """
    Função auxiliar para plotar o volume do fluido.
    Filtramos os valores próximos a zero para criar "transparência" no ar 
    e permitir que possamos ver o "miolo" quente do fluido.
    """
    threshold = 1.0 # Ignora velocidades muito baixas para o cubo não ficar opaco
    x, y, z = np.where(matriz > threshold)
    cores = matriz[x, y, z]
    
    if len(x) > 0:
        # Usa um scatter plot 3D, com a cor dada pela intensidade da velocidade
        # e alpha=0.3 para dar um aspecto translúcido de fumaça/fluido
        sc = ax.scatter(x, y, z, c=cores, cmap='magma', alpha=0.3, vmin=0, vmax=100)
        fig.colorbar(sc, ax=ax, shrink=0.5, pad=0.1)
    else:
        # Pinta apenas um ponto 0,0,0 invisível para desenhar as bordas do cubo vazio
        ax.scatter([0], [0], [0], c=[0], cmap='magma', alpha=0.0, vmin=0, vmax=100)

    # Mantém os eixos fixos no tamanho total do cubo
    ax.set_xlim(0, N)
    ax.set_ylim(0, N)
    ax.set_zlim(0, N)
    ax.set_title(titulo)
    ax.set_xlabel('Eixo X')
    ax.set_ylabel('Eixo Y')
    ax.set_zlabel('Eixo Z')

# Subplot 1: Repouso
ax1 = fig.add_subplot(131, projection='3d')
plot_volumetrico(ax1, np.zeros((N, N, N)), "1. Inicial (Parado)")

# Subplot 2: Esfera Original
ax2 = fig.add_subplot(132, projection='3d')
plot_volumetrico(ax2, u_perturbacao_inicial, "2. Esfera Adicionada")

# Subplot 3: Pós-Difusão
ax3 = fig.add_subplot(133, projection='3d')
plot_volumetrico(ax3, u, f"3. Após {iters} passos (Difusão 3D)")

plt.tight_layout()
plt.show()
