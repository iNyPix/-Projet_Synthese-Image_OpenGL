# Projet_Synthese-Image_OpenGL
Lapin réalisée en C++ avec OpenGL. Projet de L3 informatique. 
# Installation/Exécution (Linux)

1. Installer le debugger : sudo apt-get install gdb
3. Installer les librairies : sudo apt-get install libjpeg62-turbo-dev sudo apt-get install freeglut3-dev sudo apt-get install --reinstall libgl1-mesa-glx
4. Ouvrir le .cbp du projet avec CodeBlocks et inclure les librairies : Project -> build options -> linker settings -> Add GL, glut, GLU, jpeg

# Modelisation du lapin

    Corps
    Jambes
    Epaules
    Bras
    Coude
    Avant-bras
    Tête
        Pupilles
        Yeux
        Nez
        Moustache
        Dents
        Oreilles
        
    Décors
      Carotte
      Gazon

Modelisation d'une primitive

    Ellipsoïde

Utilisation de deux Textures

    Texture plaqué sur chaque face de la primitive.
    Texture enroulé autour de la primitive.

Gérer 2 types de lumières

    Lumière simple
    Lumière type spot

Contrôle de la vue

  Gérer le zoom
  
      Utiliser les touches z et Z pour zoomer sur le lapin.

  Gérer le déplacement de la caméra
  
      Utiliser les flèches du clavier pour déplacer la caméra.

  Gérer 2 animations

      Une animation automatique (rotation des pupilles).
      Une animation avec une touche du clavier (manger la carotte).
