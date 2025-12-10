# 🌡️ Système de Monitoring Intelligent de Température

[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![ENSI](https://img.shields.io/badge/ENSI-Projet%20IoT-red.svg)](https://www.ensi.tn)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

> **Projet académique** - École Nationale des Sciences de l'Informatique (ENSI)  
> Module: Programmation C | Thème: IoT & Contrôleur Climatique

## 📋 Description

Système complet de monitoring de température pour le contrôle climatique dans différents environnements (serre, serveur, maison). Ce projet simule un contrôleur IoT avec alertes intelligentes et visualisation par heat map.

## ✨ Fonctionnalités Complètes

### Les 6 Tâches Principales
- ✅ **Tâche 1**: Gestion de configuration depuis fichier
- ✅ **Tâche 2**: Simulation de capteur (lecture CSV)
- ✅ **Tâche 3**: Moteur de décision automatique
- ✅ **Tâche 4**: Journalisation avec timestamps réels
- ✅ **Tâche 5**: Système d'alerte à 3 niveaux
- ✅ **Tâche 6**: Module statistiques et rapports

### Fonctionnalités Originales
- 🎨 **Interface colorée** - Bannière ASCII + couleurs ANSI
- 🗺️ **Heat Map temporel** - Visualisation 24h × locations
- ⏰ **Timestamps corrects** - Chaque événement a sa vraie date
- 🎯 **Alertes intelligentes** - 3 niveaux d'urgence
- 📊 **Rapports formatés** - Export journal + statistiques

## 🚀 Quick Start

### Compilation
```bash
gcc -Wall -std=c99 -o monitor temperature_monitor_final.c -lm
```

### Exécution
```bash
./monitor
```

## 📁 Structure du Projet

```
temperature-monitoring/
├── temperature_monitor_final.c    # Code principal (~510 lignes)
├── config.txt                     # Configuration (seuils)
├── sensor_data.txt                # 100 lectures simulées
├── Makefile                       # Build automation
├── README.md                      # Cette documentation
├── LICENSE                        # MIT License
└── .gitignore                     # Fichiers à ignorer
```

**Fichiers générés après exécution:**
```
├── journal_evenements.txt         # Log avec dates réelles
└── rapport_journalier.txt         # Synthèse statistique
```

## ⚙️ Configuration (config.txt)

Personnalisez les paramètres du système:

```ini
seuil_min=18.0              # Température minimale acceptable (°C)
seuil_max=28.0              # Température maximale acceptable (°C)
intervalle_mesure=5         # Intervalle entre mesures (secondes)
```

## 🔔 Système d'Alerte

Le système évalue automatiquement chaque lecture et génère des alertes selon l'écart:

| Niveau | Écart | Couleur | Description |
|--------|-------|---------|-------------|
| **Normal** | Dans les seuils | 🟢 Vert | Tout va bien |
| **Avertissement** | 0-4°C | 🟡 Jaune | Attention requise |
| **Alerte Modérée** | 4-8°C | 🟣 Magenta | Action nécessaire |
| **Alerte Critique** | >8°C | 🔴 Rouge | Danger immédiat |

## 📊 Exemple de Sortie

```
╔══════════════════════════════════════════════════════════════╗
║     🌡️  SYSTÈME DE MONITORING INTELLIGENT DE TEMPÉRATURE     ║
║              ENSI - Projet IoT Contrôleur Climatique          ║
╚══════════════════════════════════════════════════════════════╝

═══ INITIALISATION ═══
[OK] Configuration: [18.0°C - 28.0°C]
[OK] 100 lectures chargées

═══ ANALYSE DES DONNÉES ═══
⚠ [AVERTISSEMENT] SERRE_A @ 2024-12-05 10:00:00: 30.25°C (62.3% humidité)
⚠ [ALERTE CRITIQUE] SERVEUR_RACK1 @ 2024-12-05 10:05:00: 37.90°C (55.1% humidité)

═══ VISUALISATION HEAT MAP ═══

╔══════════════════════════════════════════════════════════════╗
║              🌡️  HEAT MAP - TEMPÉRATURE TEMPORELLE           ║
╚══════════════════════════════════════════════════════════════╝

  Légende:  [■] <15°C  [■] 15-18°C  [■] 18-22°C  [■] 22-26°C  [■] 26-30°C  [■] >30°C

  Location        0h    2h    4h    6h    8h   10h   12h   14h   16h   18h   20h   22h
  ──────────────  ────────────────────────────────────────────────────────
  SERRE_A         ██  ██  ██  ██  ██  ██  ██  ██  ██  ██  ██  ██ │ 25.3°C
  SERVEUR_RACK1   ██  ██  ██  ██  ██  ██  ██  ██  ██  ██  ██  ██ │ 28.7°C
  MAISON_SALON    ██  ██  ██  ██  ██  ██  ██  ██  ██  ██  ██  ██ │ 21.5°C
  SERRE_B         ██  ██  ██  ██  ██  ██  ██  ██  ██  ██  ██  ██ │ 24.1°C

═══ RÉSUMÉ ═══
┌─────────────────────────────────────────┐
│ Lectures analysées    : 100             │
│ Température moyenne   : 23.43°C         │
│ Amplitude             : 10.4 - 37.9°C   │
│ Alertes totales       : 15              │
└─────────────────────────────────────────┘

✓ Terminé! Consultez:
  • journal_evenements.txt (événements avec dates correctes)
  • rapport_journalier.txt (statistiques)
```

## 📝 Exemple journal_evenements.txt

Le journal utilise maintenant les **timestamps réels** de chaque lecture:

```
[2024-12-05 10:00:00] [SYSTÈME] Démarrage du monitoring | Valeur: 0.00
[2024-12-05 10:05:00] [ALERTE] AVERTISSEMENT à SERRE_A | Valeur: 30.25
[2024-12-05 10:10:00] [ALERTE] ALERTE CRITIQUE à SERVEUR_RACK1 | Valeur: 37.90
[2024-12-05 10:15:00] [ALERTE] ALERTE MODÉRÉE à SERRE_B | Valeur: 32.15
[2024-12-05 18:45:00] [SYSTÈME] Arrêt du monitoring | Valeur: 0.00
```

## 🧪 Tests

```bash
# Compiler avec warnings
gcc -Wall -Wextra -std=c99 -o monitor temperature_monitor_final.c -lm

# Exécuter
./monitor

# Vérifier les fichiers générés
ls -lh journal_evenements.txt rapport_journalier.txt

# Voir le journal
cat journal_evenements.txt

# Voir le rapport
cat rapport_journalier.txt
```

## 🎓 Concepts C Démontrés

Ce projet illustre plusieurs concepts importants:

- ✅ **Structures** (`struct`) et énumérations (`enum`)
- ✅ **Manipulation de fichiers** (`fopen`, `fgets`, `fprintf`)
- ✅ **Parsing de chaînes** (`sscanf`, `strcmp`, `strftime`)
- ✅ **Tableaux statiques** et gestion mémoire
- ✅ **Pointeurs** et passage par référence
- ✅ **Gestion du temps** (`time.h`, `localtime`)
- ✅ **Codes ANSI** pour terminal coloré

## 📦 Publier sur GitHub

### 1. Initialiser le dépôt local

```bash
cd /chemin/vers/votre/projet
git init
git add .
git commit -m "feat: Temperature monitoring system - ENSI IoT project"
```

### 2. Créer le dépôt sur GitHub

1. Allez sur github.com
2. Cliquez sur "New repository"
3. Nom: `temperature-monitor-ensi`
4. **Laissez vide** (pas de README/gitignore)
5. Cliquez "Create repository"

### 3. Pousser le code

```bash
git remote add origin https://github.com/VOTRE_USERNAME/temperature-monitor-ensi.git
git branch -M main
git push -u origin main
```

## 🔧 Maintenance

### Modifier les seuils

Éditez `config.txt`:
```ini
seuil_min=20.0
seuil_max=30.0
```

### Ajouter des données

Ajoutez des lignes dans `sensor_data.txt`:
```
1733500000,25.5,55.0,NOUVELLE_LOCATION
```

Format: `timestamp,température,humidité,location`

## 👨‍💻 Auteur

**[Votre Nom]**  
École Nationale des Sciences de l'Informatique (ENSI)  
Année Académique 2024-2025  
Email: [votre.email@ensi.tn]

## 📄 Licence

MIT License - Libre d'utilisation pour projets académiques

Copyright (c) 2025 [Votre Nom]

## 🙏 Remerciements

- ENSI - École Nationale des Sciences de l'Informatique
- Département Informatique
- Enseignants du module Programmation C

---

**Version finale** avec heat map complet et journalisation correcte des timestamps ✨
