import numpy as np
import matplotlib.pyplot as plt

# Parâmetros físicos e de simulação
N = 50           # Tamanho da malha (N x N)
iters = 150      # Número de passos no tempo
nu = 0.1         # Viscosidade cinemática
dt = 1.0         # Passo de tempo
dx = 1.0         # Espaçamento da grade espacial

# 1. Inicializa o fluido parado (velocidade 0)
u = np.zeros((N, N))

# Prepara a figura para plotar os 3 estágios
fig, axes = plt.subplots(1, 3, figsize=(15, 5))

# PLOT 1: Fluido Estável (Parado)
im0 = axes[0].imshow(u, cmap='magma', origin='lower', vmin=0, vmax=100)
axes[0].set_title("1. Inicial (Repouso Estável)")
fig.colorbar(im0, ax=axes[0], shrink=0.5)

# 2. Cria a perturbação no centro do fluido
center = N // 2
radius = 4
for i in range(N):
    for j in range(N):
        if (i - center)**2 + (j - center)**2 < radius**2:
            u[i, j] = 100.0  # Impulso de velocidade no centro

# PLOT 2: Inserção da Perturbação
im1 = axes[1].imshow(u, cmap='magma', origin='lower', vmin=0, vmax=100)
axes[1].set_title("2. Perturbação Adicionada")
fig.colorbar(im1, ax=axes[1], shrink=0.5)

# 3. Evolução no Tempo (Diferenças Finitas - O mesmo cálculo do C)
u_new = np.copy(u)
for _ in range(iters):
    for i in range(1, N-1):
        for j in range(1, N-1):
            # Aproximação do Laplaciano
            laplacian = (u[i+1, j] - 2*u[i, j] + u[i-1, j]) / (dx**2) + \
                        (u[i, j+1] - 2*u[i, j] + u[i, j-1]) / (dx**2)
            
            # Atualiza o próximo passo com o efeito da viscosidade
            u_new[i, j] = u[i, j] + dt * nu * laplacian
            
    # Sincroniza a matriz para o próximo passo
    u = np.copy(u_new)

# PLOT 3: Pós-Difusão
im2 = axes[2].imshow(u, cmap='magma', origin='lower', vmin=0, vmax=100)
axes[2].set_title(f"3. Após {iters} passos (Difusão Viscosa)")
fig.colorbar(im2, ax=axes[2], shrink=0.5)

plt.tight_layout()
plt.show()
