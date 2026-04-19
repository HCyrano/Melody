### Mode d'installation

télécharger Melody.zip
décompresser

il vous faudra probablement remplacer le binaire par un compatible pour votre OS & architecture



#### Compilation Melody

- Prérequis (si pas déjà fait)
> ##### macOS
*bash*
`xcode-select --install` 
  
> ##### Linux (Ubuntu/Debian)
*bash*
`sudo apt install g++ make`

télécharger Code source
décompresser

Compilation
*bash*
```
cd /chemin/vers/Melody-x.y.z       # se placer dans le dossier
make                               # compiler
```
- Output attendu :
✅ Build OK → build/Melody  [Darwin/arm64]

Le binaire final est dans Melody-x.y.z/build/

deplacer le binaire dans Melody/build/Release/
vérifier les autorisations
et lancer un "ffo*".command

##### Options disponibles

`make`
Compile (seulement les fichiers modifiés)
`make rebuild`
Recompile tout from scratch
`make clean`
Supprime tout le dossier build/
`make cleanobj`
Supprime les .o et .d mais garde le binaire
`make info`
Affiche la configuration détectée


En cas d'erreur
*bash*
```
make 2>&1 | tee build.log   	# compile et sauvegarde les erreurs
cat build.log               	# consulter le log
```


Recompilation rapide après modification d'un fichier
Make ne recompile que les fichiers modifiés :
*bash*
```
# Tu modifies RXEngine.cpp
make    # recompile uniquement RXEngine.cpp + relink
```