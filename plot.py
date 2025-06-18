import pandas as pd
import matplotlib.pyplot as plt
import os

csv_files = [
    "reward_qlearning_env1.csv",
    "reward_qlearning_env2.csv",
    "reward_sarsa_env1.csv",
    "reward_sarsa_env2.csv"
]

for csv_filename in csv_files:
    if not os.path.exists(csv_filename):
        print(f"Archivo no encontrado: {csv_filename}")
        continue
    data = pd.read_csv(csv_filename)
    plt.figure()
    plt.plot(data['episode'], data['reward'])
    plt.xlabel('Episodio')
    plt.ylabel('Reward acumulado')
    plt.title(f'Curva de aprendizaje ({csv_filename})')
    plt.grid(True)
    plot_filename = os.path.splitext(csv_filename)[0] + ".png"
    plt.savefig(plot_filename)
    plt.close()
    print(f"Gráfica guardada como: {plot_filename}")
