# ⚪ Melody : IA Othello Performante ⚫

Melody est un moteur de jeu d'Othello (Reversi) de haute performance conçu en C++23. Développée pour la compétition, il combine la puissance de la recherche classique et des modèles prédictifs modernes pour offrir un jeu de haut niveau.

---

## 🧠 L'Intelligence de Melody

L'architecture de Melody repose sur une synergie entre algorithmes de recherche de pointe et une évaluation statistique fine :

* Recherche : Algorithme Minimax avec élagage Alpha-Bêta, complété par ProbCut pour un élagage sélectif basé sur des statistiques de confiance.
* Évaluation (Patterns & FM) : Utilisation de la Méthode Buro (poids par patterns). Melody ajoute une surcouche basée sur les Factorization Machines (FM) de Rendle pour modéliser les interactions complexes entre les patterns.
* Optimisation :Table de Transposition globale pour la mise en cache des positions et accélération de la recherche.

---

## 🔌 Connectivité et Protocoles
Le rôle principal de Melody est d'affronter d'autres moteurs sur internet. Il supporte :
* IOStd : (a completer) Pour les tests locaux et l'analyse.
* GGS (Internet Othello Server) : Basé sur un ODK (Othello Development Kit) modernisé, permettant une communication stable et rapide avec le serveur de jeu GGS.

---

## 🛠️ Développement et Installation
Le moteur est spécifiquement optimisé pour l'écosystème Apple afin de maximiser le nombre de nœuds calculés par seconde.

* Environnement :**macOS**
* Architecture : Entièrement optimisé pour les puces Apple Silicon (série M). Utilisation intensive des instructions NEON (ARM) pour accélérer les opérations de bitboard et les calculs de l'évaluation FM.
* Langage : C++ (standard `gnu++23`)
* Outils : Projet Xcode (`.xcodeproj`) inclus.

> [!TIP]
> Note de compilation : Pour tirer pleinement parti des optimisations ARM/NEON, compilez impérativement en mode Release via Xcode.

---

## 📜 Remerciements et Citations

> [!IMPORTANT]
> Héritage et Crédits
> Melody intègre des idées et des segments de logique repris du code d'Edax. Un immense respect et des remerciements à Richard Delorme et Toshihiko Okuhara pour leur contribution inestimable à la communauté. 
> Le support GGS s'appuie sur une modernisation de l'ODK original de C Welty.
---

## ⚖️ Licence
Distribué sous licence MIT.