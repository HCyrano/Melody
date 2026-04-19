### 🚀 Mode d'installation rapide

Téléchargez `Melody.zip`.
Décompressez l'archive.

>[note]
>Le binaire inclus est un Universal Binary (compatible Intel & Apple Silicon) pour macOS.
>Utilisateurs Linux : Une compilation est nécessaire pour votre architecture (voir section ci-dessous).

#### 🛠️ Compilation (Sources)

 **- Prérequis**
> ##### macOS `xcode-select --install` 
  
> ##### Linux (Ubuntu/Debian) `sudo apt install g++ make`

**- Procédure**

Téléchargez et décompressez le Code source

Ouvrez un terminal et lancez la compilation :
```
cd /chemin/vers/Melody-x.y.z       # se placer dans le dossier
make                               # compiler
```
Résultat attendu :
✅ Build OK → build/Melody  [Darwin/arm64]

Le binaire généré se trouve dans le dossier `build/`

**- Finalisation**

Deplacez le binaire dans Melody/build/Release/
vérifiez les permissions d'exécution.
Exécutez un script `.command`

#### ⚙️ Options de compilation (Make)
| Commande | Action |
| :--- | :--- |
| `make` | Compile (seulement les fichiers modifiés) |
| `make rebuild` | Recompile tout from scratch |
| `make clean` | Supprime tout le dossier build/ |
| `make cleanobj` | Supprime les .o et .d mais garde le binaire |
| `make info` | Affiche la configuration détectée |

>[!Tip]
```
make 2>&1 | tee build.log   	# compile et sauvegarde les erreurs
cat build.log               	# consulter le log
```