# ⚪ Melody : IA Othello Performante ⚫

**Melody** est un moteur de jeu d'Othello (Reversi) de haute performance conçu en C++23. Développée pour la compétition, il combine la puissance de la recherche classique et des modèles prédictifs modernes pour offrir un jeu de haut niveau.

---

## 🧠 L'Intelligence de Melody

L'architecture de Melody repose sur une synergie entre algorithmes de recherche de pointe et une évaluation statistique fine :

* Recherche : Algorithme Minimax avec élagage Alpha-Bêta, complété par ProbCut (***Michael Buro***) pour un élagage sélectif basé sur des statistiques de confiance.
* Optimisation : Table de Transposition globale pour la mise en cache des positions et accélération de la recherche.
* Multithreading : Exploitation du parallélisme matériel pour optimiser les performances sur les architectures multicœurs modernes (YBWC).

* Évaluation (Patterns & FM) : Implémentation d'une fonction d'évaluation basée sur les patterns (***Michael Buro***), augmentée d'une surcouche de Machines de Factorisation (***Steffen Rendle***). L'apport "FM" de permet de modéliser les corrélations croisées entre patterns, là où l'approche classique repose sur une simple somme linéaire.

---

## 🔌 Connectivité et Protocoles
Le rôle principal de Melody est d'affronter d'autres moteurs sur internet. Il supporte :
* IOStd : (Interface expérimentale) Pour les tests locaux et l'analyse.
* GGS (Internet Othello Server) : Basé sur un ODK (Othello Development Kit) modernisé, permettant une communication stable et rapide avec le serveur de jeu GGS.

---

## 🛠️ Développement et Installation
Le moteur est spécifiquement optimisé pour l'écosystème Apple afin de maximiser le nombre de nœuds calculés par seconde.

* Environnement : **macOS**
* Architecture : Entièrement optimisé pour les puces **Apple Silicon (série M)**. Utilisation intensive des instructions **NEON (ARM)** pour accélérer les opérations de bitboard et les calculs de l'évaluation FM.
* Langage : C++ (standard `gnu++23`)
* Outils : Projet Xcode (`.xcodeproj`) inclus.

> [!TIP]
> Note de compilation : Pour tirer pleinement parti des optimisations ARM/NEON, compilez impérativement en mode Release via Xcode.

---

## 📜 Remerciements et Citations

> [!IMPORTANT]
> Héritage et Crédits :
> Melody intègre des idées et des segments de logique repris du code d'Edax. Un immense respect et des remerciements à ***Richard Delorme*** et ***Toshihiko Okuhara*** pour leur contribution inestimable à la communauté. 
> Le support GGS s'appuie sur une modernisation de l'ODK original de ***C Welty***.
> Un vif remerciement à la communauté GGS pour ce cadre d'échange d'idées particulièrement stimulant.
> 
---

#### 🚀 Détails techniques de la Release :
Cette version de Melody est distribuée sous forme de binaire universel (Universal Binary), compatible avec les architectures ARM et x86-64.

Architecture ARM : Le code est optimisé pour utiliser intensivement l'extension NEON (SIMD), garantissant des performances accrues sur les processeurs compatibles (Apple Silicon, Raspberry Pi, etc.).

Architecture x86-64 : Pour l'instant, cette version est générique. Elle ne tire pas profit des instructions SIMD spécifiques (comme l'AVX/AVX2 ...).

💡 Appel à la communauté (Extensions SIMD)
Ne disposant pas actuellement du matériel nécessaire pour tester et implémenter les extensions AVX/AVX2 sur x86-64, je suis ouvert aux retours et contributions :

Si vous souhaitez aider à l'implémentation de ces optimisations.

Si vous avez les capacités de test pour valider des builds spécifiques.

🐧 Compatibilité Linux
Contrairement à macOS, aucune version binaire n'est fournie pour Linux afin d'éviter les problèmes de dépendances entre distributions.

Procédure : Veuillez télécharger le Code Source et suivre les instructions de 🛠️ Compilation.md.

Cela garantit que le binaire sera parfaitement optimisé pour votre distribution (Ubuntu, Debian, Arch, etc.) et votre processeur.

#### N'hésitez pas à ouvrir une Issue ou à me contacter pour en discuter !
---


## ⚖️ Licence
Distribué sous licence MIT.