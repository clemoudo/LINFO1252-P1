#!/usr/bin/env python3

import os
import sys
import csv
import numpy as np
import matplotlib.pyplot as plt

# Configuration des métadonnées pour chaque programme
BENCHMARKS = {
    "philosophes": {
        "csv": "data/philosophes.csv",
        "title": "Problème des philosophes (1 000 000 cycles)",
        "xlabel": "Nombre de philosophes (threads)",
        "output_prefix": "philosophes",
        "color": "#1f77b4",  # Bleu
        "ecolor": "#d62728", # Rouge
    },
    "prodcons": {
        "csv": "data/prodcons.csv",
        "title": "Problème des producteurs-consommateurs (131 072 éléments)",
        "xlabel": "Nombre total de threads (N/2 producteurs, N/2 consommateurs)",
        "output_prefix": "prodcons",
        "color": "#2ca02c",  # Vert
        "ecolor": "#d62728", # Rouge
    },
    "read-write": {
        "csv": "data/read-write.csv",
        "title": "Problème des lecteurs-écrivains (6 400 écritures, 25 400 lectures)",
        "xlabel": "Nombre total de threads (N/2 écrivains, N/2 lecteurs)",
        "output_prefix": "read-write",
        "color": "#9467bd",  # Violet
        "ecolor": "#d62728", # Rouge
    },
}


def load_csv(csv_path):
    """Lit un fichier CSV de mesures et retourne (threads, means, stds, all_runs)."""
    threads = []
    means = []
    stds = []
    all_runs = []

    if not os.path.isfile(csv_path):
        return None

    with open(csv_path, mode="r", encoding="utf-8") as f:
        reader = csv.reader(f)
        header = next(reader, None)

        for row in reader:
            if not row or not row[0].strip():
                continue
            nb_threads = int(row[0].strip())
            runs = [float(val.strip()) for val in row[1:] if val.strip()]

            if not runs:
                continue

            mean_time = np.mean(runs)
            std_time = np.std(runs, ddof=1) if len(runs) > 1 else 0.0

            threads.append(nb_threads)
            means.append(mean_time)
            stds.append(std_time)
            all_runs.append(runs)

    return threads, means, stds, all_runs


def plot_benchmark(key, config, output_dir="plots"):
    """Génère et sauvegarde le graphique pour un benchmark donné."""
    csv_path = config["csv"]
    data = load_csv(csv_path)

    if data is None:
        print(f"Fichier introuvable : '{csv_path}' (ignorer)", file=sys.stderr)
        return False

    threads, means, stds, _ = data
    if not threads:
        print(f"Aucune donnée valide dans '{csv_path}'.", file=sys.stderr)
        return False

    print(f"\nRésultats pour '{key}' ({csv_path}) :")
    for t, m, s in zip(threads, means, stds):
        print(f"   - {t:2d} threads : moyenne = {m:.3f} s  |  écart-type = {s:.3f} s")

    os.makedirs(output_dir, exist_ok=True)

    fig, ax = plt.subplots(figsize=(8, 5.5))

    # Tracé avec moyenne et barres d'erreur représentant l'écart-type
    ax.errorbar(
        threads,
        means,
        yerr=stds,
        fmt='-o',
        color=config["color"],
        ecolor=config["ecolor"],
        elinewidth=1.6,
        capsize=5,
        capthick=1.5,
        linewidth=1.8,
        markersize=6,
        label='Temps moyen ± écart-type (5 mesures)'
    )

    # Contrainte de l'énoncé : L'axe des y doit systématiquement commencer à 0
    max_y = max(m + s for m, s in zip(means, stds)) if means else 1.0
    ax.set_ylim(bottom=0, top=max_y * 1.15)

    # Graduation explicite sur les puissances de 2 mesurées
    ax.set_xticks(threads)
    ax.set_xlim(left=0, right=max(threads) + 2)

    # Titres clairs et explicites
    ax.set_xlabel(config["xlabel"], fontsize=11, fontweight="bold", labelpad=8)
    ax.set_ylabel("Temps d'exécution [secondes]", fontsize=11, fontweight="bold", labelpad=8)
    ax.set_title(config["title"], fontsize=12, fontweight="bold", pad=12)

    # Grille et légende
    ax.grid(True, linestyle="--", alpha=0.6)
    ax.legend(loc="best", fontsize=10, framealpha=0.9)

    plt.tight_layout()

    pdf_path = os.path.join(output_dir, f"{config['output_prefix']}.pdf")
    png_path = os.path.join(output_dir, f"{config['output_prefix']}.png")

    fig.savefig(pdf_path)
    fig.savefig(png_path, dpi=300)
    plt.close(fig)

    print(f"   => Graphiques générés : {pdf_path} et {png_path}")
    return True


def main():
    output_dir = "plots"

    # Si un fichier CSV spécifique est passé en argument
    if len(sys.argv) > 1:
        target_csv = sys.argv[1]
        name = os.path.splitext(os.path.basename(target_csv))[0]

        config = BENCHMARKS.get(name, {
            "csv": target_csv,
            "title": f"Évaluation des performances - {name}",
            "xlabel": "Nombre total de threads",
            "output_prefix": name,
            "color": "#1f77b4",
            "ecolor": "#d62728",
        })
        config["csv"] = target_csv
        plot_benchmark(name, config, output_dir=output_dir)
        return

    # Sinon, on traite l'ensemble des 3 benchmarks de la tâche 1.5
    print("======================================================")
    print(" Génération des graphiques de performance (Tâche 1.5)")
    print("======================================================")

    generated = 0
    for key, config in BENCHMARKS.items():
        if plot_benchmark(key, config, output_dir=output_dir):
            generated += 1

    print("\n======================================================")
    print(f" {generated} / {len(BENCHMARKS)} graphiques générés dans le dossier '{output_dir}/'.")
    print("======================================================")


if __name__ == "__main__":
    main()
