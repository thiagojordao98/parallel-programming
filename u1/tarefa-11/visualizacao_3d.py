import numpy as np
import matplotlib.pyplot as plt

# Parâmetros físicos e de simulação
# Reduzimos N no Python para que o cálculo 3D não trave a máquina,
# pois ele fará as iterações de forma puramente procedural (single-thread).
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
            # Equação da esfera
            if (i - center)**2 + (j - center)**2 + (k - center)**2 < radius**2:
                u[i, j, k] = 100.0

# Copia para plotar o estado da perturbação inicial na visualização
u_perturbacao_inicial = np.copy(u)

# 3. Evolução no Tempo em 3D (Diferenças Finitas)
u_new = np.copy(u)
for step in range(iters):
    for i in range(1, N-1):
        for j in range(1, N-1):
            for k in range(1, N-1):
                # Aproximação do Laplaciano 3D
                laplacian = (u[i+1, j, k] - 2*u[i, j, k] + u[i-1, j, k]) / (dx**2) + \
                            (u[i, j+1, k] - 2*u[i, j, k] + u[i, j-1, k]) / (dx**2) + \
                            (u[i, j, k+1] - 2*u[i, j, k] + u[i, j, k-1]) / (dx**2)
                
                # Atualiza o próximo passo temporal
                u_new[i, j, k] = u[i, j, k] + dt * nu * laplacian
                
    # Sincroniza
    u = np.copy(u_new)

# 4. Configuração do Plot (Fatiamento Tomográfico / Slicing)
# Para entender o que acontece dentro do cubo 3D, vamos "fatiá-lo" exatamente
# no meio (eixo Z = center) e desenhar um mapa de calor dessa fatia.
fig, axes = plt.subplots(1, 3, figsize=(15, 5))

# PLOT 1: Fluido Estável (Parado)
im0 = axes[0].imshow(np.zeros((N, N)), cmap='magma', origin='lower', vmin=0, vmax=100)
axes[0].set_title("1. Inicial (Fatia Z Central)")
fig.colorbar(im0, ax=axes[0], shrink=0.5)

# PLOT 2: Esfera Inserida
# u_perturbacao_inicial[:, :, center] pega apenas a matriz 2D (X, Y) correspondente ao centro de Z
im1 = axes[1].imshow(u_perturbacao_inicial[:, :, center], cmap='magma', origin='lower', vmin=0, vmax=100)
axes[1].set_title("2. Esfera Adicionada (Fatia Z)")
fig.colorbar(im1, ax=axes[1], shrink=0.5)

# PLOT 3: Pós-Difusão 3D
im2 = axes[2].imshow(u[:, :, center], cmap='magma', origin='lower', vmin=0, vmax=100)
axes[2].set_title(f"3. Após {iters} passos (Difusão 3D)")
fig.colorbar(im2, ax=axes[2], shrink=0.5)

plt.tight_layout()
plt.show()
