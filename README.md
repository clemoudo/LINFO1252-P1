# LINFO1252 – Projet 1 : Programmation multi-threadée et évaluation de performances

Ce projet s'inscrit dans le cadre du cours **LINFO1252 - Systèmes informatiques** (UCLouvain).  
L'objectif est d'étudier, implémenter et évaluer les performances d'applications concurrentes exploitant les primitives de synchronisation POSIX (mutex et sémaphores), puis d'analyser l'impact du niveau de parallélisme sur une machine locale et sur une machine serveur dédiée 16 cœurs (`studsrv`).

---

## Structure du projet

Conformément à l'arborescence attendue :

```text
.
├── Makefile                     # Fichier de build principal
├── experiments.sh               # Point d'entrée exécuté sur studsrv via INGInious
├── README.md                    # Documentation du projet
├── Rapport.pdf                  # Rapport d'analyse des résultats (à fournir)
├── save_data.sh                 # Utilitaire pour parser STDOUT et alimenter data/
├── graphes/
│   └── generate_plot.py         # Génération des graphiques (moyenne ± écart-type, PDF et PNG)
├── src/
│   └── part1/
│       ├── benchmark_part1.sh   # Script de mesures automatisées (Tâche 1.4)
│       ├── philosophes.c        # Tâche 1.1 : Problème du dîner des philosophes
│       ├── producer_consumer.c  # Tâche 1.2 : Producteurs-Consommateurs avec buffer borné
│       └── reader_writer.c      # Tâche 1.3 : Lecteurs-Écrivains (priorité aux écrivains)
├── data/                        # Fichiers CSV de mesures (ignoré par Git sauf studsrv)
└── bin/                         # Binaires exécutables générés par make (ignoré par Git)
```

---

## Les problèmes implémentés (Partie 1)

1. **Dîner des philosophes (`src/part1/philosophes.c`)**
   - Paramètre : nombre de philosophes $N$.
   - Chaque philosophe réalise 1 000 000 de cycles penser/manger.
   - Les phases penser et manger sont immédiates afin de mettre en évidence le coût pur de la synchronisation par mutex.
   - Prévention des interblocages (*deadlocks*) par bris de symétrie sur l'ordre de prise des baguettes (ressources ordonnées).

2. **Producteurs-Consommateurs (`src/part1/producer_consumer.c`)**
   - Paramètres : nombre de producteurs et nombre de consommateurs.
   - Buffer circulaire borné partagé de 8 cases.
   - Production et consommation totales fixes de 131 072 éléments partagés équitablement entre les threads.
   - Synchronisation via un mutex et deux sémaphores de comptage (`vide` et `plein`).
   - Traitement CPU simulé (`for (int i=0; i<10000; i++);`) **en dehors** de la section critique.

3. **Lecteurs-Écrivains (`src/part1/reader_writer.c`)**
   - Paramètres : nombre d'écrivains et nombre de lecteurs.
   - Priorité stricte aux écrivains (algorithme de Courtois avec `wsem`, `rsem`, `z`, `writecount` et `readcount`).
   - Volumes d'accès : 6 400 écritures et 25 400 lectures au total.
   - Traitement CPU simulé (`for (int i=0; i<10000; i++);`) **à l'intérieur** de la zone critique.

---

## Commandes principales (`Makefile`)

Toutes les opérations courantes sont automatisées via le `Makefile` :

### 1. Compilation
Compile l'ensemble des programmes en C11 avec l'option `-O0` requise pour éviter les optimisations non désirées :
```bash
make
# ou
make all
```
Les exécutables sont générés dans le dossier `bin/` :
- `bin/philosophes`
- `bin/producer_consumer`
- `bin/reader_writer`

### 2. Tests rapides
Vérifie la bonne exécution des 3 binaires avec une configuration minimale :
```bash
make test
```

### 3. Exécution des benchmarks (local)
Compile automatiquement si besoin, exécute les mesures et enregistre les CSV dans `data/` :
```bash
# Lance le benchmark complet (les 3 programmes) :
make benchmark

# Ou pour un seul programme :
make benchmark TARGET=philosophes
make benchmark TARGET=producer_consumer
make benchmark TARGET=reader_writer
```

### 4. Génération des graphiques
Trace les graphiques conformes aux consignes (axe Y commençant à 0, moyenne avec barres d'erreur représentant l'écart-type) :
```bash
# Génère les graphiques pour le dossier studsrv le plus récent (ou data/) :
make plot

# Ou pour cibler un dossier de données spécifique :
make plot DATA=data/studsrv_2026-11-20_11-40
make plot DATA=data
```
Les graphes sont générés aux formats vectoriel `.pdf` (pour le rapport) et image `.png` (résolution 300 DPI) dans le dossier `graphes/`.

### 5. Nettoyage
Supprime le dossier des binaires `bin/` :
```bash
make clean
```

### 6. Préparation de l'archive pour INGInious
Génère une archive `projet1.zip` propre et conforme aux exigences d'INGInious :
```bash
make zip
```

---

## Évaluation des performances

Les configurations mesurées pour chaque programme correspondent aux nombres de threads totaux suivants : `[2, 4, 8, 16, 32]`, avec **5 répétitions** par configuration.

### En local
Le moyen le plus simple est d'utiliser le `Makefile` :
```bash
make benchmark
```
*(Équivalent direct à : `./src/part1/benchmark_part1.sh | ./save_data.sh`)*

### Sur la machine dédiée (`studsrv`) via INGInious
1. Générez l'archive avec `make zip`.
2. Soumettez l'archive sur la tâche INGInious correspondante.
3. Le serveur exécute [`experiments.sh`](file:///home/cvier/UCL/sys-info/Projets/P1-read-write/experiments.sh) et retourne la sortie standard dans le champ de feedback.
4. Récupérez le texte du feedback et enregistrez-le dans un dossier horodaté :
   ```bash
   ./save_data.sh --studsrv < feedback_studsrv.txt
   # ou directement en collant dans le terminal :
   ./save_data.sh --studsrv
   ```
5. Générez les courbes d'analyse associées :
   ```bash
   make plot
   ```
