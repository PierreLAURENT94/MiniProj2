# Course Kronos
![](/Inkscape/readmesf.png)
## Qu'est-ce que **Course Kronos** ?
**Course Kronos** permet de **chronométrer** chaque coureur d'une **course** (Tour de France, Marathon, Triathlon, Biathlon...) avec prise en charge des éventuelles **étapes**, tout ça avec via des **cartes PCSC**.
## Pour node faire ceci :
```bat
npm init
npm install electron pcsclite smartcard open
npm i -D electron-rebuild

// Supprimez le dossier node-modules, ainsi que le fichier packages-lock.json

npm i
.\node_modules\.bin\electron-rebuild.cmd

//Ajouter:
//"scripts": {
//    "start": "electron ."
//},
//dans le le fichier packages.json

npm start
```
## Screenshots :
### Passage d'une carte inconnue :
![](/Administratif/imgs/pb.PNG)
### Enregistrement d'une nouvelle carte :
![](/Administratif/imgs/att.PNG)
### Passage d'une carte enregistrée :
![](/Administratif/imgs/ok.PNG)
### Afficheur WEB (Pour un panneau géant par exemple) :
![](/Administratif/imgs/AfficheurWEB.PNG)
## [Compte Rendu](/Administratif/Compte Rendu.pdf) 
> Pierre LAURENT • Vincent LORCY • Nov 2021
